// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include "SkeletonSink.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::HEADER_DATA_MISMATCH;
using DAQMW::FatalType::FOOTER_DATA_MISMATCH;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* skeletonsink_spec[] =
{
    "implementation_id", "SkeletonSink",
    "type_name",         "SkeletonSink",
    "description",       "SkeletonSink component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

SkeletonSink::SkeletonSink(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("skeletonsink_in",   m_in_data),
      m_in_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("skeletonsink_in",  m_InPort);

    init_command_port();
    init_state_table();
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

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

        std::cerr << "sname: " << sname << "  ";
        std::cerr << "value: " << svalue << std::endl;
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

    return 0;
}

int SkeletonSink::daq_resume()
{
    std::cerr << "*** SkeletonSink::resume" << std::endl;

    return 0;
}

int SkeletonSink::reset_InPort()
{
    int ret = true;
    while(ret == true) {
        ret = m_InPort.read();
    }

    return 0;
}

unsigned int SkeletonSink::read_InPort()
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = m_InPort.read();

    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL
        m_in_status = check_inPort_status(m_InPort);
        if (m_in_status == BUF_TIMEOUT) { // Buffer empty.
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }
        else if (m_in_status == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else {
        recv_byte_size = m_in_data.data.length();
    }

    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

int SkeletonSink::daq_run()
{
    if (m_debug) {
        std::cerr << "*** SkeletonSink::run" << std::endl;
    }

    unsigned int recv_byte_size = read_InPort();
    if (recv_byte_size == 0) { // Timeout
        return 0;
    }

    check_header_footer(m_in_data, recv_byte_size); // check header and footer
    unsigned int event_byte_size = get_event_size(recv_byte_size);

    /////////////  Write component main logic here. /////////////
    // online_analyze();
    /////////////////////////////////////////////////////////////

    inc_sequence_num();                       // increase sequence num.
    inc_total_data_size(event_byte_size);     // increase total data byte size

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
