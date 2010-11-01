// -*- C++ -*-
/*!
 * @file 
 * @brief
 * @date
 * @author
 *
 */

#ifndef BESTEFFORTDISPATCHER_H
#define BESTEFFORTDISPATCHER_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>

#include "DaqComponentBase.h"
#include "DAQServiceSVC_impl.h" // Service implementation headers

using namespace RTC;

class BestEffortDispatcher
    : public DAQMW::DaqComponentBase
{
public:
    BestEffortDispatcher(RTC::Manager* manager);
    ~BestEffortDispatcher();

    // The initialize action (on CREATED->ALIVE transition)
    // former rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq          m_in_data;
    InPort<TimedOctetSeq>  m_InPort;

    TimedOctetSeq          m_out1_data;
    OutPort<TimedOctetSeq> m_OutPort;

    TimedOctetSeq          m_out2_data;
    OutPort<TimedOctetSeq> m_BestEffort_OutPort;

private:
    int daq_dummy();
    int daq_configure();
    int daq_unconfigure();
    int daq_start();
    int daq_run();
    int daq_stop();
    int daq_pause();
    int daq_resume();

    int parse_params(::NVList* list);
    int read_data_from_detectors();
    int set_data_OutPort(unsigned int data_byte_size);
    int set_data_BestEffort_OutPort(unsigned int data_byte_size);
    unsigned int read_InPort();
    int write_OutPort();
    int write_BestEffort_OutPort();

    static const int SEND_BUFFER_SIZE = 4096;
    unsigned char m_data[SEND_BUFFER_SIZE];

    BufferStatus m_in_status;
    BufferStatus m_out_status;
    BufferStatus m_besteffort_out_status;

    unsigned int m_in_tout_counts;
    unsigned int m_out1_tout_counts;
    unsigned int m_out2_tout_counts;

    unsigned int m_inport_recv_data_size;
    bool m_debug;
};


extern "C"
{
    void BestEffortDispatcherInit(RTC::Manager* manager);
};

#endif // BESTEFFORTDISPATCHER_H
