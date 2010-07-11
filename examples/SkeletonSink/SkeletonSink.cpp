// -*- C++ -*-
/*!
 * @file  SkeletonSink.cpp
 * @brief SkeletonSink component
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

#include "SkeletonSink.h"

using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
static const char* skeletonsink_spec[] =
{
    "implementation_id", "SkeletonSink",
    "type_name",         "SkeletonSink",
    "description",       "SkeletonSink component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

SkeletonSink::SkeletonSink(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("skeletonsink_in",   m_in_data),
      m_OutPort("skeletonsink_out", m_out_data),

      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("skeletonsink_in",  m_InPort);
    registerOutPort("skeletonsink_out", m_OutPort);

    init_command_port();
    init_state_table( );
    set_comp_name("SKELETONSINK");
}

SkeletonSink::~SkeletonSink()
{
}


RTC::ReturnCode_t SkeletonSink::onInitialize()
{
    if (m_debug) {
        std::cerr << "SkeletonSink::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t SkeletonSink::onExecute(RTC::UniqueId ec_id)
{
    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int SkeletonSink::daq_dummy()
{
    return 0;
}

int SkeletonSink::daq_configure()
{
    std::cerr << "*** SkeletonSink::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int SkeletonSink::parse_params(::NVList* list)
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


int SkeletonSink::daq_unconfigure()
{
    std::cerr << "*** SkeletonSink::unconfigure" << std::endl;

    return 0;
}

int SkeletonSink::daq_start()
{
    std::cerr << "*** SkeletonSink::start" << std::endl;

    m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int SkeletonSink::daq_stop()
{
    std::cerr << "*** SkeletonSink::stop" << std::endl;
    reset_InPort();
    return 0;
}

int SkeletonSink::daq_pause()
{
    std::cerr << "*** SkeletonSink::pause" << std::endl;
    fatal_error_report(USER_DEFINED_ERROR1, "TEST FOR USER DEFINED ERROR");
    return 0;
}

int SkeletonSink::daq_resume()
{
    std::cerr << "*** SkeletonSink::resume" << std::endl;
    return 0;
}

int SkeletonSink::reset_InPort()
{
    TimedOctetSeq dummy_data;

    // int ret = BUF_SUCCESS;
    // while (ret == BUF_SUCCESS) {
    //    ret = m_InPort.read();
    // }

    std::cerr << "*** SkeletonSink::InPort flushed\n";
    return 0;
}

int SkeletonSink::daq_run()
{
    if (m_debug) {
        std::cerr << "*** SkeletonSink::run" << std::endl;
    }

    if (check_trans_lock()) {  /// got stop command
        set_trans_unlock();
        return 0;
    }

   return 0;
}

extern "C"
{
    void SkeletonSinkInit(RTC::Manager* manager)
    {
        RTC::Properties profile(skeletonsink_spec);
        manager->registerFactory(profile,
                    RTC::Create<SkeletonSink>,
                    RTC::Delete<SkeletonSink>);
    }
};
