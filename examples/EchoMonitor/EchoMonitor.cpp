// -*- C++ -*-
/*!
 * @file  EchoMonitor.cpp
 * @brief EchoMonitor component
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

#include "EchoMonitor.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::HEADER_DATA_MISMATCH;
using DAQMW::FatalType::FOOTER_DATA_MISMATCH;
using DAQMW::FatalType::USER_DEFINED_ERROR1;
using DAQMW::FatalType::USER_DEFINED_ERROR2;
using DAQMW::FatalType::USER_DEFINED_ERROR3;

// Module specification
static const char* echomonitor_spec[] =
{
    "implementation_id", "EchoMonitor",
    "type_name",         "EchoMonitor",
    "description",       "EchoMonitor component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

///const std::string EchoMonitor::CONDITION_FILE = "./condition.json";///by k.n.

EchoMonitor::EchoMonitor(RTC::Manager* manager)
#ifdef RTM04x
    : DAQMW::MlfComponent(manager),
#endif
    : DAQMW::DaqComponentBase(manager),
        m_InPort("echomonitor_in",   m_in_data),
        //m_OutPort("echomonitor_out", m_out_data),

        m_in_status(BUF_SUCCESS),
        //m_out_status(BUF_SUCCESS),

        m_debug(false),
        m_canvas(0),
        m_histo(0),
        m_bin(60),
        m_min(0.0),
        m_max(120.0),
        m_monitor_update_rate(10),
        m_hist_png_fname("myhist.png"),
        m_condition_file("")
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("echomonitor_in",   m_InPort);
    //registerOutPort("echomonitor_out", m_OutPort);

    init_command_port();
    init_state_table( );
#ifdef RTM04x
    set_comp_name(COMP_NONAME);
#endif
    set_comp_name("EchoMonitor"); //// modified
}

EchoMonitor::~EchoMonitor()
{
}


RTC::ReturnCode_t EchoMonitor::onInitialize()
{
    if (m_debug) {
        std::cerr << "EchoMonitor::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t EchoMonitor::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int EchoMonitor::daq_dummy()
{
    return 0;
}

int EchoMonitor::daq_configure()
{
    std::cerr << "*** EchoMonitor::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int EchoMonitor::parse_params(::NVList* list)
{
    bool pngFnameSpecified = false;
    bool conditionFnameSpecified = false;
    std::cerr << "param list length:" << (*list).length() << std::endl;

    int len = (*list).length();

    for (int i = 0; i < len; i+= 2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;
        if (m_debug) {
            std::cerr << "sname: "   << sname << "  ";
            std::cerr << "value: " << svalue << std::endl;
	}

        if ( sname == "hist_png_file_name" ) {
	    pngFnameSpecified = true;
            if (m_debug) { 
                std::cerr << "source addr: " << svalue << std::endl;
            }
            m_hist_png_fname = svalue;
        }
        if ( sname == "condition_file_name" ) {
	    conditionFnameSpecified = true;
            if (m_debug) { 
                std::cerr << "cond file: " << svalue << std::endl;
            }
            m_condition_file = svalue;
        }
    }
    if (!pngFnameSpecified) {
	std::cerr << "PNG file name not specified."
		  << "default file name is used:"
		  << m_hist_png_fname << std::endl;
    }
    if (!conditionFnameSpecified) {
	std::cerr << "Condition file name not specified."
		  << std::endl;
	fatal_error_report(USER_DEFINED_ERROR3, "### ERROR: condition file not specified");
    }

    return 0;
}


int EchoMonitor::daq_unconfigure()
{
    std::cerr << "*** EchoMonitor::unconfigure" << std::endl;

    return 0;
}

int EchoMonitor::set_condition(std::string condition_file, monitorParam *monitorParam)
{
    ConditionEchoMonitor conditionEchoMonitor;
    conditionEchoMonitor.initialize(condition_file);
    if (conditionEchoMonitor.getParam("common_EchoMonitor_", monitorParam)) {
        std::cerr << "condition OK" << std::endl;
    }
    else {
        throw "EchoMonitor condition error";
    }

    return 0;
}

int EchoMonitor::daq_start()
{
    std::cerr << "*** EchoMonitor::start" << std::endl;

    try {
        ///set_condition(CONDITION_FILE, &m_monitorParam);
        set_condition(m_condition_file, &m_monitorParam);
    }
    catch (std::string error_message) {
        std::cerr << error_message << std::endl;
        //fatal_error_report(USER_ERROR1, -1);
	fatal_error_report(USER_DEFINED_ERROR1, (const char*)error_message.c_str());
        return 0;
    }
    catch (...) {
        std::cerr << "unknown error" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR2, "### ERROR: Unknown exception: set_condition()");
        return 0;
    }

    if (m_canvas) {
        delete m_canvas;
        m_canvas = 0;
    }
    m_canvas = new TCanvas("c1", "canvas", 10, 10, 300, 320);

    if (m_histo) {
        delete m_histo;
        m_histo = 0;
    }

    std::cerr << "m_monitorParam.histogram_n_bin" 
	      << m_monitorParam.histogram_n_bin << std::endl;

    std::cerr << "m_monitorParam.histogram_min" 
	      << m_monitorParam.histogram_min << std::endl;

    std::cerr << "m_monitorParam.histogram_max" 
	      << m_monitorParam.histogram_max << std::endl;


    m_histo = new TH1F("histo", "EchoMonitor", 
                m_monitorParam.histogram_n_bin,
                m_monitorParam.histogram_min,
                m_monitorParam.histogram_max);

/*
    if (m_canvas) {
        delete m_canvas;
        m_canvas = 0;
    }
    m_canvas = new TCanvas("c1", "canvas", 10, 10, 300, 320);

    if (m_histo) {
        delete m_histo;
        m_histo = 0;
    }
    m_histo = new TH1F("histo", "EchoMonitor", m_bin, m_min, m_max);
*/

    m_in_status  = BUF_SUCCESS;

    /////////////////////// added ///////////////////////////////////////
    if(!check_dataPort_connections( m_InPort )){
        std::cerr << "### NO Connectors\n";
        fatal_error_report(DATAPATH_DISCONNECTED);
    }
    ////////////////////////////////////////////////////////////////////

    return 0;
}

