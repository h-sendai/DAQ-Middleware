// -*- C++ -*-
/*!
 * @file 
 * @brief
 * @date
 * @author
 *
 */

#ifndef SAMPLEFILTER_H
#define SAMPLEFILTER_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>

#include "DaqComponentBase.h"
#include "DAQServiceSVC_impl.h" // Service implementation headers

using namespace RTC;

class SampleFilter
    : public DAQMW::DaqComponentBase
{
public:
    SampleFilter(RTC::Manager* manager);
    ~SampleFilter();

    // The initialize action (on CREATED->ALIVE transition)
    // former rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq          m_in_data;
    InPort<TimedOctetSeq> m_InPort;

    TimedOctetSeq          m_out_data;
    OutPort<TimedOctetSeq> m_OutPort;

private:
    int daq_dummy();
    int daq_configure();
    int daq_unconfigure();
    int daq_start();
    int daq_run();
    int daq_stop();
    int daq_pause();
    int daq_resume();
    int print_result();

    int parse_params(::NVList* list);
    int read_data_from_detectors();
    int set_data_OutPort();
    unsigned int read_InPort();
    int write_OutPort();

    BufferStatus m_in_status;
    BufferStatus m_out_status;

    unsigned int m_inport_recv_data_size;
    bool m_debug;
    const static int OUT_EVENT_BUF_SIZE = 8*1024;
    unsigned char m_out_event_data[OUT_EVENT_BUF_SIZE];
    int event_data_filter(unsigned char *, int);
    const static int ONE_EVENT_BYTE_SIZE = 8;
    bool is_good_event(unsigned char *, int);
    
    int m_n_timeout;
};


extern "C"
{
    void SampleFilterInit(RTC::Manager* manager);
};

#endif // SAMPLEFILTER_H
