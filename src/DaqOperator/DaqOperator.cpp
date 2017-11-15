// -*- C++ -*-
/*!
 * @file DaqOperator.cpp
 * @brief Run controller and user interface component.
 * @date 1-January-2008
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2008-2011
 *     Kazuo Nakayoshi
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

#include "DaqOperator.h"

static const char* daqserviceconsumer_spec[] =
{
    "implementation_id", "DaqOperator",
    "type_name",         "DaqOperator",
    "description",       "Controller of DAQ components",
    "version",           "0.1",
    "vendor",            "KEK",
    "category",          "Generic",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

DAQMW::ParameterServer* g_server = NULL;

#include "callback.h"

// error message for dom
static const char* FORMAT_IO_ERR_E         = "[%s] No such file or directory.";
static const char* FORMAT_IO_ERR_J         = "[%s] No such file or directory.";
static const char* FORMAT_REQ_INV_IN_STS_E = "Invalid request in status %s.";
static const char* FORMAT_REQ_INV_IN_STS_J = "Invalid request in status %s.";

DaqOperator* DaqOperator::_instance = 0;

DaqOperator* DaqOperator::Instance()
{
    return _instance;
}

DaqOperator::DaqOperator(RTC::Manager* manager)
  : RTC::DataFlowComponentBase(manager),
    m_comp_num(0),
    m_service_num(0),
    m_state(LOADED),
    m_runNumber(0),
    m_start_date(" "),
    m_stop_date(" "),
    m_param_port(PARAM_PORT),
    m_com_completed(true),
    m_isConsoleMode(true),
    m_msg(" "),
    m_err_msg(" "),
    m_debug(false)
{
    if (m_debug) {
        std::cerr << "Create DaqOperator\n";
    }

    try {
        XMLPlatformUtils::Initialize();  //Initialize Xerces
    }
    catch (XMLException& e)
    {
        char* message = XMLString::transcode( e.getMessage() );
        std::cerr << "### ERROR: XML toolkit initialization error: "
                  << message << std::endl;
        XMLString::release( &message );
        // throw exception here to return ERROR_XERCES_INIT
    }

    ParamList paramList;
    ConfFileParser MyParser;

    m_conf_file = getConfFilePath();
    m_comp_num = MyParser.readConfFile(m_conf_file.c_str(), false);
    if (m_debug) {
        std::cerr << "Conf file:" << m_conf_file << std::endl;
        std::cerr << "comp num = " << m_comp_num << std::endl;
    }

    /// create CorbaConsumer for the number of components
    for (int i = 0; i < m_comp_num; i++) {
       RTC::CorbaConsumer<DAQService> daqservice;
       m_daqservices.push_back(daqservice);
    }
    if (m_debug) {
        std::cerr << "*** m_daqservices.size():" << m_daqservices.size() << std::endl;
    }

    /// create CorbaPort for the number of components
    for (int i = 0; i < m_comp_num; i++) {
        std::stringstream strstream;
        strstream << m_service_num++;
        std::string service_name = "service" + strstream.str();
        if (m_debug) {
            std::cerr << "service name: " << service_name << std::endl;
        }
        m_DaqServicePorts.push_back(new RTC::CorbaPort(service_name.c_str() ));
    }
    /// register CorbaPort
    for (int i = 0; i< m_comp_num; i++) {
        m_DaqServicePorts[i]->
            registerConsumer("daq_svc", "DAQService", m_daqservices[i] );
        registerPort( *m_DaqServicePorts[i] );
        if (m_debug) {
            std::cerr << "m_daqservices.size() = "
                      << m_daqservices.size() << std::endl;
            std::cerr << "m_DaqServicePorts.size() = "
                      << m_DaqServicePorts.size() << std::endl;
        }
    }

   FD_ZERO(&m_allset);
   FD_ZERO(&m_rset);

   m_tout.tv_sec =  3;
   m_tout.tv_usec = 0;
}

DaqOperator::~DaqOperator()
{
    XMLPlatformUtils::Terminate();
}

RTC::ReturnCode_t DaqOperator::onInitialize()
{
    if (m_debug) {
        std::cerr << "**** DaqOperator::onInitialize()\n";
    }
    _instance = this;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t DaqOperator::onStartup(RTC::UniqueId ec_id)
{
    if (m_debug)
        std::cerr << "\n**** DaqOperator::onStartup()\n";

    return RTC::RTC_OK;
}


RTC::ReturnCode_t DaqOperator::onActivated(RTC::UniqueId ec_id)
{
    if(m_debug)
        std::cerr << "\n**** DaqOperator::onActivated()\n";

    return RTC::RTC_OK;
}


RTC::ReturnCode_t DaqOperator::onExecute(RTC::UniqueId ec_id)
{
    RTC::ReturnCode_t ret = RTC::RTC_OK;

    if (m_isConsoleMode == true)
        ret = run_console_mode();
    else
        ret = run_http_mode();

    return ret;
}

RTC::ReturnCode_t DaqOperator::run_http_mode()
{
    if (g_server == NULL) {
        std::cerr << "m_param_port:" << m_param_port << std::endl;
        g_server = new DAQMW::ParameterServer(m_param_port);
        std::cerr << "ParameterServer starts..." << std::endl;

        g_server->bind("put:Params", &m_body, cb_command_configure);
        g_server->bind("put:ResetParams", &m_body, cb_command_unconfigure);
        //g_server->bind("put:Reset", &m_body, cb_command_unconfigure);
        g_server->bind("put:Begin", &m_body, cb_command_start);
        g_server->bind("put:End", &m_body, cb_command_stop);
        //g_server->bind("put:Abort", &m_body, cb_command_abort);
        g_server->bind("put:ConfirmEnd", &m_body, cb_command_confirmend);
        //g_server->bind("get:Params", &m_body, cb_command_putparams);
        g_server->bind("get:Status", &m_body, cb_command_putstatus);
        g_server->bind("get:Log", &m_body, cb_command_log);
        g_server->bind("put:Pause", &m_body, cb_command_pause);
        g_server->bind("put:Restart", &m_body, cb_command_resume);
        g_server->bind("put:StopParamsSet", &m_body, cb_command_stopparamsset);
        g_server->bind("put:Save", &m_body, cb_command_save);
        g_server->bind("put:ConfirmConnection", &m_body,
                       cb_command_confirmconnection);
        g_server->bind("put:dummy", &m_body, cb_command_dummy);
        if (m_debug) {
            std::cerr << "*** bind callback functions done\n"
                 << "*** Ready to accept a command\n";
        }
    }
    g_server->Run();
    run_data();
    return RTC::RTC_OK;
}

std::string DaqOperator::check_state(DAQLifeCycleState compState)
{
    std::string comp_state = "";
    switch (compState) {
    case LOADED:
        comp_state = "LOADED";
        break;
    case CONFIGURED:
        comp_state = "CONFIGURED";
        break;
    case RUNNING:
        comp_state = "RUNNING";
        break;
    case PAUSED:
        comp_state = "PAUSED";
        break;
    default:
        comp_state = "-RUNNING-";
        break;
    }
    return comp_state;
}

std::string DaqOperator::check_compStatus(CompStatus compStatus)
{
    std::string comp_status = "";
    switch (compStatus) {
    case COMP_WORKING:
        comp_status = "WORKING";
        break;
    case COMP_FINISHED:
        comp_status = "FINISHED";
        break;
    case COMP_WARNING:
        comp_status = "WORNING";
        break;
    case COMP_FATAL:
        comp_status = "ERROR";//"ERROR";
        break;
    case COMP_RESTART:
        comp_status = "input 2:stop or 6:restart";
        break;
    }
    return comp_status;
}

void DaqOperator::run_data()
{
    std::cerr << "\033[;H\033[2J";

    try {
        for (int i = 0; i< m_comp_num; i++) {
            Status_var status;
            status = m_daqservices[i]->getStatus();

            if (status->comp_status == COMP_FATAL) {
                RTC::ConnectorProfileList_var myprof =
                    m_DaqServicePorts[i]->get_connector_profiles();
                std::cerr << myprof[0].name << " "
                            << "### on ERROR ###  " << std::endl;

                FatalErrorStatus_var errStatus;
                errStatus = m_daqservices[i]->getFatalStatus();
                std::cerr << "\033[1;0H";
                std::cerr << "errStatus.fatalTypes:"
                     << errStatus->fatalTypes   << std::endl;
                std::cerr << "errStatus.errorCode:"
                     << errStatus->errorCode    << std::endl;
                std::cerr << "errStatus.description:"
                     << errStatus->description  << std::endl;
                m_err_msg = errStatus->description;
            } // if fatal
        }
    } catch (...) {
        std::cerr << "DaqOperator::run_data() Exception was caught" << std::endl;
    }

    std::cerr << "\033[0;0H";
    std::cerr << "RUN#" << m_runNumber
         << " start at: "  << m_start_date
         << " stop at: "   << m_stop_date   << std::endl;
}

RTC::ReturnCode_t DaqOperator::run_console_mode()
{ 
    ///char* state[] = {"LOADED", "CONFIGURED", "RUNNING", "PAUSED"};
    std::string srunNo = "0";

    int command;

    /* Console error display */
    std::string d_compname[m_comp_num];
    FatalErrorStatus_var d_message[m_comp_num];
    Status_var chkStatus;

    m_tout.tv_sec =  2;
    m_tout.tv_usec = 0;

    FD_ZERO(&m_rset);
    FD_SET(0, &m_rset);

    std::cerr << "\033[;0H" << " Command:\t" << std::endl;
    std::cerr << " " 
              << CMD_CONFIGURE   << ":configure\t"
              << CMD_START       << ":start   "
              << CMD_STOP        << ":stop" << std::endl << " "
              << CMD_UNCONFIGURE << ":unconfigure\t"
              << CMD_PAUSE       << ":pause   "
              << CMD_RESUME      << ":resume   "
              << CMD_RESTART     << ":restart"
              << std::endl;

    std::cerr << std::endl << " RUN NO: " << m_runNumber;
    std::cerr << std::endl << " start at: "  << m_start_date
              << "\tstop at: " << m_stop_date << std::endl;
    std::cerr << "\033[;11H";

    select(1, &m_rset, NULL, NULL, &m_tout);
    if (m_com_completed == false) {
        return RTC::RTC_OK;
    }
    
    // command check
    if (FD_ISSET(0, &m_rset)) {
        char comm[2];
        if (read(0, comm, sizeof(comm)) == -1) { //read(0:stdin))
            return RTC::RTC_OK;
        }

        command = (int)(comm[0] - '0');

        switch (m_state) {	// m_state init (LOADED)
        case PAUSED:
            switch ((DAQCommand)command) {
            case CMD_RESUME:
                resume_procedure();///
                m_state = RUNNING;
                break;
            default:
                std::cerr << "\033[;13H" 
                          << "   Bad Command:" << command << std::endl;
                break;
            }
            break;
        case LOADED:
            switch ((DAQCommand)command) {
            case CMD_CONFIGURE:
                configure_procedure();
                m_state = CONFIGURED;
                break;
            default:
                std::cerr << "\033[;13H"
                          << " Bad Command:" << std::endl;
                break;
            }
            break;
        case CONFIGURED:
            switch ((DAQCommand)command) {
            case CMD_START:
                std::cerr << "\033[5;20H"; // default=3;20H
                std::cerr << "input RUN NO(same run no is prohibited):   ";
                std::cerr << "\033[5;62H";
                std::cin >> srunNo;
                m_runNumber = atoi(srunNo.c_str());
                start_procedure();
                m_state = RUNNING;
                break;
            case CMD_UNCONFIGURE:
                unconfigure_procedure();
                m_state = LOADED;
                break;
            default:
                std::cerr << "\033[;13H" 
                          << "   Bad Command:" << command << std::endl;
                break;
            }
            break;
        case RUNNING:
            switch ((DAQCommand)command) {
            case CMD_STOP:
                stop_procedure();
                m_state = CONFIGURED;
                break;
            case CMD_PAUSE:///
                pause_procedure();///
                m_state = PAUSED;
                break;
            default:
                std::cerr << "\033[;13H"
                          << "   Bad Command: " << command << std::endl;
                break;
            }
            break;
        case ERRORED:
            switch ((DAQCommand)command) {
            case CMD_STOP:
                stop_procedure();
                m_state = CONFIGURED;///
                break;
            case CMD_RESTART:
                fix0_comp_stop_procedure();
                sleep(1);
                fix1_configure_procedure();
                sleep(2);
                std::cerr << "\033[5;20H"; // default:3;20H
                std::cerr << "input RUN NO(same run no is prohibited):   ";
                std::cerr << "\033[5;62H";
                std::cin >> srunNo;
                m_runNumber = atoi(srunNo.c_str());
                fix2_restart_procedure();
                // std::cerr   << "\033[0;13H" << "\033[34m"
                //             << "Send reboot command"
                //             << "\033[39m" << std::endl;

                /* comp_status check */
                for (int i = (m_comp_num - 1); i >= 0; i--) {
                    chkStatus = m_daqservices[i]->getStatus();
                    if(chkStatus->state == CONFIGURED) {
                        break;
                    }
                    else {
                        m_state = RUNNING;
                    }
                }
                break;
            default:
                std::cerr << "\033[;13H" 
                          << " 2:stop or 6:reboot" << std::endl;
                break;
            }
            break;
        }// switch (m_state)
    }
    else {
        std::cerr << " " << std::endl;
        std::cerr << "\033[;H\033[2J";
        std::cerr << "\033[7;0H"; //default = 5
        std::cerr << std::setw(16) << std::right << "GROUP:COMP_NAME"
                  << std::setw(22) << std::right << "EVENT_SIZE"
                  << std::setw(12) << std::right << "STATE"
                  << std::setw(14) << std::right << "COMP_STATUS"
                  << std::endl;
        ///std::cerr << "RUN NO: " << m_runNumber << std::endl;

        std::string compname;
        Status_var status;
        FatalErrorStatus_var errStatus;
        for (int i = (m_comp_num - 1); i >= 0; i--) {
            try {
                RTC::ConnectorProfileList_var myprof
                    = m_DaqServicePorts[i]->get_connector_profiles();
                
                compname = myprof[0].name; //compname = "group*:*"
                //std::cerr << "COMPNAME: " << compname << std::endl;
                status = m_daqservices[i]->getStatus();
                errStatus = m_daqservices[i]->getFatalStatus();

                FatalErrorStatus_var errStatus;
                errStatus = m_daqservices[i]->getFatalStatus();

                std::cerr << " " << std::setw(22) << std::left
                          << myprof[0].name //group:comp_name
                          << '\t'
                          << std::setw(14) << std::right
                          << status->event_size // data size(byte)
                          << std::setw(12) << std::right
                          << check_state(status->state);

                if (status->comp_status == COMP_FATAL) {
                    std::cerr << "\033[31m" << std::setw(14) << std::right
                              << check_compStatus(status->comp_status)
                              << "\033[39m" << std::endl;

                    /** Use error console display **/
                    d_compname[i] = compname;
                    d_message[i] = errStatus;
                    m_state = ERRORED;
                }///if Fatal
                else if (status->comp_status == COMP_RESTART) {
                    std::cerr << "\033[34m" << std::setw(14) << std::right
                              << check_compStatus(status->comp_status)
                              << "\033[39m" << std::endl;

                    /** Use error console display **/
                    d_compname[i] = compname;
                    d_message[i] = errStatus;
                }///if Restart Request
                else {
                    std::cerr << "\033[32m" << std::setw(14) << std::right
                              << check_compStatus(status->comp_status)
                              << "\033[39m" << std::endl;
                }
            } catch(...) {
                std::cerr << " ### ERROR: " << compname
                          << " : cannot connect" << std::endl;
                usleep(1000);
            }
        }//for
        std::cerr << std::endl;

        /* Display Error Console */
        if (m_state == ERRORED) {
            int cnt = 0;
            for (int i = (m_comp_num - 1); i >= 0; i--) {
                if (d_compname[i].length() != 0) {
                    std::cerr << " [ERROR" << ++cnt  << "] ";
                    std::cerr << d_compname[i] << '\t' << "<= " << "\033[31m"
                              << d_message[i]->description << "\033[39m"
                              << std::endl;
                }
            }///for
        }///if
    }///if
    return RTC::RTC_OK;
}

