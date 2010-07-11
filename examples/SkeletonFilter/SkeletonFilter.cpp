// -*- C++ -*-
/*!
 * @file  SkeletonFilter.cpp
 * @brief SkeletonFilter component
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

#include "SkeletonFilter.h"

using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
static const char* skeletonfilter_spec[] =
{
    "implementation_id", "SkeletonFilter",
    "type_name",         "SkeletonFilter",
    "description",       "SkeletonFilter component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

SkeletonFilter::SkeletonFilter(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("skeletonfilter_in",   m_in_data),
      m_OutPort("skeletonfilter_out", m_out_data),

      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("skeletonfilter_in",  m_InPort);
    registerOutPort("skeletonfilter_out", m_OutPort);

    init_command_port();
    init_state_table( );
    set_comp_name("SKELETONFILTER");
}

SkeletonFilter::~SkeletonFilter()
{
}


RTC::ReturnCode_t SkeletonFilter::onInitialize()
{
    if (m_debug) {
        std::cerr << "SkeletonFilter::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t SkeletonFilter::onExecute(RTC::UniqueId ec_id)
{
    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int SkeletonFilter::daq_dummy()
{
    return 0;
}

int SkeletonFilter::daq_configure()
{
    std::cerr << "*** SkeletonFilter::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int SkeletonFilter::parse_params(::NVList* list)
{

    std::cerr << "param list length:" << (*list).length() << std::endl;

    int length = (*list).length();
    for (int i = 0; i < length; i++) {
        if (m_debug) {
            std::cerr << "NVList[" << (*list)[i].name
                      << ","<< (*list)[i].value << "]" << std::endl;
        }
    }

    return 0;
}


int SkeletonFilter::daq_unconfigure()
{
    std::cerr << "*** SkeletonFilter::unconfigure" << std::endl;

    return 0;
}

int SkeletonFilter::daq_start()
{
    std::cerr << "*** SkeletonFilter::start" << std::endl;

    m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int SkeletonFilter::daq_stop()
{
    std::cerr << "*** SkeletonFilter::stop" << std::endl;
    reset_InPort();
    return 0;
}

int SkeletonFilter::daq_pause()
{
    std::cerr << "*** SkeletonFilter::pause" << std::endl;
    fatal_error_report(USER_DEFINED_ERROR1, "TEST FOR USER DEFINED ERROR");
    return 0;
}

int SkeletonFilter::daq_resume()
{
    std::cerr << "*** SkeletonFilter::resume" << std::endl;
    return 0;
}

int SkeletonFilter::reset_InPort()
{
    TimedOctetSeq dummy_data;

    // int ret = BUF_SUCCESS;
    // while (ret == BUF_SUCCESS) {
    //    ret = m_InPort.read();
    // }

    std::cerr << "*** SkeletonFilter::InPort flushed\n";
    return 0;
}

int SkeletonFilter::daq_run()
{
    if (m_debug) {
        std::cerr << "*** SkeletonFilter::run" << std::endl;
    }

    if (check_trans_lock()) {  /// got stop command
        set_trans_unlock();
        return 0;
    }

   return 0;
}

extern "C"
{
    void SkeletonFilterInit(RTC::Manager* manager)
    {
        RTC::Properties profile(skeletonfilter_spec);
        manager->registerFactory(profile,
                    RTC::Create<SkeletonFilter>,
                    RTC::Delete<SkeletonFilter>);
    }
};
