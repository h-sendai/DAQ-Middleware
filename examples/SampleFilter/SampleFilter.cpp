// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include "SampleFilter.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;
using DAQMW::FatalType::USER_DEFINED_ERROR2;

// Module specification
// Change following items to suit your component's spec.
static const char* samplefilter_spec[] =
{
    "implementation_id", "SampleFilter",
    "type_name",         "SampleFilter",
    "description",       "SampleFilter component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

SampleFilter::SampleFilter(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("samplefilter_in", m_in_data),
      m_OutPort("samplefilter_out", m_out_data),
      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),
      m_inport_recv_data_size(0),
      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerInPort("samplefilter_in", m_InPort);
    registerOutPort("samplefilter_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("SAMPLEFILTER");
}

SampleFilter::~SampleFilter()
{
}

RTC::ReturnCode_t SampleFilter::onInitialize()
{
    if (m_debug) {
        std::cerr << "SampleFilter::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t SampleFilter::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int SampleFilter::daq_dummy()
{
    return 0;
}

int SampleFilter::daq_configure()
{
    std::cerr << "*** SampleFilter::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int SampleFilter::parse_params(::NVList* list)
{
    std::cerr << "param list length:" << (*list).length() << std::endl;

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

        std::cerr << "sname: " << sname  << "  ";
        std::cerr << "value: " << svalue << std::endl;
    }

    return 0;
}

int SampleFilter::daq_unconfigure()
{
    std::cerr << "*** SampleFilter::unconfigure" << std::endl;

    return 0;
}

int SampleFilter::daq_start()
{
    std::cerr << "*** SampleFilter::start" << std::endl;

    m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int SampleFilter::daq_stop()
{
    std::cerr << "*** SampleFilter::stop" << std::endl;

    return 0;
}

int SampleFilter::daq_pause()
{
    std::cerr << "*** SampleFilter::pause" << std::endl;

    return 0;
}

int SampleFilter::daq_resume()
{
    std::cerr << "*** SampleFilter::resume" << std::endl;

    return 0;
}

bool SampleFilter::is_good_event(unsigned char *one_event, int size)
{
    // Sample criterion (if module number is less than 5, then ok)
    if (one_event[2] < 5) {
        return true;
    }
    return false;
}

int SampleFilter::event_data_filter(unsigned char *event_data, int event_data_size)
{
    int good_event_byte_size = 0;
    for (int i = 0; i < event_data_size; i += ONE_EVENT_BYTE_SIZE) {
        if (is_good_event(&event_data[i], ONE_EVENT_BYTE_SIZE)) {
            memcpy(&(m_out_event_data[good_event_byte_size]), &(event_data[i]), ONE_EVENT_BYTE_SIZE);
            good_event_byte_size += ONE_EVENT_BYTE_SIZE;
        }
    }

    return good_event_byte_size;
}

int SampleFilter::set_data_OutPort()
{
    // filtering.
    // event_data_filter()
    // Return value: Filtered event data size in bytes.
    //               Good event data is stored in m_out_event_data
    int all_event_byte_size = get_event_size(m_inport_recv_data_size);
    int event_data_byte_size = event_data_filter(&(m_in_data.data[HEADER_BYTE_SIZE]), all_event_byte_size);
    if (event_data_byte_size < 0) {
        fatal_error_report(USER_DEFINED_ERROR1, "SampleFilter error");
    }

    unsigned char header[HEADER_BYTE_SIZE];
    unsigned char footer[FOOTER_BYTE_SIZE];
    set_header(header, event_data_byte_size);
    set_footer(footer);

    m_out_data.data.length(event_data_byte_size + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);
    memcpy(&(m_out_data.data[0]), header, HEADER_BYTE_SIZE);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), m_out_event_data, event_data_byte_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + event_data_byte_size]), footer, FOOTER_BYTE_SIZE);

    return 0;
}

unsigned int SampleFilter::read_InPort()
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
        m_in_status = BUF_SUCCESS;
    }
    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

int SampleFilter::write_OutPort()
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
            return -1;
        }
    }

    return 0; // successfully done
}

int SampleFilter::daq_run()
{
    if (m_debug) {
        std::cerr << "*** SampleFilter::run" << std::endl;
    }

    if (m_out_status != BUF_TIMEOUT) {
        m_inport_recv_data_size = read_InPort();

        if (m_inport_recv_data_size == 0) { // TIMEOUT
            return 0;
        }
        else {
            check_header_footer(m_in_data, m_inport_recv_data_size);
            //set_data_OutPort(m_inport_recv_data_size);
            set_data_OutPort();
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
        inc_sequence_num();                    // increase sequence num.
        unsigned int event_data_size = get_event_size(m_inport_recv_data_size);
        inc_total_data_size(event_data_size);  // increase total data byte size
    }

    return 0;
}

extern "C"
{
    void SampleFilterInit(RTC::Manager* manager)
    {
        RTC::Properties profile(samplefilter_spec);
        manager->registerFactory(profile,
                    RTC::Create<SampleFilter>,
                    RTC::Delete<SampleFilter>);
    }
};
