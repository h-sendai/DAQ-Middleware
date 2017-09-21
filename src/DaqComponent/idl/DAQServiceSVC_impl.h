// -*-C++-*-
/*!
 * @file  DAQServiceSVC_impl.h
 * @brief Service implementation header of DAQService.idl
 * @date 1-January-2008
 * @author Kazuo Nakayoshi (kazuo.nakayoshi@kek.jp)
 * @author Yoshiji Yasu (yoshiji.yasu@kek.jp)
 *
 * Copyright (C) 2008-2011
 *     Kazuo Nakayoshi and Yoshiji Yasu
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

#include "DAQServiceSkel.h"

#ifndef DAQSERVICESVC_IMPL_H
#define DAQSERVICESVC_IMPL_H
 
/*
 * Example class implementing IDL interface DAQService
 */

class DAQServiceSVC_impl
    : public virtual POA_DAQService,
      public virtual PortableServer::RefCountServantBase
{
private:
    // Make sure all instances are built on the heap by making the
    // destructor non-public
    // virtual ~DAQServiceSVC_impl();

public:
    // standard constructor
    DAQServiceSVC_impl();
    virtual ~DAQServiceSVC_impl();

    DAQLifeCycleState getState();
    RTC::ReturnCode_t setCommand(DAQCommand command);
    DAQCommand getCommand();
    DAQDone checkDone();
    void setDone();
    void setStatus(const Status& stat);
    Status* getStatus();
    void setCompParams(const NVList& comp_params);
    NVList* getCompParams();
    void setRunNo(const CORBA::Long run_no);
    CORBA::Long getRunNo();
    void setFatalStatus(const FatalErrorStatus& fatalStaus);
    FatalErrorStatus* getFatalStatus();
    void setErrorStatus(const ErrorStatus& err_occur);
    ErrorStatus* getErrorStatus();
    
private:
    DAQCommand m_command;
    int m_new;
    DAQDone m_done;
    DAQLifeCycleState m_state;
    Status m_status;
    FatalErrorStatus m_fatalStatus;
    NVList m_comp_params;
    CORBA::Long m_run_no;
    ErrorStatus m_err_occur;
};

#endif // DAQSERVICESVC_IMPL_H