bool DaqOperator::parse_body(const char* buf, const std::string tagname)
{
    XercesDOMParser* parser = new XercesDOMParser;

    MemBufInputSource* memBufIS
        = new MemBufInputSource( (const XMLByte*)buf, strlen(buf), "test", false);

    parser->parse(*memBufIS);
    ///XMLCh* name = XMLString::transcode("params");
    XMLCh* name = XMLString::transcode(tagname.c_str());

    DOMDocument* doc = parser->getDocument();
    DOMElement* root = doc->getDocumentElement();
    DOMNodeList* list = root->getElementsByTagName(name);

    DOMElement* ele = (DOMElement*)list->item(0);
    DOMNode* node = ele->getFirstChild();
    char *tag = XMLString::transcode(node->getTextContent());

    if (m_debug) {
        std::cerr << "tagname:" << tagname << std::endl;
    }

    if (strlen(tag) != 0 && tagname == "params") {
        m_config_file_tmp = tag;
        std::cerr << "*** m_config_file:" << m_config_file_tmp << std::endl;
    }
    if (strlen(tag) != 0 && tagname == "runNo") {
        int tag_len = strlen(tag);

        if (tag_len > 6 || tag_len < 1) {
            std::cerr << "DaqOperator: Invalid Run No.\n";
            return false;
        }
        m_runNumber = atoi(tag);

        if (m_debug) {
            std::cerr << "strlen(tag):" << tag_len << std::endl;
            std::cerr << "*** m_runNumber:" << m_runNumber << std::endl;
        }
    }

    XMLString::release(&name);
    XMLString::release(&tag);

    delete(memBufIS);
    parser->resetDocumentPool();
    delete(parser);

    return true;
}

