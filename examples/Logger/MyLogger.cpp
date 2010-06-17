// -*- C++ -*-
/*!
 * @file MyLogger.cpp
 * @brief Event data logging component.
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

#include "MyLogger.h"
//#include "stringUtils.h"

using DAQMW::FatalType::BAD_DIR;
using DAQMW::FatalType::CANNOT_MAKE_DIR;
using DAQMW::FatalType::CANNOT_OPEN_FILE;
using DAQMW::FatalType::HEADER_DATA_MISMATCH;
using DAQMW::FatalType::FOOTER_DATA_MISMATCH;
using DAQMW::FatalType::CANNOT_WRITE_DATA;

//extern void toLower(std::basic_string<char>& s);

// Module specification
static const char* mylogger_spec[] =
{
    "implementation_id", "MyLogger",
    "type_name",         "MyLogger",
    "description",       "Event data Logging component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

MyLogger::MyLogger(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
    m_InPort("logger_in", m_in_data),
    m_isDataLogging(false),
    m_filesOpened(false),
    m_in_status(BUF_SUCCESS),
    //m_in_status(RTC::DataPortStatus::PORT_OK),
    m_update_rate(100),
    m_debug(false)
{
    // Registration: InPort/OutPort/Service
    registerInPort("logger_in", m_InPort);

    init_command_port();
    init_state_table( );
    set_comp_name("MyLogger");
}

MyLogger::~MyLogger()
{
}

RTC::ReturnCode_t MyLogger::onInitialize()
{
    if (m_debug) {
	std::cerr << "MyLogger::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t MyLogger::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int MyLogger::daq_dummy() 
{
    return 0;
}

int MyLogger::daq_configure() 
{
    std::cerr << "*** Loggqer::configure" << std::endl;
    int ret = 0;
    m_total_event = 0;
    m_isDataLogging = false; 
    m_moduleNameList.clear();

    ::NVList* list = m_daq_service0.getCompParams();
    parse_params(list);

    return ret;
}

int MyLogger::parse_params(::NVList* list)
{
    int ret = 0;

    bool isExistParamLogging = false;
    bool isExistParamDirName = false;

    int length = (*list).length();
    for(int i=0; i< length ; i+=2) {
	if (m_debug) {
	    std::cerr << "NVList[" << (*list)[i].name 
		      << ","<< (*list)[i].value << "]" << std::endl;
	}
	std::string sname  = (std::string)(*list)[i].value;
	std::string svalue = (std::string)(*list)[i+1].value;
	if ( sname == "eventByteSize" ) {
	    m_eventByteSize = atoi( svalue.c_str() );
	    std::cerr << "m_eventByteSize:" << m_eventByteSize << std::endl;
	}
	if ( sname == "isLogging" ) {
	    toLower(svalue); // all characters of cvale are converted to lower case.
	    isExistParamLogging = true;
	    if ( svalue == "yes" ) {
		m_isDataLogging = true;
		fileUtils = new FileUtils();
		isExistParamLogging = true;
		std::cerr << "MyLogger: Save to file: true\n";		
	    }
	    else if (svalue == "no" ) {
		m_isDataLogging = false;
		isExistParamLogging = true;
		std::cerr << "MyLogger: Save to file: false\n";
	    }
	}

        if ( sname == "monRate" ) {
            m_update_rate = atoi(svalue.c_str());
            if (m_debug) {
                std::cerr << "update rate:" << m_update_rate << std::endl;
            }
        }

    }

    if (m_isDataLogging) {
	
	for(int i=0; i< length ; i+=2) {
	    if (m_debug) {
		std::cerr << "NVList[" << (*list)[i].name 
			  << ","<< (*list)[i].value << "]" << std::endl;
	    }
	    std::string sname  = (std::string)(*list)[i].value;
	    std::string svalue = (std::string)(*list)[i+1].value;

	    if ( sname == "dirName" ) {
		isExistParamDirName = true;
		m_dirName = svalue;
		if (m_isDataLogging) {
		    std::cerr << "Dir name for data saving:" 
			      << m_dirName << std::endl;
		    ret = fileUtils->check_dir(m_dirName);
		    if (ret != 0) {
			delete fileUtils;
			fileUtils = 0;
			std::cerr << "Can not open directory:" 
				  << m_dirName << std::endl;
			//fatal_error_report(DAQMW::FatalType::BAD_DIR, -1);
			fatal_error_report(BAD_DIR);
			return 0;
		    }
		}
	    }

	    if ( sname =="runNumber" ) {
		m_runNumber = atoi(svalue.c_str());
		std::cerr << "Run Number:" 
			  << m_runNumber << std::endl;
	    }

	    if ( sname == "maxFileSizeInMegaByte" ) {
		m_maxFileSizeInMByte = strtoul(svalue.c_str(), NULL, 0);
		std::cerr << "Max File size(MByte):" 
			  << m_maxFileSizeInMByte << std::endl;
	    }
#ifdef PSD
	    if ( sname == "srcAddr" ) {
		std::stringstream ss;
		///int modNo = addrToModNo(svalue);
		int modNo = 0;
		std::cerr << "=== modNo: " << modNo << std::endl;
		ss << modNo;
		m_moduleNameList.push_back(ss.str());
	    }
#endif
	}
    }

    for(int i = 0; i < (int)m_moduleNameList.size(); i++) {
	std::cerr << "modNo=" << m_moduleNameList.at(i) << std::endl;
    }
    return ret;
}

int MyLogger::daq_unconfigure() 
{
    std::cerr << "*** MyLogger::unconfigure" << std::endl;
    if (m_isDataLogging) {
	for(int i = 0; i< (int)m_file_list.size(); i++) {
	    delete m_file_list[i];
	}
	m_file_list.clear();
      
	delete fileUtils;
	if (m_debug) {
	    std::cerr << "fileUtils deleted\n";
	}
	fileUtils = 0;
    }
    return 0;
}

int MyLogger::daq_start() 
{
    std::cerr << "*** MyLogger::start" << std::endl;

    m_in_status = BUF_SUCCESS;
    m_filesOpened = false;
    m_runNumber = m_daq_service0.getRunNo();
    std::cerr << "m_runNumber:" << m_runNumber << std::endl;

    if (m_isDataLogging) {
        int ret = 0;
	fileUtils->set_run_no(m_runNumber);
#ifdef PSD
	if ( fileUtils->make_dir(m_dirName, m_instId) != 0) {
	    std::cerr << "### ERROR: MyLogger: make dir failed\n";
	    fatal_error_report(CANNOT_MAKE_DIR);
	    return 0;
	}
	fileUtils->set_daq_id(m_daqId_str);
#endif
	std::cerr << "m_maxFileSizeInMByte:" 
		  << m_maxFileSizeInMByte << std::endl;
	fileUtils->set_max_size_in_MegaBytes(m_maxFileSizeInMByte);
	//fileUtils->set_compress_flag(m_compress); /// not implement
	ret = fileUtils->open_file(m_dirName);
	if (ret != 0) {
	    std::cerr << "### ERROR: MyLogger: open file failed\n";
	    fatal_error_report(CANNOT_OPEN_FILE);
	}
	else {
	    std::cerr << "*** MyLogger: file open succeed\n";
	    m_filesOpened = true;
	}
    }
    std::cerr << "*** MyLogger: daq_start exit\n";
    return 0;
}

int MyLogger::daq_stop() 
{
    std::cerr << "*** MyLogger::stop" << std::endl;

    if (m_isDataLogging && m_filesOpened) {
	std::cerr << "MyLogger::stop: close files \n";
	//fileUtils->close_files(false);
	fileUtils->close_file();
    }

    reset_InPort();
    return 0;
}

int MyLogger::daq_pause()
{
    std::cerr << "*** MyLogger::pause" << std::endl;
    return 0;
}

int MyLogger::daq_resume()
{
    std::cerr << "*** MyLogger::resume" << std::endl;
    return 0;
}

int MyLogger::reset_InPort()
{
    ///RTC::TimedOctetSeq dummy_data;

    bool ret = true;
    while( ret ) {
	///ret = m_InPort.read(dummy_data);
	ret = m_InPort.read();
	if(ret == true) {
	    std::cerr << "m_in_data.data.length(): " 
		      << m_in_data.data.length() << std::endl;
	}
    }
    std::cerr << "*** MyLogger::InPort flushed\n";
    return 0;
}

void MyLogger::toLower(std::basic_string<char>& s) {
  for(std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
    *p = tolower(*p);
  }
}


int MyLogger::daq_run() {
#ifdef ORG
    if( check_trans_lock() ) {
	set_trans_unlock();
	return 0;
    }
#endif

    ///m_in_status = m_InPort.read(m_in_data);
    unsigned char header[HEADER_BYTE_SIZE];
    int event_byte_size = 0;

    bool ret = m_InPort.read();

    if (ret == true) {
	int block_byte_size = m_in_data.data.length();

	event_byte_size = block_byte_size - HEADER_BYTE_SIZE - FOOTER_BYTE_SIZE;
	if (m_debug) {
	    std::cerr << "m_in_data.data.length:" 
		      << m_in_data.data.length() << std::endl;
	    std::cerr << "event_byte_size w/ header, fooger = " 
		      << event_byte_size << std::endl;
	}
	    
	if (event_byte_size == 0) {
	    return 0;
	} 

	unsigned char footer[FOOTER_BYTE_SIZE];
	for (int i = 0; i < (int)HEADER_BYTE_SIZE; i++) {
	    header[i] = m_in_data.data[i];
	    if (m_debug) {
		std::cerr << std::hex <<  (int)m_in_data.data[i] << std::endl;
	    }
	}
	int footer_index = block_byte_size - FOOTER_BYTE_SIZE;

	for (int i = 0; i < (int)FOOTER_BYTE_SIZE; i++) {
	    footer[i] = m_in_data.data[footer_index + i];
	    if (m_debug) {
		std::cerr << std::hex << (int)m_in_data.data[footer_index + i] 
			  << std::endl;
		std::cerr << std::dec;
	    }
	}

	if ( !check_header(&header[0], event_byte_size) ) {
	    std::cerr << "### ERROR: header invalid\n";
	    fatal_error_report(HEADER_DATA_MISMATCH);
	    
	    return 0;
	}
	if ( !check_footer(&footer[0], m_loop) ) {
	    std::cerr << "### ERROR: footer invalid\n";
	    fatal_error_report(FOOTER_DATA_MISMATCH);
	    
	    return 0;
	}

    }
    else {
	//std::cerr << "### InPort.read() failed\n";
	if( check_trans_lock() ) {
	    std::cerr << "**** trans unlock\n";
	    set_trans_unlock();
	}
	return 0;
    }

   if (m_isDataLogging) {
       try {
	   ///fileUtils->write(modNo, (char *)&m_in_data.data[HEADER_BYTE_SIZE], 
	   fileUtils->write((char *)&m_in_data.data[HEADER_BYTE_SIZE], event_byte_size);
       }
       catch(...) {
	   std::cerr << "### MyLogger: ERROR occured at data saving\n";
	   fatal_error_report(CANNOT_WRITE_DATA);

	   return 0;
       }
   }

   m_total_size += event_byte_size;	    
   m_loop++;

   if(m_debug) {
       if (m_loop%m_update_rate == 0) {
	   std::cerr << "MyLogger: loop = " << m_loop << std::endl;
	   std::cerr << "\033[A\r";
       }
   }

   return 0;
}

extern "C"
{
    void MyLoggerInit(RTC::Manager* manager)
    {
	RTC::Properties profile(mylogger_spec);
	manager->registerFactory(profile,
				 RTC::Create<MyLogger>,
				 RTC::Delete<MyLogger>);
    }
};


