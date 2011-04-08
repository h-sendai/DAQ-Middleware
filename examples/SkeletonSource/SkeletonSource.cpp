// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include "SkeletonSource.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* skeletonsource_spec[] =
{
    "implementation_id", "SkeletonSource",
    "type_name",         "SkeletonSource",
    "description",       "SkeletonSource component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

SkeletonSource::SkeletonSource(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("skeletonsource_out", m_out_data),
      m_recv_byte_size(0),
      m_out_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("skeletonsource_out", m_OutPort);

    init_command_port();
    init_state_table();
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

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

        std::cerr << "sname: " << sname << "  ";
        std::cerr << "value: " << svalue << std::endl;
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

    m_out_status = BUF_SUCCESS;

    return 0;
}

int SkeletonSource::daq_stop()
{
    std::cerr << "*** SkeletonSource::stop" << std::endl;

    return 0;
}

int SkeletonSource::daq_pause()
{
    std::cerr << "*** SkeletonSource::pause" << std::endl;

    return 0;
}

int SkeletonSource::daq_resume()
{
    std::cerr << "*** SkeletonSource::resume" << std::endl;

    return 0;
}

int SkeletonSource::read_data_from_detectors()
{
    int received_data_size = 0;
    /// write your logic here
    return received_data_size;
}

int SkeletonSource::set_data(unsigned int data_byte_size)
{
    unsigned char header[8];
    unsigned char footer[8];

    set_header(&header[0], data_byte_size);
    set_footer(&footer[0]);

    ///set OutPort buffer length
    m_out_data.data.length(data_byte_size + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);
    memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &m_data[0], data_byte_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + data_byte_size]), &footer[0],
           FOOTER_BYTE_SIZE);

    return 0;
}

int SkeletonSource::write_OutPort()
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
    else {
        m_out_status = BUF_SUCCESS; // successfully done
    }

    return 0;
}

int SkeletonSource::daq_run()
{
    if (m_debug) {
        std::cerr << "*** SkeletonSource::run" << std::endl;
    }

    if (check_trans_lock()) {  // check if stop command has come
        set_trans_unlock();    // transit to CONFIGURED state
        return 0;
    }

    if (m_out_status == BUF_SUCCESS) {   // previous OutPort.write() successfully done
        m_recv_byte_size = read_data_from_detectors();
        if (m_recv_byte_size > 0) {
            set_data(m_recv_byte_size); // set data to OutPort Buffer
        }
    }

    if (write_OutPort() < 0) {
        ;     // Timeout. do nothing.
    }
    else {    // OutPort write successfully done
        inc_sequence_num();                     // increase sequence num.
        inc_total_data_size(m_recv_byte_size);  // increase total data byte size
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
