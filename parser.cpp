#include <iostream>

#include <string>
#include <sstream>
#include <cctype>
#include <vector>

#include "tokeniser.hpp"
#include "stable.hpp"

STable syms;

class Expression{
public:
  virtual std::string print() = 0;
  virtual int eval() = 0;
  virtual ~Expression(){};
};

class Int : public Expression{
  std::string data;
public:
  std::string print(){
    return data;
  }

  int eval(){
    return std::stoi(data);
  }

  Int(std::string data_):data(data_){}
};

class OpPlus : public Expression{
  std::string data;
  Expression *left, *right;
public:
  std::string print(){
    std::stringstream ss;
    ss <<  "+ " << "(" << left->print() << ") ,  (" << right->print() << ") ";
    return ss.str();
  }

  int eval(){
    return left->eval() + right->eval();
  }

  OpPlus(Expression* left_, Expression* right_):
    left(left_), right(right_){};
  ~OpPlus(){
    delete(left);
    delete(right);
  }
};

class ExpAssign : public Expression {
  std::string data;
  Expression* left, *right;
public:
  std::string print(){
    std::stringstream ss;
    ss << "= " << left->print() << " "   << right->print();
    return ss.str();
  }

  int eval(){
    int val = right->eval();
    std::cout << "Assign: " << left->print() << ":" << val << "\n";
    syms.addVal(left->print(), val);
    return val;
  }

  ExpAssign(Expression* l, Expression* ri): left(l), right(ri){}
  
};

class ExpSym : public Expression {
  std::string data;
public:
  std::string print(){
      return data;
  }
  
  ExpSym(std::string da):data(da){}
  
  int eval(){
    return syms.getVal(data);
  }
};

class OpMinus : public Expression{
  std::string data;
  Expression* left, *right;
public:
  std::string print(){
    std::stringstream ss;
    ss <<  "- " << "(" << left->print() << ") ,  (" << right->print() << ") ";
    return ss.str();
  }

  int eval(){
    return left->eval() - right->eval();
  }

  OpMinus(Expression* left_, Expression* right_):
    left(left_), right(right_){};
  ~OpMinus(){
    delete(left);
    delete(right);
  }
};

class ExpStatement : public Expression {
  std::vector<Expression*> exps;
public:
  std::string print(){ 
    std::stringstream ss;
    for(auto e : exps)
      ss << e->print() << ";\n";
    return ss.str();
  }

  void add(Expression* e){
     exps.push_back(e);
  }

  int eval(){
    for(auto& e : exps)
      e->eval();
    return 0;
  }
};


// P = ES END
// ES = S; | S;ES
// S  = D | A
// D: int VAR
// A: VAR = E
// E = T T'
// T' = + T T'| - T T'| e
// T = I | VAR| (E);


std::stack<Expression*> stackExp;
Tokeniser t;
bool expression();


bool checkToken(TokenType type){
  Token tt = t.getNext();
  //std::cout << "Matching " << type << " To: " << tt.type << "\n";
  if(tt.type == type){
    if(type == TokenType::INTEGER){
      Expression* e = new Int(t.getCurrent().data);
      stackExp.push(e);
    }
    if(type == TokenType::STRING){
      Expression* e = new ExpSym(t.getCurrent().data);
      stackExp.push(e);
    }
    return true;
  }
  t.rewind();
  return false;  
}

bool isTerm(){
  if(checkToken(TokenType::INTEGER)){
    return true;
  }

  if(checkToken(TokenType::STRING))
     return true;
  if( checkToken(TokenType::LEFTPARAN) 
      && expression() 
      && checkToken(TokenType::RIGHTPARAN)
      ){
    return true;
  }
  return false;
}

bool isTermE(){

  if(checkToken(TokenType::MINUS)){
    Expression* left = stackExp.top();
    stackExp.pop();
    if(isTerm()){
      Expression* right = stackExp.top();
      stackExp.pop();
      Expression* exp = new OpMinus(left, right);
      stackExp.push(exp);
      if(isTermE())
	return true;
    }
    return false;
  }else{

    t.rewind();
    if(checkToken(TokenType::PLUS)){
	Expression* left = stackExp.top();
	stackExp.pop();

	if(isTerm() ){
	Expression* right = stackExp.top();
	stackExp.pop();
	Expression* exp = new OpPlus(left, right);
	stackExp.push(exp);
	if( isTermE())
	  return true;
	}
	return false;
    }else{
      t.rewind();
      return true;
    }    
  }
}
  

bool expression(){
  if(isTerm() && isTermE())
    return true;    
  t.rewind();
  return false;
}

bool isAss(){
  if(checkToken(TokenType::STRING)
     && checkToken(TokenType::ASSIGN)){
    Expression* s = stackExp.top();
    stackExp.pop();    
    if(expression()){
      Expression* e = stackExp.top();
      stackExp.pop();    
      Expression* ass = new ExpAssign(s,e);
      stackExp.push(ass);
      return true;
    }
  }
  return false;
}

bool isDecl(){
  if( checkToken(TokenType::KINT) &&
      checkToken(TokenType::STRING)){
    Expression* right = stackExp.top();
    stackExp.pop();    
    Expression* rig = stackExp.top();
    stackExp.pop();    
    return true;
  }
  return false; 
}

bool isStatement(){
  if(isAss())
    return true;
  if(isDecl())
    return true;
  return false;
}

ExpStatement* stmts;

bool statements(){
  if(isStatement() &&
     checkToken(TokenType::TERM)){
    Expression* right = stackExp.top();
    stackExp.pop();    
    stmts->add(right);
    if (statements())
      return true;
    else if(checkToken(TokenType::END))
      return true;
    return false;
  }
  return false;
}


void Program() {
  stmts = new ExpStatement();
  if(statements()){
    std::cout << "GOOD\n";
    while(!stackExp.empty()){
      Expression* e = stackExp.top();
      stackExp.pop();
      std::cout << e->print() << "\n ";
      std::cout << e->eval() << "\n"; 
      delete(e);
    }

    std::cout << stmts->print();
    std::cout << stmts->eval() << "\n";
  }else {
    std::cout << "BAD " << t.getNext().type << "\n";
  }
}
 



int main(int argc, char** argv) {
  if(argc < 2 ) return 0;
 
  t.tokenise(argv[1]);
  Program();
}
