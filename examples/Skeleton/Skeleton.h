// -*- C++ -*-
/*!
 * @file Skeleton.h
 * @brief Skeleton component.
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

#ifndef SKELETON_H
#define SKELETON_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include "DaqComponentBase.h"
// Service implementation headers
#include "DAQServiceSVC_impl.h"

using namespace RTC;

class Skeleton
    : public DAQMW::DaqComponentBase
{
public:
    Skeleton(RTC::Manager* manager);
    ~Skeleton();

    // The initialize action (on CREATED->ALIVE transition)
    // formaer rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq m_in_data;
    InPort<TimedOctetSeq> m_InPort;

    TimedOctetSeq m_out_data;
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

    int parse_params(::NVList* list);
    int reset_InPort();

    int m_in_status;
    int m_out_status;
    int m_sampling_rate;
    bool m_debug;
};


extern "C"
{
    void SkeletonInit(RTC::Manager* manager);
};

#endif // SKELETON_H
