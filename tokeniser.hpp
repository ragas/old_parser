#include <stack>
#include <vector>

enum TokenType {
  ERR, //0
  INTEGER, //1
  PLUS, MINUS, MUL, DIV, LESSTHAN, GREATERTHAN, //2
  LEFTPARAN, RIGHTPARAN, LEFTBRACE, RIGHTBRACE, //8
  LEFTBRACKET, RIGHTBRACKET, //12
  STRING, END, TEMP, TERM, //14
  KINT, ASSIGN //18
};

struct Token {
  TokenType type;
  std::string data;
  Token(TokenType t, std::string d):
    type(t), data(d){}
};

class Tokeniser {
  
private:
  bool is_seperator(char);
  bool saving;
  std::vector<Token> tokens;
  int pos;
  Token tmp;
  Token IsInteger(std::ifstream&);
  Token IsOperator(std::ifstream&);
  Token ErrToken(std::ifstream&);
  Token IsString(std::ifstream&);
  Token IsKeyword(Token t);
public:
  Tokeniser();
  void tokenise(std::string filename);
  Token getNext();
  void rewind();
  bool hasNext();
  Token getCurrent();
  //int save(); void restore(int); 
};
