// -*- C++ -*-
/*!
 * @file myheader.cpp
 * @brief 
 * @date 19-September-2017
 * @author Sai Kohata (b114078@cc.it-hiroshima.ac.jp)
 *
 * Copyright (C) 2017
 *     Sai Kohata
 *     Hiroshima Institute Technology (HIT), Japan.
 *     All rights reserved.
 *
 */

#ifndef MYHEADER_H
#define MYHEADER_H

#include <iostream>
#include "DaqOperator.h"

using namespace std;

// New class
class CompManager
{
public:
	~CompManager();
	
    void saveCompname(const string saveCompname);    
    string recvCompname();
    void saveStatus(const Status_var& saveStatus);
    Status_var* recvStatus();
    void saveFatalErrorStatus(const FatalErrorStatus& saveFatalErrorStatus);
    FatalErrorStatus* recvFatalStatus(); 
    void setErrorStatus(const bool saveErrorOccur);
    bool getErrorStatus();

private:
	string M_compname;
	Status_var M_status;
	FatalErrorStatus* M_fatalerrorstatus;
	bool M_ErrOccur;
};  

#endif // MYHEADER_H