int DaqOperator::set_runno(RTC::CorbaConsumer<DAQService> daqservice, unsigned runno)
{
    try {
        daqservice->setRunNo(runno);

    } catch(...) {
        std::cerr << "*** setRunNumber: failed" << std::endl;
    }

    return 0;
}

int DaqOperator::set_command(RTC::CorbaConsumer<DAQService> daqservice,
                             DAQCommand daqcom)
{
    int status = 0;

    try {
        status = daqservice->setCommand(daqcom);
    }
    catch(...) {
        std::cerr << "### ERROR: set command: exception occured\n ";
    }

    return 0;
}

int DaqOperator::check_done(RTC::CorbaConsumer<DAQService> daqservice)
{
    int status = 0;

    try {

        while (status == 0) {
            status = daqservice->checkDone();
            if (status == 0) {
                usleep(0);
            }
        }
    } catch(...) {
        std::cerr << "### checkDone: failed" << std::endl;
    }
    return 0;
}

int DaqOperator::set_service_list()
{

    if (m_debug) {
        std::cerr << "==========================================\n";
        std::cerr << "\n\n---- service num = " << m_service_num << std::endl;
        std::cerr << "==========================================\n";
    }

    m_daqServiceList.clear();

    for (int i = 0; i < m_service_num; i++) {
        RTC::ConnectorProfileList_var myprof;
        myprof = m_DaqServicePorts[i]->get_connector_profiles();
        if (m_debug) {
            std::cerr << " ====> index     :" << i << std::endl;
            std::cerr << " ====> prof name:" << myprof[0].name << std::endl;
            std::string id = (std::string)myprof[0].name;
            std::cerr << "====> ID: " << id << std::endl;
        }
        struct serviceInfo serviceInfo;
        serviceInfo.comp_id    = myprof[0].name;
        serviceInfo.daqService = m_daqservices[i];
        m_daqServiceList.push_back(serviceInfo);
    }
    return 0;
}

