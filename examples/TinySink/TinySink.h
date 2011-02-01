// -*- C++ -*-
/*!
 * @file 
 * @brief
 * @date
 * @author
 *
 */

#ifndef TINYSINK_H
#define TINYSINK_H

#include "DaqComponentBase.h"

using namespace RTC;

class TinySink
    : public DAQMW::DaqComponentBase
{
public:
    TinySink(RTC::Manager* manager);
    ~TinySink();

    // The initialize action (on CREATED->ALIVE transition)
    // former rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq          m_in_data;
    InPort<TimedOctetSeq>  m_InPort;

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
    int reset_InPort();

    unsigned int read_InPort();
    //int online_analyze();
    static const unsigned int RECV_BUFFER_SIZE = 4096;
    unsigned char m_data[RECV_BUFFER_SIZE];

    BufferStatus m_in_status;
    bool m_debug;
};


extern "C"
{
    void TinySinkInit(RTC::Manager* manager);
};

#endif // TINYSINK_H
