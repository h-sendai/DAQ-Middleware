// -*- C++ -*-
/*!
 * @file  SkeletonSource.cpp
 * @brief SkeletonSource component
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

#include "SkeletonSource.h"

using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
static const char* skeletonsource_spec[] =
{
    "implementation_id", "SkeletonSource",
    "type_name",         "SkeletonSource",
    "description",       "SkeletonSource component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

SkeletonSource::SkeletonSource(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("skeletonsource_in",   m_in_data),
      m_OutPort("skeletonsource_out", m_out_data),

      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("skeletonsource_in",  m_InPort);
    registerOutPort("skeletonsource_out", m_OutPort);

    init_command_port();
    init_state_table( );
    set_comp_name("SKELETONSOURCE");
}

SkeletonSource::~SkeletonSource()
{
}


RTC::ReturnCode_t SkeletonSource::onInitialize()
{
    if (m_debug) {
        std::cerr << "SkeletonSource::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t SkeletonSource::onExecute(RTC::UniqueId ec_id)
{
    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int SkeletonSource::daq_dummy()
{
    return 0;
}

int SkeletonSource::daq_configure()
{
    std::cerr << "*** SkeletonSource::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int SkeletonSource::parse_params(::NVList* list)
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


int SkeletonSource::daq_unconfigure()
{
    std::cerr << "*** SkeletonSource::unconfigure" << std::endl;

    return 0;
}

int SkeletonSource::daq_start()
{
    std::cerr << "*** SkeletonSource::start" << std::endl;

    m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int SkeletonSource::daq_stop()
{
    std::cerr << "*** SkeletonSource::stop" << std::endl;
    reset_InPort();
    return 0;
}

int SkeletonSource::daq_pause()
{
    std::cerr << "*** SkeletonSource::pause" << std::endl;
    fatal_error_report(USER_DEFINED_ERROR1, "TEST FOR USER DEFINED ERROR");
    return 0;
}

int SkeletonSource::daq_resume()
{
    std::cerr << "*** SkeletonSource::resume" << std::endl;
    return 0;
}

int SkeletonSource::reset_InPort()
{
    TimedOctetSeq dummy_data;

    // int ret = BUF_SUCCESS;
    // while (ret == BUF_SUCCESS) {
    //    ret = m_InPort.read();
    // }

    std::cerr << "*** SkeletonSource::InPort flushed\n";
    return 0;
}

int SkeletonSource::daq_run()
{
    if (m_debug) {
        std::cerr << "*** SkeletonSource::run" << std::endl;
    }

    if (check_trans_lock()) {  /// got stop command
        set_trans_unlock();
        return 0;
    }

   return 0;
}

extern "C"
{
    void SkeletonSourceInit(RTC::Manager* manager)
    {
        RTC::Properties profile(skeletonsource_spec);
        manager->registerFactory(profile,
                    RTC::Create<SkeletonSource>,
                    RTC::Delete<SkeletonSource>);
    }
};
