#ifndef STRING_UTILS
#define STRING_UTILS

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <iostream>
#include <iomanip>
#include <cctype>

void split(const std::string& s, char c, std::vector< std::string >& v ) {
  std::string::size_type i = 0;
  std::string::size_type j = s.find(c);

  while (j != std::string::npos) {
    v.push_back(s.substr(i, j-i));
    i = ++j;
    j = s.find(c,j);

    if(j == std::string::npos) {
      v.push_back(s.substr(i, s.length()));
    }
  }

}

template<typename T>
void rtrim(std::basic_string<T>& s, T c) {
  if(s.empty()) {
    return;
  }

  typename std::basic_string<T>::iterator p;
  for(p = s.end(); p != s.begin() && *--p == c;);
  if( *p != c) {
    p++;
  }
  
  s.erase(p, s.end());
}

std::string xpath2param(std::string xpath)
{
    std::vector< std::string > v;
    split(xpath, '/', v);
    std::string param = v[(v.size() - 1)];
    rtrim(param, ']');
    rtrim(param, '0');
    rtrim(param, '[');   
    
    return param;
}

void toLower(std::basic_string<char>& s) {
  for(std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
    *p = tolower(*p);
  }
}

void toLower(char* s) {
  int len = strlen(s);
  for(int i = 0; i < len; i++) {
    *s = tolower(*s);
    s++;
  }
}

std::string strDec2hex(std::string strDec)
{
    long lval = 0;
    char *offset;
    lval = strtol(strDec.c_str(), &offset, 10);
    std::cerr << "lval=" << lval << std::endl;

    std::stringstream ss;
    ss.str("");
    if (lval > 0) {
	ss << std::showbase << std::hex << lval;
    }
    return ss.str();
}

#endif
