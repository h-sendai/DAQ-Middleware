// -*- C++ -*-
/*!
 * @file  Skeleton.cpp
 * @brief Skeleton component
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

#include "Skeleton.h"

using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
static const char* skeleton_spec[] =
{
    "implementation_id", "Skeleton",
    "type_name",         "Skeleton",
    "description",       "Skeleton component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

Skeleton::Skeleton(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("skeleton_in",   m_in_data),
      m_OutPort("skeleton_out", m_out_data),

      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("skeleton_in",  m_InPort);
    registerOutPort("skeleton_out", m_OutPort);

    init_command_port();
    init_state_table( );
    set_comp_name("SKELETON");
}

Skeleton::~Skeleton()
{
}


RTC::ReturnCode_t Skeleton::onInitialize()
{
    if (m_debug) {
        std::cerr << "Skeleton::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t Skeleton::onExecute(RTC::UniqueId ec_id)
{
    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int Skeleton::daq_dummy()
{
    return 0;
}

int Skeleton::daq_configure()
{
    std::cerr << "*** Skeleton::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int Skeleton::parse_params(::NVList* list)
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


int Skeleton::daq_unconfigure()
{
    std::cerr << "*** Skeleton::unconfigure" << std::endl;

    return 0;
}

int Skeleton::daq_start()
{
    std::cerr << "*** Skeleton::start" << std::endl;

    m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int Skeleton::daq_stop()
{
    std::cerr << "*** Skeleton::stop" << std::endl;
    reset_InPort();
    return 0;
}

int Skeleton::daq_pause()
{
    std::cerr << "*** Skeleton::pause" << std::endl;
    fatal_error_report(USER_DEFINED_ERROR1, "TEST FOR USER DEFINED ERROR");
    return 0;
}

int Skeleton::daq_resume()
{
    std::cerr << "*** Skeleton::resume" << std::endl;
    return 0;
}

int Skeleton::reset_InPort()
{
    TimedOctetSeq dummy_data;
/*
    int ret = BUF_SUCCESS;
    while( ret == BUF_SUCCESS) {
        ret = m_InPort.read();
    }
*/
    std::cerr << "*** Skeleton::InPort flushed\n";
    return 0;
}

int Skeleton::daq_run()
{
    if (m_debug) {
        std::cerr << "*** Skeleton::run" << std::endl;
    }

    if (check_trans_lock()) {  /// got stop command
        set_trans_unlock();
        return 0;
    }

   return 0;
}

extern "C"
{
    void SkeletonInit(RTC::Manager* manager)
    {
        RTC::Properties profile(skeleton_spec);
        manager->registerFactory(profile,
                    RTC::Create<Skeleton>,
                    RTC::Delete<Skeleton>);
    }
};
