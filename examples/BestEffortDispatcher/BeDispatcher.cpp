// -*- C++ -*-
/*!
 * @file  Dispatcher.cpp
 * @brief Event data dispatching component
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

#include "BeDispatcher.h"
#include "stringUtils.h"

using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::HEADER_DATA_MISMATCH;
using DAQMW::FatalType::FOOTER_DATA_MISMATCH;

// Module specification
static const char* bedispatcher_spec[] =
{
    "implementation_id", "BeDispatcher",
    "type_name",         "BeDispatcher",
    "description",       "Event data dispatching component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

BeDispatcher::BeDispatcher(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort(  "dispatcher_in",   m_in_data),
      m_OutPort1("dispatcher_out", m_out_data1),
      m_OutPort2("dispatcher_out_be", m_out_data2),
      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),
      m_out_be_status(BUF_SUCCESS),
      m_sampling_rate(100),
      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("dispatcher_in",     m_InPort);
    registerOutPort("dispatcher_out",    m_OutPort1);
    registerOutPort("dispatcher_out_be", m_OutPort2);

    init_command_port();
    init_state_table( );
    set_comp_name("BEDISPATCHER");
}

BeDispatcher::~BeDispatcher()
{
}


RTC::ReturnCode_t BeDispatcher::onInitialize()
{
    if (m_debug) {
	std::cerr << "BeDispatcher::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t BeDispatcher::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int BeDispatcher::daq_dummy() 
{
    return 0;
}

int BeDispatcher::daq_configure() 
{
    std::cerr << "*** BeDispatcher::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int BeDispatcher::parse_params(::NVList* nvlist)
{
    int len = (*nvlist).length();
    if (m_debug) {
        std::cerr << "list length:" << (*nvlist).length() << std::endl;
    }
    std::string sampRateAndPort;

    for (int i = 0; i < len; i+= 2) {
        std::string sname  = (std::string)(*nvlist)[i].value;
        std::string svalue = (std::string)(*nvlist)[i+1].value;
        if (m_debug) {
            std::cerr << "sname: "   << sname << "  ";
            std::cerr << "value: " << svalue << std::endl;
        }
	if ( sname == "eventByteSize" ) {
	    m_eventByteSize = atoi( svalue.c_str());
	}
        if ( sname == "samplingRate" ) {
            if (m_debug) {
                std::cerr << "Sampling rate of Monitor:" << svalue
			  << std::endl;
            }
	    sampRateAndPort = svalue;
        }
    }

    if (sampRateAndPort != "") {
	std::cerr << "sampRateAndPort" << sampRateAndPort << std::endl;
	std::vector< std::string > ratePortList;
	split(sampRateAndPort, '/', ratePortList);
	m_sampling_rate      = atoi(ratePortList[1].c_str());
	m_monitor_port_name  = ratePortList[0].c_str();
    }
    else {
	m_sampling_rate = 1; /// No sampling. All data are sent to Logger and Monitor.
    }

    std::cerr << "m_eventByteSize:" << m_eventByteSize      << std::endl;
    std::cerr << "m_sampling_rate:" << m_sampling_rate      << std::endl;
    std::cerr << "m_monitor_port: " << m_monitor_port_name  << std::endl;

    return 0;
}


int BeDispatcher::daq_unconfigure() 
{
    std::cerr << "*** BeDispatcher::unconfigure" << std::endl;

    return 0;
}

int BeDispatcher::daq_start() 
{
    std::cerr << "*** BeDispatcher::start" << std::endl;

    m_in_status   = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;
    m_out_be_status = BUF_SUCCESS;

    return 0;
}

int BeDispatcher::daq_stop() 
{
    std::cerr << "*** BeDispatcher::stop" << std::endl;
 
    reset_InPort();
    return 0;
}

int BeDispatcher::daq_pause()
{
    std::cerr << "*** BeDispatcher::pause" << std::endl;
    return 0;
}

int BeDispatcher::daq_resume()
{
    std::cerr << "*** BeDispatcher::resume" << std::endl;
    return 0;
}


int BeDispatcher::set_sampling_rate(int rate)
{
    m_sampling_rate = rate;

    return 0;
}

int BeDispatcher::reset_InPort()
{
    TimedOctetSeq dummy_data;

    ///int ret = BUF_SUCCESS;
    bool ret = true;
    ///while( ret == BUF_SUCCESS) {
    while( ret == true) {
	///ret = m_InPort.read(dummy_data);
	ret = m_InPort.read();
    }
    std::cerr << "*** BeDispatcher::InPort flushed\n";
    return 0;
}

int BeDispatcher::daq_run() 
{
    if (m_debug) {
	std::cerr << "*** BeDispatcher::run" << std::endl;
	std::cerr << "loop: " << m_loop << std::endl;
    }

    bool ret_in = false;
    int event_byte_size = 0;

    if(m_out_status != BUF_TIMEOUT) {
        ///ret_in = m_InPort.read(m_in_data);
        ret_in = m_InPort.read();
	if(ret_in == false) {
	    m_in_status = BUF_TIMEOUT;
	    //std::cerr << "InPort.read(): BUF_TIMEOUT\n";
	    if ( check_trans_lock() ) {
		set_trans_unlock();		
	    }
	    return 0;
	}
	else {
	    //std::cerr << "InPort.read(): BUF_SUCCESS\n";
	    m_in_status = BUF_SUCCESS;
	}
    }

    if (check_trans_lock() ) {
	std::cerr << "##### check_trans_lock(): TRUE\n";
    }

//   if (m_in_status == BUF_TIMEOUT && check_trans_lock() ) {
//       set_trans_unlock();
//       return 0;
//   }
   if (check_trans_lock() ) {
       set_trans_unlock();
       return 0;
   }
   /*
    *  No data exist in upstream, return and retry.
    */
   else if (m_in_status == BUF_TIMEOUT) {
       if (check_trans_lock() ) {
	   set_trans_unlock();
       }
       return 0;
   }
   /*
    *  Fatal error occcured in Inport.read(), set error status and retrun.
    */
   else if (m_in_status == BUF_FATAL) {
       //fatal_error_report(DAQMW::FatalType::INPORT_ERROR, -1);
       fatal_error_report(INPORT_ERROR);
       return 0;
   }
   else if (m_in_status == BUF_SUCCESS) {

       int block_byte_size = m_in_data.data.length();
       if(m_debug) {
	   std::cerr << "*** read data size: " << block_byte_size << std::endl;
       }
       m_out_data1.data.length(block_byte_size);
       m_out_data2.data.length(block_byte_size);

       unsigned char header[HEADER_BYTE_SIZE];
       unsigned char footer[FOOTER_BYTE_SIZE];

       event_byte_size = block_byte_size - HEADER_BYTE_SIZE - FOOTER_BYTE_SIZE;
       if(m_debug) {
	   std::cerr << "block_byte_size w/  header and footer:" << block_byte_size
		     << std::endl;
	   std::cerr << "event_byte_size w/o header and footer:" << event_byte_size
		     << std::endl;
       }
       for (int i = 0; i < (int)HEADER_BYTE_SIZE; i++) {
	   header[i] = m_in_data.data[i];
       }

       int footer_index = block_byte_size - FOOTER_BYTE_SIZE;
       for (int i = 0; i < (int)FOOTER_BYTE_SIZE; i++) {
	   footer[i] = m_in_data.data[footer_index + i];
       }

       if( !check_header(&header[0], event_byte_size) ) {
	   std::cerr << "### ERROR: header invalid\n";
	   //fatal_error_report(DAQMW::FatalType::HEADER_DATA_MISMATCH, -1);
	   fatal_error_report(HEADER_DATA_MISMATCH);
	   //return 0;
       }

       if( !check_footer(&footer[0], m_loop) ) {
	   std::cerr << "### ERROR: footer invalid\n";
	   
	   //fatal_error_report(DAQMW::FatalType::FOOTER_DATA_MISMATCH, -1);
	   fatal_error_report(FOOTER_DATA_MISMATCH);
	   //return 0;
       }

       //m_total_event += eventByteSizeToNum( event_byte_size );


       if(m_debug) {
	   std::cerr << "*** BeDispatcher: data len ="
		     << event_byte_size << std::endl;
       }
       for(CORBA::ULong i=0; i< (CORBA::ULong)block_byte_size; i++) {
	   m_out_data1.data[i] = m_in_data.data[i];
	   m_out_data2.data[i] = m_in_data.data[i];
       }
   }

   if( m_in_status != BUF_TIMEOUT ) {
       bool ret_out = false;
       
       ret_out = m_OutPort1.write();
       //std::cerr << "OutPort1.write()\n";
       if(ret_out != true) {
	   m_out_status  = check_outPort_status(m_OutPort1);
       } else {
	   m_out_status = BUF_SUCCESS;
	   //std::cerr << "*** OutPort1.write(): succeed\n";
       }
   }

   if( m_in_status != BUF_TIMEOUT ) {
       bool ret_out_be = false;
       ret_out_be = m_OutPort2.write();
       //std::cerr << "OutPort2.write()\n";
       if(ret_out_be != true) {
		    m_out_be_status  = check_outPort_status(m_OutPort2);
       } else {
	   m_out_be_status = BUF_SUCCESS;
       }
   }

   if( (m_in_status  != BUF_TIMEOUT) && (m_out_status != BUF_TIMEOUT)) {
       m_total_size += event_byte_size;
       m_loop++;

       if(m_debug) {
	   if(m_loop%100 == 0) {
	       std::cerr << "BeDispatcher: loop = " << m_loop << std::endl;
	       std::cerr << "\033[A\r";
	   }
       }
   }

   return 0;
}

extern "C"
{
 
    void BeDispatcherInit(RTC::Manager* manager)
    {
	RTC::Properties profile(bedispatcher_spec);
	manager->registerFactory(profile,
				 RTC::Create<BeDispatcher>,
				 RTC::Delete<BeDispatcher>);
    }
};


