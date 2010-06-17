// -*- C++ -*-
/*!
 * @file EchoMonitor.h
 * @brief EchoMonitor component.
 * @date
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2008
 *     Kazuo Nakayoshi
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 */

#ifndef ECHOMONITOR_H
#define ECHOMONITOR_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>

#ifdef RTM04x
#include "MlfComponent.h"
#include "MyRingBuffer.h"
#endif
/////////////// RTM-1.0.0 //////////////
#include "DaqComponentBase.h"

////////// ROOT Include files //////////
#include "TH1.h"
#include "TCanvas.h"
#include "TImage.h"
///#include "TApplication.h"

/////// Condition Include files ///////
#include "ConditionEchoMonitor.h"

// Service implementation headers
#include "DAQServiceSVC_impl.h"

using namespace RTC;

class EchoMonitor
#ifdef RTM04x
    : public DAQMW::MlfComponent
#endif
    : public DAQMW::DaqComponentBase
{
public:
    EchoMonitor(RTC::Manager* manager);
    ~EchoMonitor();

    // The initialize action (on CREATED->ALIVE transition)
    // formaer rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq m_in_data;
#ifdef RTM04x
    InPort<TimedOctetSeq, MyRingBuffer> m_InPort;
#endif
    InPort<TimedOctetSeq> m_InPort;

    //TimedOctetSeq m_out_data;
    //OutPort<TimedOctetSeq, MyRingBuffer> m_OutPort;

private:
    int daq_dummy();
    int daq_configure();
    int daq_unconfigure();
    int daq_start();
    int daq_run();
    int daq_stop();
    int daq_pause();
    int daq_resume();
    int reset_InPort();

    int parse_params(::NVList* list);
    int set_condition(std::string condition_file, monitorParam *monitorParam);
    BufferStatus m_in_status;
    int m_sampling_rate;
    bool m_debug;

    static const int EVENT_BYTE_SIZE = 4;
    ////////// ROOT Histogram //////////
    TCanvas *m_canvas;
    TH1F    *m_histo;
    int      m_bin;
    double   m_min;
    double   m_max;
    int      m_monitor_update_rate;
    std::string m_hist_png_fname;

    ////////// Condition database //////////
    ///static const std::string CONDITION_FILE;
    std::string m_condition_file;
    monitorParam m_monitorParam;
};


extern "C"
{
  void EchoMonitorInit(RTC::Manager* manager);
};

#endif // ECHOMONITOR_H
