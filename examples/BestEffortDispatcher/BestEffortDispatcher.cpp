// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include "BestEffortDispatcher.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* besteffortdispatcher_spec[] =
{
    "implementation_id", "BestEffortDispatcher",
    "type_name",         "BestEffortDispatcher",
    "description",       "Dispatcher component with best-effort OutPort",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

BestEffortDispatcher::BestEffortDispatcher(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("dispatcher_in", m_in_data),
      m_OutPort("dispatcher_out", m_out1_data),
      m_BestEffort_OutPort("dispatcher_besteffort_out", m_out2_data),
      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),
      m_besteffort_out_status(BUF_SUCCESS),
      m_in_tout_counts(0),
      m_out1_tout_counts(0),
      m_out2_tout_counts(0),
      m_inport_recv_data_size(0),
      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerInPort("dispatcher_in", m_InPort);
    registerOutPort("dispatcher_out1", m_OutPort);
    registerOutPort("dispatcher_out2", m_BestEffort_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("BESTEFFORT_DISPATCHER");
}

BestEffortDispatcher::~BestEffortDispatcher()
{
}

RTC::ReturnCode_t BestEffortDispatcher::onInitialize()
{
    if (m_debug) {
        std::cerr << "BestEffortDispatcher::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t BestEffortDispatcher::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int BestEffortDispatcher::daq_dummy()
{
    return 0;
}

int BestEffortDispatcher::daq_configure()
{
    std::cerr << "*** BestEffortDispatcher::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int BestEffortDispatcher::parse_params(::NVList* list)
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

int BestEffortDispatcher::daq_unconfigure()
{
    std::cerr << "*** BestEffortDispatcher::unconfigure" << std::endl;

    return 0;
}

int BestEffortDispatcher::daq_start()
{
    std::cerr << "*** BestEffortDispatcher::start" << std::endl;
    m_in_status   = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;
    m_besteffort_out_status = BUF_SUCCESS;

    return 0;
}

int BestEffortDispatcher::daq_stop()
{
    std::cerr << "*** BestEffortDispatcher::stop" << std::endl;

    return 0;
}

int BestEffortDispatcher::daq_pause()
{
    std::cerr << "*** BestEffortDispatcher::pause" << std::endl;

    return 0;
}

int BestEffortDispatcher::daq_resume()
{
    std::cerr << "*** BestEffortDispatcher::resume" << std::endl;

    return 0;
}

int BestEffortDispatcher::read_data_from_detectors()
{
    int received_data_size = 0;
    /// write your logic here
    return received_data_size;
}

int BestEffortDispatcher::set_data_OutPort(unsigned int data_byte_size)
{
    ///set OutPort buffer length
    m_out1_data.data.length(data_byte_size);
    memcpy(&(m_out1_data.data[0]), &m_in_data.data[0], data_byte_size);
    return 0;
}

int BestEffortDispatcher::set_data_BestEffort_OutPort(unsigned int data_byte_size)
{
    ///set OutPort buffer length
    m_out2_data.data.length(data_byte_size);
    memcpy(&(m_out2_data.data[0]), &(m_in_data.data[0]), data_byte_size);

    return 0;
}

unsigned int BestEffortDispatcher::read_InPort()
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = m_InPort.read();

    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL
        m_in_status = check_inPort_status(m_InPort);
        if (m_in_status == BUF_TIMEOUT) { // Buffer empty.
            m_in_tout_counts++;
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }
        else if (m_in_status == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else {
        m_in_tout_counts = 0;
        recv_byte_size = m_in_data.data.length();
        m_in_status = BUF_SUCCESS;
    }
    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

int BestEffortDispatcher::write_OutPort()
{
    ////////////////// send data from OutPort  //////////////////
    bool ret = m_OutPort.write();

    //////////////////// check write status /////////////////////
    if (ret == false) {  // TIMEOUT or FATAL
        m_out_status  = check_outPort_status(m_OutPort);
        if (m_out_status == BUF_FATAL) {   // Fatal error
            fatal_error_report(OUTPORT_ERROR);
        }
        if (m_out_status == BUF_TIMEOUT) { // Timeout
            m_out1_tout_counts++;
            return -1;
        }
    }
    m_out1_tout_counts = 0;
    return 0; // successfully done
}

int BestEffortDispatcher::write_BestEffort_OutPort()
{
    ////////////////// send data from OutPort  //////////////////
    bool ret = m_BestEffort_OutPort.write();

    //////////////////// check write status /////////////////////
    if (ret == false) {  // TIMEOUT or FATAL
        m_besteffort_out_status  = check_outPort_status(m_BestEffort_OutPort);
        if (m_besteffort_out_status == BUF_FATAL) {   // Fatal error
            fatal_error_report(OUTPORT_ERROR);
        }
        if (m_besteffort_out_status == BUF_TIMEOUT) { // Timeout
            m_out2_tout_counts++;
            return -1;
        }
    }
    m_out2_tout_counts = 0;
    return 0; // successfully done
}

int BestEffortDispatcher::daq_run()
{
    if (m_debug) {
        std::cerr << "*** BestEffortDispatcher::run" << std::endl;
    }

    std::cerr << "out1_tout_counts:" << m_out2_tout_counts << "  bestEff_tout_counts:" << m_out2_tout_counts << std::endl;

    //if ((m_out_status != BUF_TIMEOUT) && (m_besteffort_out_status != BUF_TIMEOUT)) {
    if (m_out_status != BUF_TIMEOUT) {
        m_inport_recv_data_size = read_InPort();

        if (m_inport_recv_data_size == 0) { // TIMEOUT
            return 0;
        }
        else {
            check_header_footer(m_in_data, m_inport_recv_data_size);
            set_data_OutPort(m_inport_recv_data_size);
            set_data_BestEffort_OutPort(m_inport_recv_data_size);
        }
    }

    if (m_in_status != BUF_TIMEOUT) {
        if (write_OutPort() < 0) { // TIMEOUT
            ; // do nothing
        }
        else {
            m_out_status = BUF_SUCCESS;
        }
    }

    if ((m_in_status != BUF_TIMEOUT) && (m_out_status != BUF_TIMEOUT)) {
        if (write_BestEffort_OutPort() < 0) { // TIMEOUT
            ; // do nothing
        }
        else {
            m_besteffort_out_status = BUF_SUCCESS;
        }
    }

    if ((m_in_status  != BUF_TIMEOUT) &&
        (m_out_status != BUF_TIMEOUT)) {
            inc_sequence_num();                    // increase sequence num.
            unsigned int event_data_size = get_event_size(m_inport_recv_data_size);
            inc_total_data_size(event_data_size);  // increase total data byte size
    }

    return 0;
}

extern "C"
{
    void BestEffortDispatcherInit(RTC::Manager* manager)
    {
        RTC::Properties profile(besteffortdispatcher_spec);
        manager->registerFactory(profile,
                    RTC::Create<BestEffortDispatcher>,
                    RTC::Delete<BestEffortDispatcher>);
    }
};
