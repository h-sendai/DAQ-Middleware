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

using namespace std;

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
        cerr << "Create DaqOperator\n";
    }

    try {
        XMLPlatformUtils::Initialize();  //Initialize Xerces
    }
    catch (XMLException& e)
    {
        char* message = XMLString::transcode( e.getMessage() );
        cerr << "### ERROR: XML toolkit initialization error: "
                  << message << endl;
        XMLString::release( &message );
        // throw exception here to return ERROR_XERCES_INIT
    }

    ParamList paramList;
    ConfFileParser MyParser;

    m_conf_file = getConfFilePath();
    m_comp_num = MyParser.readConfFile(m_conf_file.c_str(), false);
    if (m_debug) {
        cerr << "Conf file:" << m_conf_file << endl;
        cerr << "comp num = " << m_comp_num << endl;
    }

    /// create CorbaConsumer for the number of components
    for (int i = 0; i < m_comp_num; i++) {
       RTC::CorbaConsumer<DAQService> daqservice;
       m_daqservices.push_back(daqservice);
    }
    if (m_debug) {
        cerr << "*** m_daqservices.size():" << m_daqservices.size() << endl;
    }

    /// create CorbaPort for the number of components
    for (int i = 0; i < m_comp_num; i++) {
        stringstream strstream;
        strstream << m_service_num++;
        string service_name = "service" + strstream.str();
        if (m_debug) {
            cerr << "service name: " << service_name << endl;
        }
        m_DaqServicePorts.push_back(new RTC::CorbaPort(service_name.c_str() ));
    }
    /// register CorbaPort
    for (int i = 0; i< m_comp_num; i++) {
        m_DaqServicePorts[i]->
            registerConsumer("daq_svc", "DAQService", m_daqservices[i] );
        registerPort( *m_DaqServicePorts[i] );
        if (m_debug) {
            cerr << "m_daqservices.size() = "
                      << m_daqservices.size() << endl;
            cerr << "m_DaqServicePorts.size() = "
                      << m_DaqServicePorts.size() << endl;
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
        cerr << "**** DaqOperator::onInitialize()\n";
    }
    _instance = this;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t DaqOperator::onStartup(RTC::UniqueId ec_id)
{
    if (m_debug)
        cerr << "\n**** DaqOperator::onStartup()\n";

    return RTC::RTC_OK;
}


RTC::ReturnCode_t DaqOperator::onActivated(RTC::UniqueId ec_id)
{
    if(m_debug)
        cerr << "\n**** DaqOperator::onActivated()\n";

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
        cerr << "m_param_port:" << m_param_port << endl;
        g_server = new DAQMW::ParameterServer(m_param_port);
        cerr << "ParameterServer starts..." << endl;

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
            cerr << "*** bind callback functions done\n"
                 << "*** Ready to accept a command\n";
        }
    }
    g_server->Run();
    run_data();
    return RTC::RTC_OK;
}

string DaqOperator::check_state(DAQLifeCycleState compState)
{
    string comp_state = "";
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
    case ERROR:
     	comp_state = "ERROR"; // Teel Error
        break;
    /*
    case STOP:
        comp_state = "FIX_WAIT"; // Tell Stop Successful
        break;
    */
    }
    return comp_state;
}

string DaqOperator::check_compStatus(CompStatus compStatus)
{
    string comp_status = "";
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
        comp_status = "ERROR"; //FATAL_ERROR
        break;
    case COMP_FIXWAIT:
		comp_status = "GET_REQUEST"; // COMP wait fix cmd
		break;
    }
    return comp_status;
}

void DaqOperator::run_data()
{
    cerr << "\033[;H\033[2J";

    try {
        for (int i = 0; i< m_comp_num; i++) {
            Status_var status;
            status = m_daqservices[i]->getStatus();

            if (status->comp_status == COMP_FATAL) {
                RTC::ConnectorProfileList_var myprof =
                    m_DaqServicePorts[i]->get_connector_profiles();
                cerr << myprof[0].name << " "
                            << "### on ERROR ###  " << endl;

                FatalErrorStatus_var errStatus;
                errStatus = m_daqservices[i]->getFatalStatus();
                cerr << "\033[1;0H";
                cerr << "errStatus.fatalTypes:"
                     << errStatus->fatalTypes   << endl;
                cerr << "errStatus.errorCode:"
                     << errStatus->errorCode    << endl;
                cerr << "errStatus.description:"
                     << errStatus->description  << endl;
                m_err_msg = errStatus->description;
            } // if fatal
        }
    } catch (...) {
        cerr << "DaqOperator::run_data() Exception was caught" << endl;
    }

    cerr << "\033[0;0H";
    cerr << "RUN#" << m_runNumber
         << " start at: "  << m_start_date
         << " stop at: "   << m_stop_date << endl;
}