int DaqOperator::fix0_comp_stop_procedure()
{
    m_com_completed = false;

    try
    {
        Status_var status;
        for (int i = (m_comp_num - 1); i >= 0; i--) {
            status = m_daqservices[i]->getStatus();
            if (status->comp_status == COMP_FATAL) {
                set_command(m_daqservices[i], CMD_STOP);
                check_done(m_daqservices[i]);
            }
        }
    } catch (...) {
        std::cerr << "### ERROR: DaqOperator: Failed to stop Component.\n";
        return 1;
    }
    
    time_t now = time(0);
    m_stop_date = asctime(localtime(&now));
    m_stop_date[m_stop_date.length()-1] = ' ';
    m_stop_date.erase(0, 4);
    
    m_com_completed = true;  
    return 0;
}

int DaqOperator::fix1_configure_procedure()
{
    Status_var status;
    m_com_completed = false;
    try {
        for (int i = 0; i < m_comp_num; i++) {
            status = m_daqservices[i]->getStatus();
            if (status->state == CONFIGURED) {
                set_command(m_daqservices[i], CMD_UNCONFIGURE);
                check_done(m_daqservices[i]);
            }
        }
    } catch(...) {
        std::cerr << "### ERROR: DaqOperator: Failed to unconfigure Component.\n";
        return 1;
    }

    ParamList paramList;
    ::NVList systemParamList;
    ::NVList groupParamList;
    m_start_date = "";
    m_stop_date  = "";

    try {
        for (int i = 0; i < (int)m_daqservices.size(); i++) {
            RTC::ConnectorProfileList_var myprof
                = m_DaqServicePorts[i]->get_connector_profiles();

            char * id = CORBA::string_dup(myprof[0].name);

            for (int j = 0; j < (int)paramList.size(); j++) {
                if (paramList[j].getId() == id) {
					int len = paramList[j].getList().length();	
					::NVList mylist(len);
					mylist = paramList[j].getList();
					m_daqservices[i]->setCompParams( paramList[j].getList() );
                }
            }
            CORBA::string_free(id);
        }
                
        for (int i = 0; i < m_comp_num; i++) {
			status = m_daqservices[i]->getStatus();
			if (status->state == LOADED) {
				set_command(m_daqservices[i], CMD_CONFIGURE);
				check_done(m_daqservices[i]);
			}
        }
    } catch (...) {
        std::cerr << "### ERROR: DaqOperator: Failed to configure Components.\n";
        return 1;
    }
    
    m_com_completed = true;
    return 0;
}