int EchoMonitor::daq_stop()
{
    std::cerr << "*** EchoMonitor::stop" << std::endl;

    reset_InPort();

    return 0;
}

int EchoMonitor::daq_pause()
{
    std::cerr << "*** EchoMonitor::pause" << std::endl;
    return 0;
}

int EchoMonitor::daq_resume()
{
    std::cerr << "*** EchoMonitor::resume" << std::endl;
    return 0;
}

int EchoMonitor::daq_run()
{
    if (m_debug) {
        std::cerr << "*** EchoMonitor::run" << std::endl;
	std::cerr << "    Loop: " << m_loop << std::endl;
    }

    if (check_trans_lock()) {  // got stop command
        set_trans_unlock();
        return 0;
    }
#ifdef RTM04x
    m_in_status = m_InPort.read(m_in_data);
#endif

    bool ret = m_InPort.read();
    if(!ret) {
	return 0;
    }

    if ((m_in_status == BUF_TIMEOUT) && check_trans_lock()) {
        set_trans_unlock();
        return 0;
    }
    else if (m_in_status == BUF_TIMEOUT) {
        return 0;
    }
    else if (m_in_status == BUF_FATAL) {
        ///fatal_error_report(USER_ERROR1, -1);
	fatal_error_report(INPORT_ERROR);////// new
        return 0;
    }

    /////////////// Get Data Length ///////////////
    unsigned int block_byte_size = m_in_data.data.length();
    if (m_debug) {
	std::cerr << "*** read data size: " << block_byte_size << std::endl;
    }
    unsigned int event_byte_size = block_byte_size - HEADER_BYTE_SIZE - FOOTER_BYTE_SIZE;

    /////////////// Check Header and Footer ///////////////
    unsigned char header[HEADER_BYTE_SIZE];
    unsigned char footer[FOOTER_BYTE_SIZE];
    for (unsigned int i = 0; i < HEADER_BYTE_SIZE; i++) {
        header[i] = m_in_data.data[i];
    }
    if (check_header(header, event_byte_size) == false) {
        std::cerr << "### ERROR: header invalid in EchoMonitor" << std::endl;
        fatal_error_report(HEADER_DATA_MISMATCH);
        ///return 0;
    }
    for (unsigned int i = 0; i < FOOTER_BYTE_SIZE; i++) {
        footer[i] = m_in_data.data[block_byte_size - FOOTER_BYTE_SIZE + i];
    }
#ifdef NO_BESTEFFORT    
    if (check_footer(footer, m_loop) == false) {
        std::cerr << "### ERROR: footer invalid in EchoMonitor" << std::endl;
        fatal_error_report(FOOTER_DATA_MISMATCH);
        ///return 0;
    }
#endif
    ////////// Extract each event data and fill to histogram data //////////
    unsigned int *event_data;
    for (unsigned int i = HEADER_BYTE_SIZE; i < block_byte_size - FOOTER_BYTE_SIZE; i += EVENT_BYTE_SIZE) {
        event_data = (unsigned int *) &m_in_data.data[i];
        m_histo->Fill(*event_data);
    }
    if (m_loop % m_monitor_update_rate == 0) {
        m_histo->Draw();
        ///m_canvas->Update();
	TImage *img = TImage::Create();
       img->FromPad(m_canvas);
       img->WriteImage(m_hist_png_fname.c_str());   
    }

    ////////// One daq_run() with reading data almost done //////////
    m_loop++;
    //m_total_event += event_byte_size / EVENT_BYTE_SIZE;
    m_total_size += event_byte_size;



    return 0;
}

int EchoMonitor::reset_InPort()
{
    TimedOctetSeq dummy_data;

#ifdef RTM04x       
    int ret = BUF_SUCCESS;
    while (ret == BUF_SUCCESS) {

        ret = m_InPort.read(dummy_data);
    }
#endif
    bool ret = true;
    while (ret) {
        ret = m_InPort.read();
    }

    std::cerr << "*** Monitor::InPort flushed\n";
    return 0;
}

extern "C"
{

    void EchoMonitorInit(RTC::Manager* manager)
    {
    RTC::Properties profile(echomonitor_spec);
    manager->registerFactory(profile,
                 RTC::Create<EchoMonitor>,
                 RTC::Delete<EchoMonitor>);
    }
};
