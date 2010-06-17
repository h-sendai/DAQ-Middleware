#include <iostream>
#include <string>
//#include "ParameterServer.h"

typedef int (*CallBackFunction)();
//typedef int (*CallBackFunction)(ParameterServer s);
class Parameter {
 public:
  void set( std::string* valueP, CallBackFunction call ){
    m_valuePointer = valueP;
    m_callBackFunc = call;
  };
  std::string* getValueP() {
    return m_valuePointer;
  };
  CallBackFunction getCallBackFunc() {
    return m_callBackFunc;
  };
 private:
  std::string* m_valuePointer;
  CallBackFunction m_callBackFunc;
};
