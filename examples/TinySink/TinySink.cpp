// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include "TinySink.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::HEADER_DATA_MISMATCH;
using DAQMW::FatalType::FOOTER_DATA_MISMATCH;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* tinysink_spec[] =
{
    "implementation_id", "TinySink",
    "type_name",         "TinySink",
    "description",       "TinySink component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TinySink::TinySink(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("tinysink_in",   m_in_data),
      m_in_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("tinysink_in",  m_InPort);

    init_command_port();
    init_state_table();
    set_comp_name("TINYSINK");
}

TinySink::~TinySink()
{
}

RTC::ReturnCode_t TinySink::onInitialize()
{
    if (m_debug) {
        std::cerr << "TinySink::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TinySink::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TinySink::daq_dummy()
{
    return 0;
}

int TinySink::daq_configure()
{
    std::cerr << "*** TinySink::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int TinySink::parse_params(::NVList* list)
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

int TinySink::daq_unconfigure()
{
    std::cerr << "*** TinySink::unconfigure" << std::endl;

    return 0;
}

int TinySink::daq_start()
{
    std::cerr << "*** TinySink::start" << std::endl;

    m_in_status  = BUF_SUCCESS;

    return 0;
}

int TinySink::daq_stop()
{
    std::cerr << "*** TinySink::stop" << std::endl;
    reset_InPort();

    return 0;
}

int TinySink::daq_pause()
{
    std::cerr << "*** TinySink::pause" << std::endl;

    return 0;
}

int TinySink::daq_resume()
{
    std::cerr << "*** TinySink::resume" << std::endl;

    return 0;
}

int TinySink::reset_InPort()
{
    int ret = true;
    while(ret == true) {
        ret = m_InPort.read();
    }

    return 0;
}

unsigned int TinySink::read_InPort()
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

int TinySink::daq_run()
{
    if (m_debug) {
        std::cerr << "*** TinySink::run" << std::endl;
    }

    unsigned int recv_byte_size = read_InPort();
    if (recv_byte_size == 0) { // Timeout
        return 0;
    }

    check_header_footer(m_in_data, recv_byte_size); // check header and footer
    unsigned int event_byte_size = get_event_size(recv_byte_size);

    /////////////  Write component main logic here. /////////////
    // online_analyze();
    if (event_byte_size > RECV_BUFFER_SIZE) {
        fatal_error_report(USER_DEFINED_ERROR1, "Length Too Large");
    }
    memcpy(m_data, &m_in_data.data[HEADER_BYTE_SIZE], event_byte_size);
    for (unsigned int i = 0; i < event_byte_size; i++) {
        fprintf(stderr, "%02X ", m_data[i]);
        if ((i + 1) % 16 == 0) {
            fprintf(stderr, "\n");
        }
    }
    /////////////////////////////////////////////////////////////

    inc_sequence_num();                       // increase sequence num.
    inc_total_data_size(event_byte_size);     // increase total data byte size

    return 0;
}

extern "C"
{
    void TinySinkInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tinysink_spec);
        manager->registerFactory(profile,
                    RTC::Create<TinySink>,
                    RTC::Delete<TinySink>);
    }
};
