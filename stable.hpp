#ifndef __STABLE_HPP__
#define __STABLE_HPP__
#include <map>
#include <string>
#include <tuple>

struct ValTbl{
  std::string type;
  int val;
};

class STable{
  std::map<std::string, ValTbl > smap;

public:
  bool addSym(std::string, std::string);//name & type
  bool addVal(std::string, int);//name & val
  int getVal(std::string);
};
#endif
