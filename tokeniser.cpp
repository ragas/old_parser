#include <iostream>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>

#include "tokeniser.hpp"



//Token IsOperator(std::ifstream&);
  
bool Tokeniser::is_seperator(char c){
  if(
     (c == '+')||(c == '=')||
     (c == '-')||
     (c == '*')||
     (c == '/')||(c == ' ')||
     (c == '<')||(c == ';')
     )
    return true;
  return false;
}

Token Tokeniser::IsInteger(std::ifstream& ifs){
  std::stringstream ss;
  char single_char = ifs.peek();;
  while(single_char != EOF){
    if(std::isdigit(single_char)){
      single_char = ifs.get();
      ss << (char)single_char;
      single_char  = ifs.peek();
    }else if(ss.str() != "")
      return Token(TokenType::INTEGER, ss.str());
    else
      return Token(TokenType::ERR, ss.str());      
  }    
  return Token(TokenType::END, ss.str());
}

Token Tokeniser::IsOperator(std::ifstream& ifs){
  std::stringstream ss;
  char single_char;
  if(ifs.get(single_char)){
    ss << single_char;
    if(single_char == '+')
      return Token(TokenType::PLUS, ss.str());
    if(single_char == '-')
      return Token(TokenType::MINUS, ss.str());
    if(single_char == '*')
      return Token(TokenType::MUL, ss.str());
    if(single_char == '/')
      return Token(TokenType::DIV, ss.str());
    if(single_char == '>')
      return Token(TokenType::GREATERTHAN, ss.str());
    if(single_char == '<')
      return Token(TokenType::LESSTHAN, ss.str());
    if(single_char == '(')
      return Token(TokenType::LEFTPARAN, ss.str());
    if(single_char == ')')
      return Token(TokenType::RIGHTPARAN, ss.str());
    if(single_char == '[')
      return Token(TokenType::LEFTBRACKET, ss.str());
    if(single_char == ']')
      return Token(TokenType::RIGHTBRACKET, ss.str());
    if(single_char == '{')
      return Token(TokenType::LEFTBRACE, ss.str());
    if(single_char == '}')
      return Token(TokenType::RIGHTBRACE, ss.str());
    if(single_char == ';')
      return Token(TokenType::TERM, ss.str());
    if(single_char == '=')
      return Token(TokenType::ASSIGN, ss.str());
    else 
      return Token(TokenType::ERR, ss.str());
  }
  return Token(TokenType::END, ss.str());
}

Token Tokeniser::IsString(std::ifstream& ifs){
  std::stringstream ss;
  char single_char = ifs.peek();
  while(single_char != EOF){
    if(is_seperator(single_char))
      if(ss.str() != "")
	return Token(TokenType::STRING, ss.str());
      else return Token(TokenType::ERR,"");
    else{
      ss << single_char;
      ifs.get();
      single_char = ifs.peek();
    }    
  }
  return Token(TokenType::END, ss.str());  
}

Token Tokeniser::IsKeyword(Token t){
  std::string kw = t.data;
  if(kw == "int")
    return Token(TokenType::KINT, kw);
  return t;
}

Token Tokeniser::ErrToken(std::ifstream& ifs){
  std::stringstream ss;
  char single_char;
  while(ifs.get(single_char)){
    if(std::isspace(single_char)){
      return Token(TokenType::ERR, ss.str());
    } else {
      ss << (char)single_char;
    }
  }
  return Token(TokenType::END, ss.str());
}
  
Tokeniser::Tokeniser():pos(0),saving(false), tmp(TokenType::TEMP,""){}

void Tokeniser::tokenise(const std::string filename){
  std::ifstream ifs(filename.c_str(), std::ifstream::in);
  while(ifs.good()){
      
    char c;
    do{
      if(!ifs.get(c))	  
	break;
    }while(std::isspace(c));
    if (c == EOF) break;
    ifs.putback(c);
      
    int pos = ifs.tellg();

    Token t = IsInteger(ifs);
    if(t.type == TokenType::INTEGER){
      tokens.push_back(t);
      continue;
    }
      
    ifs.seekg(pos);
    t = IsOperator(ifs);
      
    if(t.type != TokenType::ERR){
      tokens.push_back(t);
      continue;
    }

    ifs.seekg(pos);
    t = IsString(ifs);
    if(t.type != TokenType::ERR){
      t = IsKeyword(t);
      tokens.push_back(t);
      continue;
    }
    ifs.seekg(pos);
    t = ErrToken(ifs);
    if(t.type == TokenType::END){
      std::cout << "END\n";
    }else{
      std::cout << "Err token " << t.data << "\n";
    }
  }
  for(auto& t : tokens){
    std::cout << t.type << ":" << t.data << "\n";
  }
}

Token Tokeniser::getNext(){
  tmp = tokens.front();
  tokens.erase(tokens.begin());
  return tmp;
}

bool Tokeniser::hasNext(){
  return !tokens.empty();
}

void Tokeniser::rewind(){
  if(tmp.type != TokenType::TEMP)
    tokens.insert(tokens.begin(),tmp);
  tmp.type = TokenType::TEMP;
}

Token Tokeniser::getCurrent(){
  return tmp;
}


