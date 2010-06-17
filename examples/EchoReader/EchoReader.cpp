// -*- C++ -*-
/*!
 * @file  EchoReader.cpp
 * @brief EchoReader component
 * @date 2009/07/17
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2009
 *     Kazuo Nakayoshi
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 */

#include "EchoReader.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::USER_DEFINED_ERROR1;
using DAQMW::FatalType::USER_DEFINED_ERROR2;

// Module specification
static const char* echoReader_spec[] =
{
    "implementation_id", "EchoReader",
    "type_name",         "EchoReader",
    "description",       "EchoReader component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

///const std::string EchoReader::HOSTADDR = "127.0.0.1"; ///localhost
///const unsigned int EVENT_BYTE_SIZE = 4;

EchoReader::EchoReader(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("echoReader_out", m_out_data),
      m_out_status(BUF_SUCCESS),
      m_sock(0), 
      m_dataByteSize(400),///100 events
      m_wdata(0),
      m_rdata(0),
      m_srcAddr(""),
      m_srcPort(7),
      m_debug(true)
{
    // Registration: InPort/OutPort/Service

    registerOutPort("echoReader_out", m_OutPort);

    init_command_port();
    init_state_table( );
    set_comp_name("EchoReader");

    ///m_eventByteSize = EVENT_BYTE_SIZE;
}

EchoReader::~EchoReader()
{
    if(m_wdata) {
	delete [] m_wdata;
	m_wdata = 0;
    }
    if(m_rdata) {
	delete [] m_rdata;
	m_rdata = 0;
    }
}


RTC::ReturnCode_t EchoReader::onInitialize()
{
    if (m_debug) {
	std::cerr << "EchoReader::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t EchoReader::onExecute(RTC::UniqueId ec_id)
{
    daq_do();
    return RTC::RTC_OK;
}

int EchoReader::daq_dummy() 
{
    return 0;
}

int EchoReader::daq_configure() 
{
    std::cerr << "*** EchoReader::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    m_rdata = new unsigned char[m_dataByteSize];
    m_wdata = new unsigned char[m_dataByteSize];
    return 0;
}

int EchoReader::parse_params(::NVList* list)
{
    bool srcAddrSpecified = false;
    bool srcPortSpecified = false;
    std::cerr << "param list length:" << (*list).length() << std::endl;

    int len = (*list).length();

    for (int i = 0; i < len; i+= 2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;
        if (m_debug) {
            std::cerr << "sname: "   << sname << "  ";
            std::cerr << "value: " << svalue << std::endl;
        }

        if ( sname == "srcAddr" ) {
	    srcAddrSpecified = true;
            if (m_debug) { 
                std::cerr << "source addr: " << svalue << std::endl;
            }
            m_srcAddr = svalue;
        }
        if ( sname == "srcPort" ) {
	    srcPortSpecified = true;
            if (m_debug) { 
                std::cerr << "source port: " << svalue << std::endl;
            }
	    char* offset;
            m_srcPort = (int)strtol(svalue.c_str(), &offset, 10);
        }
    }
    if(!srcAddrSpecified) {
	std::cerr << "### ERROR:data source address not specified\n";
	fatal_error_report(USER_DEFINED_ERROR1, "NO SRC ADDRESS");
    }
    if(!srcPortSpecified) {
	std::cerr << "### ERROR:data source port not specified\n";
	fatal_error_report(USER_DEFINED_ERROR1, "NO SRC PORT");
    }

    return 0;
}

int EchoReader::daq_unconfigure() 
{
    std::cerr << "*** EchoReader::unconfigure" << std::endl;
    return 0;
}

int EchoReader::daq_start() 
{
    std::cerr << "*** EchoReader::start" << std::endl;
 
    ///m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    try {
	/// Create socket and connect to echo server.
	m_sock = new DAQMW::Sock(); 
	//m_sock->connect(HOSTADDR, PORTNO);
	m_sock->connect(m_srcAddr, m_srcPort);
    } catch (DAQMW::SockException& e) {
        std::cerr << "Sock Fatal Error : " << e.what() << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    } catch (...) {
        std::cerr << "Sock Fatal Error : Unknown" << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    }

    //const std::vector<OutPortConnector*>& conn_list = m_OutPort.connectors();
    //std::cerr << "### Num of Connectors: " << conn_list.size() << std::endl;
    bool outport_conn = check_dataPort_connections( m_OutPort );
    if(!outport_conn) {
	std::cerr << "### NO Connectors\n";
	//fatal_error_report(DAQMW::FatalType::DATAPATH_DISCONNECTED, -1);
	fatal_error_report(DATAPATH_DISCONNECTED);
    }
    return 0;
}

int EchoReader::daq_stop() 
{
    std::cerr << "*** EchoReader::stop" << std::endl;
    /// disconnect from echo server
    m_sock->disconnect(); 
    delete m_sock;        
    m_sock = 0;           

    return 0;
}

int EchoReader::daq_pause()
{
    std::cerr << "*** EchoReader::pause" << std::endl;
    return 0;
}

int EchoReader::daq_resume()
{
    std::cerr << "*** EchoReader::resume" << std::endl;
    return 0;
}

/// generates a Gaussian distribution(mean: 60, sigma: 10) number
/// using uniform random numbers.
unsigned int EchoReader::getGauss()
{
    double gdat = 0.0;
    double amp  = 10.0;
    int loop = 12; 
    for (int i = 0; i< loop; i++) {
	gdat = gdat + (double)random()/((double)RAND_MAX + 1);
    }
    return (unsigned)(gdat * amp);
}

/// send data to echo server 
int EchoReader::send_data_to_echoServer()
{
    /// generates 100 events
    for (int i = 0; i < m_dataByteSize; i+=4) {
	unsigned int gdat = getGauss();
	*(unsigned*)&m_wdata[i] = gdat;
	//*(unsigned*)&m_wdata[i] = htonl(gdat);
    }

    /// send to echo server
    int status = m_sock->writeAll(m_wdata, m_dataByteSize);
    if(status == DAQMW::Sock::ERROR_FATAL) {
        std::cerr << "fatal error..." << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    }
    else if(status == DAQMW::Sock::ERROR_TIMEOUT) {
	std::cerr << "Timeout.. retry..." << std::endl;
	fatal_error_report(USER_DEFINED_ERROR2, "SOCKET TIMEOUT");
    }
    return 0;
}

/// receive data from echo server
int EchoReader::recv_data_from_echoServer()
{
    /// read 100 events data from echo server
    int status = m_sock->readAll(m_rdata, m_dataByteSize);
    if(status == DAQMW::Sock::ERROR_FATAL) {
        std::cerr << "fatal error..." << std::endl;
	//fatal_error_report(USER_ERROR1, -1);
	fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    }
    else if(status == DAQMW::Sock::ERROR_TIMEOUT) {
	std::cerr << "Timeout.. retry..." << std::endl;
	//fatal_error_report(USER_ERROR2, -1);
	fatal_error_report(USER_DEFINED_ERROR2, "SOCKET TIMEOUT");
    }

    if (m_debug) {
	unsigned char* rdata_ptr = &m_rdata[0];
	for (int i = 0; i < m_dataByteSize/4; i++) {
	    unsigned int rdat = *(unsigned int*)rdata_ptr;
	    //std::cerr << ntohl(rdat) << std::endl;
	    std::cerr << rdat << std::endl;
	    rdata_ptr += 4;
	}
    }
    return 0;
}

/// set data to OutPort buffer
int EchoReader::set_data_to_OutPortBuf( unsigned int seq_num )
{
    unsigned char header[8];
    unsigned char footer[8];
    
    set_header(&header[0], m_dataByteSize);
    set_footer(&footer[0], seq_num);

    ///set OutPort buffer length
    m_out_data.data.length(m_dataByteSize + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE); 
    memcpy( &(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
    memcpy( &(m_out_data.data[HEADER_BYTE_SIZE]), &m_rdata[0], m_dataByteSize);
    memcpy( &(m_out_data.data[HEADER_BYTE_SIZE + m_dataByteSize]), &footer[0], FOOTER_BYTE_SIZE);
    check_footer(&m_out_data.data[HEADER_BYTE_SIZE + m_dataByteSize], seq_num);

    return 0;
}

int EchoReader::daq_run() 
{
    if (m_debug) {
	std::cerr << "*** EchoReader::run" << std::endl;
	std::cerr << "loop: " << m_loop << std::endl;
    }

    send_data_to_echoServer();
    recv_data_from_echoServer();

    set_data_to_OutPortBuf(m_loop);
    ///m_out_status = m_OutPort.write(m_out_data); /// send data from OutPort
    bool ret_out = false;

    ret_out = m_OutPort.write(); /// send data from OutPort
    if(ret_out != true) {
	m_out_status  = check_outPort_status(m_OutPort);
	if (m_out_status == BUF_FATAL) {
	    std::cerr << "#";
	}
    } else {
	m_out_status = BUF_SUCCESS;
	m_loop++;
	//m_total_event += (m_dataByteSize/m_eventByteSize);
	m_total_size += m_dataByteSize;
    }

    if (check_trans_lock() ) {  /// got stop command 
        set_trans_unlock();
        return 0;
    }

    //usleep(100000); /// sleep for 100ms to adjust histogram update time
    usleep(10000); /// sleep for 10ms to adjust histogram update time

    return 0;
}

extern "C"
{
     void EchoReaderInit(RTC::Manager* manager)
    {
	RTC::Properties profile(echoReader_spec);
	manager->registerFactory(profile,
				 RTC::Create<EchoReader>,
				 RTC::Delete<EchoReader>);
    }
};
