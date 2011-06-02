
// -*- C++ -*-
/*!
 * @file Parameter.h
 * @brief 
 * @date 1-January-2009
 * @author Yoshiji Yasu (yoshiji.yasu@kek.jp)
 *
 * Copyright (C) 2009-2011
 *     Yoshiji Yasu
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

#ifndef PARAMETER_H
#define PARAMETER_H

#include <iostream>
#include <string>

typedef int (*CallBackFunction)();

/*!
 * @class Parameter
 * @brief Parameter class
 * 
 * This class is a parameter for ParameterServer class
 *
 */
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

#endif // PARAMETER_H
