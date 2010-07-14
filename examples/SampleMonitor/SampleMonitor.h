// -*- C++ -*-
/*!
 * @file 
 * @brief
 * @date
 * @author
 *
 */

#ifndef SAMPLEMONITOR_H
#define SAMPLEMONITOR_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>

#include "DaqComponentBase.h"
#include "DAQServiceSVC_impl.h" // Service implementation headers

#include <arpa/inet.h> // for ntohl()

////////// ROOT Include files //////////
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TApplication.h"

#include "SampleData.h"

using namespace RTC;

class SampleMonitor
    : public DAQMW::DaqComponentBase
{
public:
    SampleMonitor(RTC::Manager* manager);
    ~SampleMonitor();

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
    int decode_data(struct sampleData* mydata, int size);

    BufferStatus m_in_status;
    //BufferStatus m_out_status;

    ////////// ROOT Histogram //////////
    TCanvas *m_canvas;
    TH1F   **m_hist;
    int      m_bin;
    double   m_min;
    double   m_max;
    int      m_monitor_update_rate;

    static const int CH_NUM = 8;

    bool m_debug;
};


extern "C"
{
    void SampleMonitorInit(RTC::Manager* manager);
};

#endif // SAMPLEMONITOR_H
