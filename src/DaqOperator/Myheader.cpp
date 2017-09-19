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

#include <iostream>
#include "Myheader.h"

using namespace std;

void saveCompname(const string saveCompname)
{
	M_compname = saveCompname;
}   

string recvCompname()
{
	string* mycomp = new string;
	*mycomp = M_compname;
	return mycomp;
}

void saveStatus(const Status_var& saveStatus)
{
	M_status = saveStatus;
}

Status_var* recvStatus()
{
	Status_var* mystatus = new Status_var;
	*mystatus = M_status;
	return mystatus;
}

void saveFatalErrorStatus(const FatalErrorStatus& saveFatalStatus)
{
	M_fatalerrorstatus = saveFatalErrorStatus;
}

FatalErrorStatus* recvFatalStatus()
{
	FatalErrorStatus* myfatalstatus = new FatalErrorStatus;
	*myfatalstatus = M_fatalstatus;
	return myfatalstatus
}

void CompManager::setErrorStatus(const bool saveErrorOccur)
{
    M_err_occur = err_occur;
}

bool CompManager::getErrorStatus()
{
    bool* myerror = new bool;
    *myerror = M_err_occur;
    return myerror;
}