int DaqOperator::fix2_restart_procedure()
{
    m_com_completed = false;

    time_t now = time(0);
    m_start_date = asctime(localtime(&now));
    m_start_date[m_start_date.length()-1] = ' ';
    m_start_date.erase(0, 4);
    m_stop_date = "";

    Status_var status;
    try {
		for (int i = 0; i < m_comp_num; i++) {
            status = m_daqservices[i]->getStatus();
            if (status->state == CONFIGURED) {
				set_runno(m_daqservices[i], m_runNumber);
				check_done(m_daqservices[i]);
			}
        }

      for (int i = 0; i < m_comp_num; i++) {
        status = m_daqservices[i]->getStatus();
	  		if (status->state == CONFIGURED) {
		  		set_command(m_daqservices[i], CMD_START);
			  	check_done(m_daqservices[i]);
            }
        }
    } catch (...) {
        std::cerr << "### ERROR: DaqOperator: Failed to start Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::configure_procedure()
{
    if (m_debug) {
        std::cout << "*** configure_procedure: enter" << std::endl;
    }
    m_com_completed = false;
    ConfFileParser MyParser;
    ParamList paramList;
    CompGroupList groupList;
    ::NVList systemParamList;
    ::NVList groupParamList;
    m_start_date = "";
    m_stop_date  = "";

    try
    {
        m_comp_num = MyParser.readConfFile(m_conf_file.c_str(), true);
        paramList  = MyParser.getParamList();
        groupList  = MyParser.getGroupList();

        if (m_debug) {
            std::cerr << "*** Comp num = " << m_comp_num << std::endl;
            std::cerr << "*** paramList.size()  = " << paramList.size() << std::endl;
            std::cerr << "*** groupList.size()  = " << groupList.size() << std::endl;
            std::cerr << "*** serviceList.size()= " << m_daqServiceList.size() << std::endl;
        }

        for (int index = 0; index < (int)paramList.size(); index++) {
            if (m_debug) {
                std::cerr << "ID:" << paramList[index].getId() << std::endl;
            }
            ::NVList mylist = paramList[index].getList();
            if (m_debug) {
                for (int i = 0; i < (int)mylist.length(); i++) {
                    std::cerr << "  name :" << mylist[i].name  << std::endl;
                    std::cerr << "  value:" << mylist[i].value << std::endl;
                }
            }
        }
        if (m_debug) {
            for (int i = 0; i< (int)m_daqServiceList.size(); i++) {
                std::cerr << "*** id:" << m_daqServiceList[i].comp_id << std::endl;
            }
        }
    } catch (...) {
        std::cerr << "### ERROR: DaqOperator: Failed to read the Configuration file\n";
        std::cerr << "### Check the Configuration file\n";
        return 1;
    }

    if (m_debug) {
        std::cerr << "m_daqServiceList.size():" << m_daqServiceList.size() << std::endl;
    }

    try {
        for (int i = 0; i < (int)m_daqservices.size(); i++) {
            RTC::ConnectorProfileList_var myprof
                = m_DaqServicePorts[i]->get_connector_profiles();

            char * id = CORBA::string_dup(myprof[0].name);

            if (m_debug) {
                std::cerr << "*** id:" << id << std::endl;
            }

            for (int j = 0; j < (int)paramList.size(); j++) {
                if (m_debug) {
                    std::cerr << "paramList[i].getId():" << paramList[j].getId() << std::endl;
                }
                if (paramList[j].getId() == id) {
                    if (m_debug) {
                        std::cerr << "paramList[i].getId():" << paramList[j].getId() << std::endl;
                        std::cerr << "m_daqServiceList  id:" << id << std::endl;
                    }
                    int len = paramList[j].getList().length();
                    if (m_debug) {
                        std::cerr << "paramList[i].getList().size()" << len << std::endl;
                    }
                    ::NVList mylist(len);
                    mylist = paramList[j].getList();

                    if (m_debug) {
                        for (int k = 0; k < len; k++) {
                            std::cerr << "mylist[" << k << "].name: " << mylist[k].name << std::endl;
                            std::cerr << "mylist[" << k << "].valu: " << mylist[k].value << std::endl;
                        }
                    }
                    m_daqservices[i]->setCompParams( paramList[j].getList() );
                }
            }
            CORBA::string_free(id);
        }

        for (int i = 0; i< m_comp_num; i++) {
            set_command(m_daqservices[i], CMD_CONFIGURE);
            check_done(m_daqservices[i]);
        }

    } catch (...) {
        std::cerr << "### ERROR: DaqOperator: Failed to configure Components.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::unconfigure_procedure()
{
    m_com_completed = false;
    try {
        for (int i = 0; i< m_comp_num; i++) {
            set_command(m_daqservices[i], CMD_UNCONFIGURE);
            check_done(m_daqservices[i]);
        }
    } catch(...) {
        std::cerr << "### ERROR: DaqOperator: Failed to unconfigure Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::start_procedure()
{
    m_com_completed = false;
    try {
        time_t now = time(0);
        m_start_date = asctime(localtime(&now));
        m_start_date[m_start_date.length()-1] = ' ';
        m_start_date.erase(0, 4);
        m_stop_date = "";

        if (m_debug) {
            std::cerr << "start_parocedure: runno: " << m_runNumber << std::endl;
        }

        for (int i = 0; i< m_comp_num; i++) {
            set_runno(m_daqservices[i], m_runNumber);
            check_done(m_daqservices[i]);
        }

        for (int i = 0; i< m_comp_num; i++) {
            set_command(m_daqservices[i], CMD_START);
            check_done(m_daqservices[i]);
        }

    } catch (...) {
        std::cerr << "### ERROR: DaqOperator: Failed to start Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::stop_procedure()
{
    m_com_completed = false;
    int comp_num = m_comp_num - 1;

    try {

        for (int i = comp_num; i >= 0; i--) {
            set_command(m_daqservices[i], CMD_STOP);
            check_done(m_daqservices[i]);
        }

        time_t now = time(0);
        m_stop_date = asctime(localtime(&now));
        m_stop_date[m_stop_date.length()-1] = ' ';
        m_stop_date.erase(0, 4);
        //m_stop_date = " Stop : " + stop_date;
    } catch (...) {
        std::cerr << "### ERROR: DaqOperator: Failed to stop Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::pause_procedure()
{
    m_com_completed = false;
    int comp_num = m_comp_num - 1;
    try {
        for (int i = comp_num; i >= 0; i--) {
            set_command(m_daqservices[i], CMD_PAUSE);
            check_done(m_daqservices[i]);
        }
    } catch(...) {
        std::cerr << "### ERROR: DaqOperator: Failed to pause Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::resume_procedure()
{
    m_com_completed = false;
    try {

        for (int i = 0; i< m_comp_num; i++) {
            set_command(m_daqservices[i], CMD_RESUME);
            check_done(m_daqservices[i]);
        }

    } catch(...) {
        std::cerr << "### ERROR: DaqOperator: Failed to resume Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::abort_procedure()
{
  std::cout << "abort_procedure: enter" << std::endl;

  return 0;
}

int DaqOperator::putstatus_procedure()
{
    return 0;
}

int DaqOperator::log_procedure()
{
    return 0;
}

#ifdef NOUSE
void DaqOperator::addCorbaPort()
{
    RTC::CorbaConsumer<DAQService> daqservice;

    m_daqservices.push_back(daqservice);

    std::stringstream strstream;
    strstream << m_service_num++;
    std::string service_name = "service" + strstream.str();
}
void DaqOperator::delCorbaPort()
{
    ;
}
#endif

void DaqOperator::set_console_flag(bool isConsole)
{
    std::cerr << "set_console_flag(): " << isConsole << std::endl;
    m_isConsoleMode = isConsole;
}

void DaqOperator::set_port_no(int port)
{
    m_param_port = port;
}

std::string DaqOperator::getConfFilePath()
{
    std::string pathFile = ".confFilePath";
    std::ifstream ifs(pathFile.c_str());
    std::string mypath = "";
    ifs >> mypath;
    return mypath;
}

std::string DaqOperator::getMsg()
{
    return m_msg;
}

std::string DaqOperator::getBody()
{
    return m_body;
}

int DaqOperator::command_configure()
{
    //std::cout << "command_configure: enter" << std::endl;

    if (m_state != LOADED) {
        createDom_ng("Params");
        std::cerr << "   Bad Command\n";
        return 1;
    }

    m_config_file = m_config_file_tmp;
    if (configure_procedure() == 1) {
        char str_e[128];
        sprintf(str_e, FORMAT_IO_ERR_E, m_config_file.c_str());

        char str_j[128];
        sprintf(str_j, FORMAT_IO_ERR_J, m_config_file.c_str());

        createDom_ng("Params", RET_CODE_IO_ERR, str_e, str_j);

        return 1;
    }

    m_state = CONFIGURED;
    createDom_ok("Params");
    return 0;
}

int DaqOperator::command_unconfigure()
{
    //std::cout << "command_unconfigure: enter" << std::endl;

     if (m_state != CONFIGURED) {
         createDom_ng("ResetParams");
         std::cerr << "   Bad Command\n";
         return 1;
    }
    unconfigure_procedure();
    m_state = LOADED;
    createDom_ok("ResetParams");
    return 0;
}

int DaqOperator::command_start()
{
	//std::cout << "command_start: enter" << std::endl;

    if (m_state != CONFIGURED) {
        createDom_ng("Begin");
        std::cerr << "   Bad Command\n";
        return 1;
    }
    start_procedure();
    m_state = RUNNING;
    createDom_ok("Begin");
    return 0;
}

int DaqOperator::command_stop()
{
    //std::cout << "command_stop: enter" << std::endl;
    if (m_state != RUNNING) {
        createDom_ng("End");
        std::cerr << "   Bad Command\n";
        return 1;
    }

    stop_procedure();
    m_state = CONFIGURED;
    createDom_ok("End");

    return 0;
}

int DaqOperator::command_abort()
{
    //std::cout << "command_abort: enter" << std::endl;

    if (m_state != RUNNING) {
        createDom_ng("Abort");
        std::cerr << "   Bad Command\n";
        return 1;
    }

    abort_procedure();
    unconfigure_procedure();
    m_state = LOADED;

    createDom_ok("Abort");

    return 0;
}

int DaqOperator::command_confirmend()
{
    //std::cout << "command_confirmend: enter" << std::endl;

    if (m_state == RUNNING) {
        createDom_ng("ConfirmEnd");
        std::cerr << "   Bad Command\n";
        return 1;
    }

    createDom_ok("ConfirmEnd");

    return 0;
}

int DaqOperator::command_putparams()
{
    //std::cout << "command_putparams: enter" << std::endl;

    if (m_state == LOADED) {
        createDom_ng("Params");
        std::cerr << "   Bad Command\n";
        return 1;
    }

    DAQMW::CreateDom createDom;
    m_msg = createDom.getParams("Params", &m_nv_list[0]);

    return 0;
}

int DaqOperator::command_putstatus()
{
    //std::cerr << "command_putstatus: enter" << std::endl;
    putstatus_procedure();

    //Status status1 = m_daqservices[0]->getStatus();
    //::NVList* list = &(status1.list);

    DAQMW::CreateDom createDom;
    //m_msg = createDom.getStatus("Status", m_state, open, list);
    m_msg = createDom.getStatus("Status", m_state);

    //std::cerr << "Status:msg:" << m_msg << std::endl;
    return 0;
}

int DaqOperator::command_log()
{
    //std::cerr << "command_log: enter" << std::endl;
    //std::cerr << "m_comp_num: " << m_comp_num << std::endl;

    log_procedure();
    DAQMW::CreateDom createDom;

    groupStatus groupStat;
    groupStatusList groupStatList;

    bool fatal_error = false;

    for (int i = 0; i < m_comp_num; i++) {

        RTC::ConnectorProfileList_var  myprof
            = m_DaqServicePorts[i]->get_connector_profiles();
        groupStat.groupId      = CORBA::string_dup(myprof[0].name);

        Status_var status = m_daqservices[i]->getStatus();

        groupStat.comp_status.comp_name  = CORBA::string_dup(status->comp_name);
        groupStat.comp_status.state  = status->state;
        groupStat.comp_status.event_size  = status->event_size;
        groupStat.comp_status.comp_status  = status->comp_status;

        if(groupStat.comp_status.comp_status == COMP_FATAL) {
            fatal_error = true;
        }

        groupStatList.push_back( groupStat );
    }

    if(fatal_error) {
        std::cerr << "### FATAL: command_log(): " << m_err_msg << std::endl;
        m_msg = createDom.getLog("Log", groupStatList, m_err_msg);
        m_err_msg = "";
    } else {
        m_msg = createDom.getLog("Log", groupStatList);
    }

    for (unsigned int i = 0; i < groupStatList.size(); i++) {
        CORBA::string_free(groupStatList[i].groupId);
        // CORBA::string_member destructor will freed
        // groupStatList[i].comp_status.comp_name);
    }
    return 0;
}

int DaqOperator::command_pause()
{
    //std::cout << "command_pause: enter" << std::endl;
    if (m_state != RUNNING) {
        createDom_ng("Pause");
        std::cerr << "   Bad Command\n";
        return 1;
    }

    pause_procedure();
    m_state = PAUSED;

    createDom_ok("Pause");

    return 0;
}

int DaqOperator::command_resume()
{
    //std::cout << "command_resume: enter" << std::endl;

    if (m_state != PAUSED) {
        createDom_ng("Restart");
        std::cerr << "   Bad Command\n";
        return 1;
    }

    resume_procedure();
    m_state = RUNNING;
    createDom_ok("Restart");

    return 0;
}

int DaqOperator::command_stopparamsset()
{
    //std::cout << "command_stopparamsset: enter" << std::endl;
    createDom_ok("StopParamsSet");

    return 0;
}

int DaqOperator::command_resetparams()
{
    //std::cout << "command_resetparams: enter" << std::endl;
    createDom_ok("ResetParams");

    return 0;
}

int DaqOperator::command_save()
{
    //std::cout << "command_save: enter" << std::endl;
    createDom_ok("Save");

    return 0;
}

int DaqOperator::command_confirmconnection()
{
    //std::cout << "command_confirmconnection: enter" << std::endl;
    createDom_ok("ConfirmConnection");

    return 0;
}

int DaqOperator::command_dummy()
{
    //std::cout << "command_dummy: enter" << std::endl;
    return 0;
}

void DaqOperator::createDom_ok(std::string name)
{
    DAQMW::CreateDom createDom;
    m_msg = createDom.getOK(name);
}

void DaqOperator::createDom_ng(std::string name)
{
    DAQMW::CreateDom createDom;
    std::string state = createDom.getState(m_state, false);
    m_msg = "";

    char str_e[128];
    sprintf(str_e, FORMAT_REQ_INV_IN_STS_E, state.c_str());
    char str_j[128];
    sprintf(str_j, FORMAT_REQ_INV_IN_STS_J, state.c_str());

    createDom_ng(name, RET_CODE_REQ_INV_IN_STS, str_e, str_j);
}

void DaqOperator::createDom_ng(std::string name, int code, char* str_e, char* str_j)
{
    DAQMW::CreateDom createDom;
    m_msg = createDom.getNG(name, code, name, str_e, str_j);
}

extern "C"
{
    void DaqOperatorInit(RTC::Manager* manager)
    {
        RTC::Properties profile(daqserviceconsumer_spec);
        manager->registerFactory(profile,
                                 RTC::Create<DaqOperator>,
                                 RTC::Delete<DaqOperator>);
    }
};