RTC::ReturnCode_t DaqOperator::run_console_mode()
{
    //new variables
    int errOccur[m_comp_num];
	string d_compname[m_comp_num];
	FatalErrorStatus_var d_err_message[m_comp_num];

    string srunNo = "0";
    int command;

    m_tout.tv_sec =  2;
    m_tout.tv_usec = 0;

    FD_ZERO(&m_rset);
    FD_SET(0, &m_rset);

    cerr    << "\033[0;0H";
    cerr    << " Command:" 	    << endl
    << " "  << CMD_CONFIGURE   	<< ":configure  "
            << CMD_START       	<< ":start  "
            << CMD_STOP        	<< ":stop  "
            << CMD_UNCONFIGURE	<< ":unconfigure  "
            << CMD_PAUSE       	<< ":pause  "
            << CMD_RESUME      	<< ":resume  "
            << endl << " "
            << CMD_FIX			    << ":fix(reboot)" // NEW STATE
            << endl;

    cerr << "\n" << " RUN NO: " 	<< m_runNumber;
    cerr << "\n" << " start at: "  	<< m_start_date
                 << "\tstop at: "   << m_stop_date << endl;
    cerr << "\033[1;11H";

    select(1, &m_rset, NULL, NULL, &m_tout);
    if (m_com_completed == false) {
        return RTC::RTC_OK;
    }

    // command check
    if (FD_ISSET(0, &m_rset)) {
        char comm[2];
        if ( read(0, comm, sizeof(comm)) == -1) { //read(0:stdin))
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
                cerr << " Bad Command:" << command << endl;
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
                cerr << " Bad Command:" << command << endl;
                break;
            }
            break;
        case CONFIGURED:
            switch ((DAQCommand)command) {
            case CMD_START:
                cerr << "\033[4;20H";
                cerr << "input RUN NO(same run no is prohibited):\t";
                cerr << "\033[4;62H";
                cin >> srunNo;
                m_runNumber = atoi(srunNo.c_str());
                start_procedure();
                m_state = RUNNING;
                break;
            case CMD_UNCONFIGURE:
                unconfigure_procedure();
                m_state = LOADED;
                break;
            case CMD_FIX:
				next_procedure(errOccur);
				m_state = RUNNING;
                break;
            default:
                cerr << " Bad Command:" << command << endl;
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
                cerr << " Bad Command:" << command << endl;
                break;
            }
            break;
        case ERROR:
			switch ((DAQCommand)command) {
			case CMD_STOP:
				stop_procedure();
                m_state = CONFIGURED;
				break;
            case CMD_FIX:
				comp_stop_procedure(errOccur);
				m_state = CONFIGURED;
				break;
			default:
				cerr << " Bad Command:" << command << endl;
				break;
			}
			break;
        
	/* case STOP:
			cerr << "\033[3;H" <<	" Stop. Fix wait." << endl;
            switch ((DAQCommand)command) {
            case CMD_FIX:
				comp_reboot_procedure();
				m_state = RUNNING;
                break;
            default:
				cerr << " Bad Command:" << command << endl;
                break;
            }
			break;
    */
        }/// switch (m_state) 
    }///if
    else {
        cerr << " " << endl;
        cerr << "\033[;H\033[2J" << "\033[6;0H" << endl;
        cerr << setw(16) << right << "GROUP:COMP_NAME"
             << setw(22) << right << "EVENT_SIZE"
             << setw(12) << right << "STATE"
             << setw(14) << right << "COMP_STATUS" << endl;

        //cerr << "RUN NO: " << m_runNumber << endl;
		
		Status_var d_status[m_comp_num];
		
        for (int i = (m_comp_num - 1); i >= 0; i--) {
            string compname;
            
            try {

                RTC::ConnectorProfileList_var myprof
                    = m_DaqServicePorts[i]->get_connector_profiles();
				
                compname = myprof[0].name; //compname = "group*:*"

				//cerr << "COMPNAME: " << compname << endl;
				
				Status_var status;
                status = m_daqservices[i]->getStatus(); // status input
				d_status[i] = status;	// Use stop_comp(d_status[]) "ERROR"
				
                cerr << " " << setw(22) << left
                     << myprof[0].name //group:comp_name
                     << '\t'
                     << setw(14) << right
                     << status->event_size // データサイズ(byte)
                     << setw(12) << right
                     << check_state(status->state) // ステート(LOADED)
                     << setw(14) << right;

                if (status->comp_status == COMP_FATAL || 
                    status->comp_status == COMP_FIXWAIT) {
                    cerr << "\033[31m"
                         << check_compStatus(status->comp_status)
                         << "\033[39m" << endl;

                    FatalErrorStatus_var errStatus;
                    errStatus = m_daqservices[i]->getFatalStatus();
                    
                    status->state = ERROR;
                    /** Use error console display **/
                    d_compname[i] = compname;
                    d_err_message[i] = errStatus;
                    errOccur[i] = 1;
                    
                } ///if = red word descript
                else {
                    cerr << check_compStatus(status->comp_status) << endl;
                    errOccur[i] = 0;
                } ///else = white word descript         
                
            } catch(...) {
                cerr << " ### ERROR: " << compname << "  : cannot connect" << endl;
                sleep(1);
            }
        }///for
        cerr << endl;

        /* Display Error Console */
        int count = 0;
        int d_compname_len;

        for (int i = (m_comp_num - 1); i >= 0; i--) {
			if ((d_compname_len = d_compname[i].length()) != 0) {
				++count;
				cerr<< " [" << "\033[31m" << "ERROR"  << count << "\033[39m" << "] "
                    << d_compname[i] << "\t\033[D<= "
                    << d_err_message[i]->description << endl
                    << "\033[;13H\033[31m" 
                    << "# ERR_MODE 2:Stop(All comp), 6:Stop(Error comp only)"
                    << "\033[39m" << endl;
                m_state = ERROR;
			}
		}///for		
    }///if..else

    return RTC::RTC_OK;
}

