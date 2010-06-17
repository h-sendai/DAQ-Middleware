// -*-C++-*-
/*!
 * @file  DAQServiceSVC_impl.h
 * @brief Service implementation header of DAQService.idl
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
    //virtual ~DAQServiceSVC_impl();

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
    void setRunNo(const long run_no);
    long getRunNo();
    void setFatalStatus(const FatalErrorStatus& fatalStaus);

    FatalErrorStatus* getFatalStatus();

private:
    DAQCommand m_command;
    long m_new;
    DAQDone m_done;
    DAQLifeCycleState m_state;
    Status m_status;
    FatalErrorStatus m_fatalStatus;
    NVList m_comp_params;
    long   m_run_no;
};

#endif // DAQSERVICESVC_IMPL_H