int DaqOperator::comp_reboot_procedure(int errOccur[])
{
    m_com_completed = false;
    ConfFileParser MyParser;
    ParamList paramList;
    Status_var status;
    
    /*while(1) {
		
		for (int i = 0; i< m_comp_num; i++) {
			status = m_daqservices[i]->getStatus();
        }
        
        if (status->comp_status == COMP_FIXWAIT) {
            break;
        }
        sleep(1);
	}*/
	
    /*
	try {
		m_comp_num = MyParser.readConfFile(m_conf_file.c_str(), true);
        paramList  = MyParser.getParamList();
        		
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
			if (errOccur[i] == 1) {
				set_command(m_daqservices[i], CMD_CONFIGURE);
				check_done(m_daqservices[i]);
			}
        }/// for
    } catch (...) {
        cerr << "### ERROR: DaqOperator: Failed to configure Components.\n";
        return 1;
    }
    */
    
    /** start_procedure() *******************************/
    try {
		
		for (int i = 0; i< m_comp_num; i++) {
			status = m_daqservices[i]->getStatus();
			if (errOccur[i] == 1) {
                set_runno(m_daqservices[i], m_runNumber);
                check_done(m_daqservices[i]);
                
				set_command(m_daqservices[i], CMD_START);
				check_done(m_daqservices[i]);
                errOccur = 0;
			}
        }/// for
    } catch (...) {
        cerr << "### ERROR: DaqOperator: Failed to start Component.\n";
        return 1;
    }

    m_com_completed = true;
    return 0;
}

int DaqOperator::next_procedure(int errOccur[])
{
    m_com_completed = false;
    try {
        time_t now = time(0);

        m_start_date = asctime(localtime(&now));
        m_start_date[m_start_date.length()-1] = ' ';
        m_start_date.erase(0, 4);
        m_stop_date = "";

        if (m_debug) {
            cerr << "start_parocedure: runno: " << m_runNumber << endl;
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
        cerr << "### ERROR: DaqOperator: Failed to start Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::comp_stop_procedure(int errOccur[])
{
    m_com_completed = false;
    
    try {
        
        for (int i = 0; i< m_comp_num; i++) {
            Status_var status;
            status = m_daqservices[i]->getStatus();
            if (errOccur[i] == 1) {
                set_command(m_daqservices[i], CMD_STOP);
                check_done(m_daqservices[i]);			
            }
        }
    } catch (...) {
        cerr << "### ERROR: DaqOperator: Failed to stop Component.\n";
        return 1;
    }
    /*
    try {
        for (int i = 0; i< m_comp_num; i++) {
            Status_var status;
            status = m_daqservices[i]->getStatus();
            if (errOccur[] == 1) {
                set_command(m_daqservices[i], CMD_UNCONFIGURE);
                check_done(m_daqservices[i]);
            }
        }
    } catch(...) {
        cerr << "### ERROR: DaqOperator: Failed to unconfigure Component.\n";
        return 1;
    }
    */
    m_com_completed = true;  
    return 0;
}
/*
int DaqOperator::command_fix()
{
    if (m_state != STOP || m_state != ERROR) {
        createDom_ng("Params");
        cerr << "   Bad Command\n";
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
    
    start_procedure();
    m_state = RUNNING;
    
    createDom_ok("Begin");
    return 0;
}
*/
bool DaqOperator::parse_body(const char* buf, const string tagname)
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
        cerr << "tagname:" << tagname << endl;
    }

    if (strlen(tag) != 0 && tagname == "params") {
        m_config_file_tmp = tag;
        cerr << "*** m_config_file:" << m_config_file_tmp << endl;
    }
    if (strlen(tag) != 0 && tagname == "runNo") {
        int tag_len = strlen(tag);

        if (tag_len > 6 || tag_len < 1) {
            cerr << "DaqOperator: Invalid Run No.\n";
            return false;
        }
        m_runNumber = atoi(tag);

        if (m_debug) {
            cerr << "strlen(tag):" << tag_len << endl;
            cerr << "*** m_runNumber:" << m_runNumber << endl;
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
        cerr << "*** setRunNumber: failed" << endl;
    }

    return 0;
}

int DaqOperator::set_command(RTC::CorbaConsumer<DAQService> daqservice, DAQCommand daqcom)
{
    int status = 0;

    try {
        status = daqservice->setCommand(daqcom);
    }
    catch(...) {
        cerr << "### ERROR: set command: exception occured\n ";
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
        cerr << "### checkDone: failed" << endl;
    }
    return 0;
}

int DaqOperator::set_service_list()
{

    if (m_debug) {
        cerr << "==========================================\n";
        cerr << "\n\n---- service num = " << m_service_num << endl;
        cerr << "==========================================\n";
    }

    m_daqServiceList.clear();

    for (int i = 0; i< m_service_num; i++) {
        RTC::ConnectorProfileList_var myprof;
        myprof = m_DaqServicePorts[i]->get_connector_profiles();
        if (m_debug) {
            cerr << " ====> index     :" << i << endl;
            cerr << " ====> prof name:" << myprof[0].name << endl;
            string id = (string)myprof[0].name;
            cerr << "====> ID: " << id << endl;
        }
        struct serviceInfo serviceInfo;
        serviceInfo.comp_id    = myprof[0].name;
        serviceInfo.daqService = m_daqservices[i];
        m_daqServiceList.push_back(serviceInfo);
    }
    return 0;
}

int DaqOperator::configure_procedure()
{
    if (m_debug) {
        cout << "*** configure_procedure: enter" << endl;
    }
    m_com_completed = false;
    ConfFileParser MyParser;
    ParamList paramList;
    CompGroupList groupList;
    m_start_date = "";
    m_stop_date  = "";

    try
    {
        m_comp_num = MyParser.readConfFile(m_conf_file.c_str(), true);
        paramList  = MyParser.getParamList();
        groupList  = MyParser.getGroupList();

        if (m_debug) {
            cerr << "*** Comp num = " << m_comp_num << endl;
            cerr << "*** paramList.size()  = " << paramList.size() << endl;
            cerr << "*** groupList.size()  = " << groupList.size() << endl;
            cerr << "*** serviceList.size()= " << m_daqServiceList.size() << endl;
        }

        for (int index = 0; index < (int)paramList.size(); index++) {
            if (m_debug) {
                cerr << "ID:" << paramList[index].getId() << endl;
            }
            ::NVList mylist = paramList[index].getList();
            if (m_debug) {
                for (int i = 0; i < (int)mylist.length(); i++) {
                    cerr << "  name :" << mylist[i].name  << endl;
                    cerr << "  value:" << mylist[i].value << endl;
                }
            }
        }
        if (m_debug) {
            for (int i = 0; i< (int)m_daqServiceList.size(); i++) {
                cerr << "*** id:" << m_daqServiceList[i].comp_id << endl;
            }
        }
    } catch (...) {
        cerr << "### ERROR: DaqOperator: Failed to read the Configuration file\n";
        cerr << "### Check the Configuration file\n";
        return 1;
    }

    if (m_debug) {
        cerr << "m_daqServiceList.size():" << m_daqServiceList.size() << endl;
    }

    try {
        for (int i = 0; i < (int)m_daqservices.size(); i++) {
            RTC::ConnectorProfileList_var myprof
                = m_DaqServicePorts[i]->get_connector_profiles();

            char * id = CORBA::string_dup(myprof[0].name);

            if (m_debug) {
                cerr << "*** id:" << id << endl;
            }

            for (int j = 0; j < (int)paramList.size(); j++) {
                if (m_debug) {
                    cerr << "paramList[i].getId():" << paramList[j].getId() << endl;
                }
                if (paramList[j].getId() == id) {
                    if (m_debug) {
                        cerr << "paramList[i].getId():" << paramList[j].getId() << endl;
                        cerr << "m_daqServiceList  id:" << id << endl;
                    }
                    int len = paramList[j].getList().length();
                    if (m_debug) {
                        cerr << "paramList[i].getList().size()" << len << endl;
                    }
                    ::NVList mylist(len);
                    mylist = paramList[j].getList();

                    if (m_debug) {
                        for (int k = 0; k < len; k++) {
                            cerr << "mylist[" << k << "].name: " << mylist[k].name << endl;
                            cerr << "mylist[" << k << "].valu: " << mylist[k].value << endl;
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
        cerr << "### ERROR: DaqOperator: Failed to configure Components.\n";
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
        cerr << "### ERROR: DaqOperator: Failed to unconfigure Component.\n";
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
            cerr << "start_parocedure: runno: " << m_runNumber << endl;
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
        cerr << "### ERROR: DaqOperator: Failed to start Component.\n";
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
        cerr << "### ERROR: DaqOperator: Failed to stop Component.\n";
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
        cerr << "### ERROR: DaqOperator: Failed to pause Component.\n";
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
        cerr << "### ERROR: DaqOperator: Failed to resume Component.\n";
        return 1;
    }
    m_com_completed = true;
    return 0;
}

int DaqOperator::abort_procedure()
{
  cout << "abort_procedure: enter" << endl;

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

    stringstream strstream;
    strstream << m_service_num++;
    string service_name = "service" + strstream.str();
}
void DaqOperator::delCorbaPort()
{
    ;
}
#endif

void DaqOperator::set_console_flag(bool isConsole)
{
    cerr << "set_console_flag(): " << isConsole << endl;
    m_isConsoleMode = isConsole;
}

void DaqOperator::set_port_no(int port)
{
    m_param_port = port;
}

string DaqOperator::getConfFilePath()
{
    string pathFile = ".confFilePath";
    ifstream ifs(pathFile.c_str());
    string mypath = "";
    ifs >> mypath;
    return mypath;
}

string DaqOperator::getMsg()
{
    return m_msg;
}

string DaqOperator::getBody()
{
    return m_body;
}

//
// Command: fix
//


int DaqOperator::command_configure()
{
    //cout << "command_configure: enter" << endl;

    if (m_state != LOADED) {
        createDom_ng("Params");
        cerr << "   Bad Command\n";
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
    //cout << "command_unconfigure: enter" << endl;

     if (m_state != CONFIGURED) {
         createDom_ng("ResetParams");
         cerr << "   Bad Command\n";
         return 1;
    }
    unconfigure_procedure();
    m_state = LOADED;
    createDom_ok("ResetParams");
    return 0;
}

int DaqOperator::command_start()
{
	//cout << "command_start: enter" << endl;

    if (m_state != CONFIGURED) {
        createDom_ng("Begin");
        cerr << "   Bad Command\n";
        return 1;
    }
    start_procedure();
    m_state = RUNNING;
    createDom_ok("Begin");
    return 0;
}

int DaqOperator::command_stop()
{
    //cout << "command_stop: enter" << endl;
    if (m_state != RUNNING) {
        createDom_ng("End");
        cerr << "   Bad Command\n";
        return 1;
    }

    stop_procedure();
    m_state = CONFIGURED;
    createDom_ok("End");

    return 0;
}

int DaqOperator::command_abort()
{
    //cout << "command_abort: enter" << endl;

    if (m_state != RUNNING) {
        createDom_ng("Abort");
        cerr << "   Bad Command\n";
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
    //cout << "command_confirmend: enter" << endl;

    if (m_state == RUNNING) {
        createDom_ng("ConfirmEnd");
        cerr << "   Bad Command\n";
        return 1;
    }

    createDom_ok("ConfirmEnd");

    return 0;
}

int DaqOperator::command_putparams()
{
    //cout << "command_putparams: enter" << endl;

    if (m_state == LOADED) {
        createDom_ng("Params");
        cerr << "   Bad Command\n";
        return 1;
    }

    DAQMW::CreateDom createDom;
    m_msg = createDom.getParams("Params", &m_nv_list[0]);

    return 0;
}

int DaqOperator::command_putstatus()
{
    //cerr << "command_putstatus: enter" << endl;
    putstatus_procedure();

    //Status status1 = m_daqservices[0]->getStatus();
    //::NVList* list = &(status1.list);

    DAQMW::CreateDom createDom;
    //m_msg = createDom.getStatus("Status", m_state, open, list);
    m_msg = createDom.getStatus("Status", m_state);

    //cerr << "Status:msg:" << m_msg << endl;
    return 0;
}

int DaqOperator::command_log()
{
    //cerr << "command_log: enter" << endl;
    //cerr << "m_comp_num: " << m_comp_num << endl;

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
        cerr << "### FATAL: command_log(): " << m_err_msg << endl;
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
    //cout << "command_pause: enter" << endl;
    if (m_state != RUNNING) {
        createDom_ng("Pause");
        cerr << "   Bad Command\n";
        return 1;
    }

    pause_procedure();
    m_state = PAUSED;

    createDom_ok("Pause");

    return 0;
}

int DaqOperator::command_resume()
{
    //cout << "command_resume: enter" << endl;

    if (m_state != PAUSED) {
        createDom_ng("Restart");
        cerr << "   Bad Command\n";
        return 1;
    }

    resume_procedure();
    m_state = RUNNING;
    createDom_ok("Restart");

    return 0;
}

int DaqOperator::command_stopparamsset()
{
    //cout << "command_stopparamsset: enter" << endl;
    createDom_ok("StopParamsSet");

    return 0;
}

int DaqOperator::command_resetparams()
{
    //cout << "command_resetparams: enter" << endl;
    createDom_ok("ResetParams");

    return 0;
}

int DaqOperator::command_save()
{
    //cout << "command_save: enter" << endl;
    createDom_ok("Save");

    return 0;
}

int DaqOperator::command_confirmconnection()
{
    //cout << "command_confirmconnection: enter" << endl;
    createDom_ok("ConfirmConnection");

    return 0;
}

int DaqOperator::command_dummy()
{
    //cout << "command_dummy: enter" << endl;
    return 0;
}

void DaqOperator::createDom_ok(string name)
{
    DAQMW::CreateDom createDom;
    m_msg = createDom.getOK(name);
}

void DaqOperator::createDom_ng(string name)
{
    DAQMW::CreateDom createDom;
    string state = createDom.getState(m_state, false);
    m_msg = "";

    char str_e[128];
    sprintf(str_e, FORMAT_REQ_INV_IN_STS_E, state.c_str());
    char str_j[128];
    sprintf(str_j, FORMAT_REQ_INV_IN_STS_J, state.c_str());

    createDom_ng(name, RET_CODE_REQ_INV_IN_STS, str_e, str_j);
}

void DaqOperator::createDom_ng(string name, int code, char* str_e, char* str_j)
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
