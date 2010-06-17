// -*- C++ -*-
/*!
 * @file DaqOperatorComp.cpp
 * @brief DAQ Controller application
 * @date $Date$
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <rtm/CorbaNaming.h>
#include <rtm/RTObject.h>
#include <rtm/NVUtil.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/CorbaConsumer.h>
#include <assert.h>
#include "DAQServiceSVC_impl.h"
#include "DAQServiceStub.h"
#include "ConfFileParser.h"
#include <rtm/Manager.h>
#include "DaqOperator.h"

using namespace RTC;

struct inport_info {
    std::string inport_name;
    std::string from_name;
    ///Port_ptr inport_ptr;
    PortService_ptr inport_ptr;
    ConnectorProfile prof;
};

struct outport_info {
    std::string outport_name;
    ///Port_ptr outport_ptr;
    PortService_ptr outport_ptr;
};

struct service_info {
    std::string comp_id;
    int startup_order;
    ///Port_ptr service_ptr; /// DAQ-Components servicePorts ptr
    PortService_ptr service_ptr; /// DAQ-Components servicePorts ptr
};

struct operator_service_info {
    std::string comp_id;
    int startup_order;
    ///Port_ptr service_ptr; ///DaqOperators servicePorts ptr
    PortService_ptr service_ptr; ///DaqOperators servicePorts ptr
};

typedef std::vector< inport_info  > InportList;
typedef std::vector< outport_info > OutportList;
typedef std::vector< service_info > DaqSvcList;
typedef std::vector< operator_service_info >  OperatorSvcList;

InportList  inport_list;
OutportList outport_list;
DaqSvcList  service_list;
OperatorSvcList operator_service_list;

typedef CorbaConsumer<RTObject> corbaObj;
typedef std::vector< corbaObj > daq_comps;
CompInfoList daq_comp_list;
int comp_num;
DAQLifeCycleState current_state;
//bool debug = false;
bool debug = true;

bool isConsoleMode = false;
std::string xml_file = "";
//std::string xml_file = "config.xml";
int port_param_server = 30000;
std::string host_ns = "localhost";
std::string port_ns = "9876";
int port_no = 30000;

CorbaNaming* init_orb(const char* hostname, CORBA::ORB_var orb)
{
    int _argc(0);
    char** _argv(0);
    CorbaNaming* naming;

    try {
	orb = CORBA::ORB_init(_argc, _argv);
	std::cerr << "hostname: " << hostname << std::endl;
	std::cerr << " ORB_init done\n";

	naming = new CorbaNaming(orb, hostname);
	std::cerr << " CorbaNaming done\n";
    }
    catch(std::exception& e)
    {
	std::cerr << "### Exception occured: " << e.what() << std::endl;
	std::cerr << "### NO omniNames?" << std::endl;
	exit(1);
    }
    return naming;
}

int find_comps(CorbaNaming* naming, CompGroupList* daq_group_list)
{
    std::cerr << "*** find_components()\n";

    CompInfoList compInfoList;
    ComponentGroup g;

    ComponentInfoContainer p;
    int group_num = daq_group_list->size();
    std::cerr << "group num = " << group_num << std::endl;

    std::string host_info = ".host_cxt/";

    CorbaConsumer<RTObject> comp;

    for(int gindex = 0; gindex < group_num; gindex++) {
	//std::cerr << "gindex = " << gindex << std::endl;
	g = daq_group_list->at(gindex);
	std::string gid = g.getGroupId();
	std::cerr << "===> gid = " << gid << std::endl;

	compInfoList = g.getCompInfoList();
	int comp_num = compInfoList.size();
	std::cerr << "comp num = " << comp_num << std::endl;

	for(int index = 0; index < comp_num; index++) {

	    struct inport_info  inport_info;
	    struct outport_info outport_info;
	    struct service_info service_info;

	    //std::cerr << "index=" << index << std::endl;
	    p = compInfoList.at(index);

	    std::cerr << "*** id   = " << p.getId() << std::endl;
	    std::cerr << "*** name = " << p.getName() << std::endl;
	   
	    //std::string instName = p.getAddress() + ".host_cxt/" + p.getName();
	    std::string instName = p.getAddress() + host_info + p.getName();
	    std::cerr << "**** instName: " << instName << std::endl;

	    try {
		comp.setObject(naming->resolve((const char*)instName.c_str() ));
	    }
	    catch(CORBA::SystemException& ex) {  
		std::cerr << "Caught CORBA::" << ex._name() << std::endl;
		throw;
	    }  
	    catch(CORBA::Exception& ex) {  
		std::cerr << "Caught CORBA::Exception: " << ex._name() << std::endl;  
		throw;
	    }  
	    catch(omniORB::fatalException& fe) {  
		std::cerr << "Caught omniORB::fatalException:" << std::endl;  
		std::cerr << " file: " << fe.file()   << std::endl;  
		std::cerr << " line: " << fe.line()   << std::endl;  
		std::cerr << " mesg: " << fe.errmsg() << std::endl;  
		throw;
	    }
	    catch(...) {
		std::cerr << "### DaqOperator: find_comps: Unknown exception caught\n";
		throw;
	    }

	    RTC::ComponentProfile_var prof;
	    prof = comp->get_component_profile();
#ifdef DEBUG
	    std::cout << "=================================================" << std::endl;
	    std::cout << " Component Profile" << std::endl;
	    std::cout << "-------------------------------------------------" << std::endl;
	    std::cout << "InstanceID:     " << prof->instance_name << std::endl;
	    std::cout << "Implementation: " << prof->type_name << std::endl;
	    std::cout << "Description:    " << prof->description << std::endl;
	    std::cout << "Version:        " << prof->version << std::endl;
	    std::cout << "Maker:          " << prof->vendor << std::endl;
	    std::cout << "Category:       " << prof->category << std::endl;
	    std::cout << "  Other properties   " << std::endl;
	    NVUtil::dump(prof->properties);
	    std::cout << "=================================================" << std::endl;
#endif

	    ///PortList* portlist;
	    PortServiceList* portlist;
	    portlist = comp->get_ports();

	    int outport_count = 0;
	    int inport_count  = 0;

	    for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i)
	    {
		///Port_ptr port;
		PortService_ptr port;
		port = (*portlist)[i];
		//std::cout << "================================================="
		//	  << std::endl;
		//std::cout << "Port" << i << " (name): ";
		std::string pname = (std::string)port->get_port_profile()->name;
		//std::cerr << pname << std::endl;
		//std::cout << "-------------------------------------------------"
		//	  << std::endl;
		RTC::PortInterfaceProfileList iflist;
		iflist = port->get_port_profile()->interfaces;
	      
		for (CORBA::ULong i(0), n(iflist.length()); i < n; ++i)
		{
		    const char* pol;
		    pol = iflist[i].polarity == 0 ? "PROVIDED" : "REQUIRED";
		}
		
		std::string port_type = NVUtil::toString(port->get_port_profile()->properties, 
						       "port.port_type");

		if (port_type == "DataInPort") {
		    std::cerr << "*** DataInPort\n";
		    std::vector<std::string> myInport = p.getInport();
		    std::vector<std::string> myFrom   = p.getFromOutPort();
		    std::cerr << "    myInport:" << myInport[inport_count] << std::endl;
		    std::cerr << "    myFrom:"   << myFrom[inport_count]   << std::endl;

		    inport_info.inport_name = gid + p.getId() + ":" + myInport[0];

		    inport_info.from_name   = gid + myFrom[inport_count];
		    inport_info.inport_ptr  = port;
		    inport_list.push_back(inport_info);
		    inport_count++;
		}
		else if (port_type == "DataOutPort") {
		    std::cerr << "*** DataOutPort\n";
		    std::vector<std::string> myOutport = p.getOutport();
		    std::cerr << "    myOutport:" << myOutport[outport_count] << std::endl;
		    //std::cerr << "*** DataOutPort size:" << myOutport.size() << std::endl;

		    //outport_info.outport_name = myOutport[outport_count];
		    outport_info.outport_name = gid + p.getId() + ":" + myOutport[outport_count];
		    //std::cerr << "*** outport_info.outport_name=" << outport_info.outport_name << std::endl;
		    outport_info.outport_ptr  = port;
		    outport_list.push_back(outport_info);
		    outport_count++;
		}
		else if (port_type == "CorbaPort") {
		    //std::cerr << "*** CorbaPort\n";
		    service_info.comp_id = gid + ":" +p.getId();
		    service_info.startup_order = atoi(p.getStartupOrder().c_str());
		    //std::cerr << "*** service_info.comp_id: " << service_info.comp_id << std::endl;
		    service_info.service_ptr = port;
		    service_list.push_back(service_info);
		}

		//std::cerr << "port_type = " << port_type << std::endl;
		//std::cerr << "from      = " << p->getFromOutPort() << std::endl;

		//std::cout << "-------------------------------------------------" << std::endl;
	    }
	}
    }
    std::cerr << "inport_list.size() =" << inport_list.size() << std::endl;
    std::cerr << "outport_list.size()=" << outport_list.size() << std::endl;
    std::cerr << "service_list.size()=" << service_list.size() << std::endl;
    
    return 0;
}

#ifdef OLD
int connect_comps()
{
    std::cerr << "*** connect comps" << std::endl;
    //std::cerr << "    inport size:" << (int)inport_list.size() << std::endl;
    
    std::cerr << "--------------------------------------------------------\n";
    for(int i=0; i< (int)inport_list.size(); i++) {
	std::cerr << "inport name: " << inport_list[i].inport_name << std::endl;
	std::cerr << "from name: "   << inport_list[i].from_name << std::endl;
    }

    for(int i=0; i< (int)outport_list.size(); i++) {
	std::cerr << "outport name: " << outport_list[i].outport_name << std::endl;
    }
    std::cerr << "--------------------------------------------------------\n";


   ///connect OutPorts to InPorts respectively
    for(int index = 0; index < (int)inport_list.size(); index ++) {
	if (debug) {
	    std::cerr << "InPort name:" << inport_list[index].inport_name << std::endl;
	}
	ConnectorProfile prof;
	std::string connector_name = "connector" + index;
	prof.connector_id = CORBA::string_dup(connector_name.c_str());
	prof.name = CORBA::string_dup(connector_name.c_str());
	prof.ports.length(2);
	prof.ports[0] = inport_list[index].inport_ptr;

	int conn_false_cnt = 0;

	for(int index2 = 0; index2 < (int)outport_list.size(); index2 ++) {
	    //std::cerr << " = OutPort name:" << outport_list[index2].outport_name << std::endl;
	    //std::cerr << " = From name:" << inport_list[index].from_name << std::endl;
	    if (inport_list[index].from_name == outport_list[index2].outport_name) {
		if (debug) {
		    std::cerr << " === Connect ";
		    std::cerr << "  === OutPort name:" << outport_list[index2].outport_name << std::endl;
		    std::cerr << "  === InPort name:" << inport_list[index].inport_name << std::endl;
		}

		prof.ports[1] = outport_list[index2].outport_ptr;

		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.interface_type",
						       ///"CORBA_Any"));
						       "corba_cdr"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.dataflow_type",
						       "Push"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.subscription_type",
						       "Flush"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.push_interval",
						       "1.0"));
		ReturnCode_t ret;
		ret = prof.ports[0]->connect(prof);
		assert(ret == RTC::RTC_OK);

		//std::cout << "Connector ID: " << prof.connector_id << std::endl;
		if (debug) {
		    NVUtil::dump(prof.properties);
		}
	    }
	    else {
		conn_false_cnt++;
		if (conn_false_cnt == (int)outport_list.size()) {
		    std::cerr << "### ERROR: DaqOperatorComp: connection failed\n";
		    std::cerr << "### Check config.xml file\n";
		    exit(1);
		}
		//std::cerr << "not match\n";
		//std::cerr << "  === OutPort name:" << outport_list[index2].outport_name << std::endl;
		//std::cerr << "  === InPort name:" << inport_list[index].from_name << std::endl;
	    }
	}
    }

    std::cerr << "service_list.size() = " << service_list.size() << std::endl;
    std::cerr << "operator_port.size() = " << operator_service_list.size() << std::endl;
    int index_operator = 0;

    ///connect DAQ-Componets servicePorts to DaqOperators respectively
    for(int index=0; index < (int)service_list.size(); index++) {
	if (debug) {
	    std::cerr << "service_list[" << index << "]:" << service_list[index].comp_id << std::endl;
	}

	if (service_list[index].comp_id != "DaqOperator") { /// not a DaqOperator
	    ConnectorProfile prof;
	    //std::string connector_name = "connector_svc_" + service_list[index].comp_id;
	    std::string connector_name = service_list[index].comp_id;
	    //std::cerr << "connector name: " << connector_name << std::endl;
	    prof.connector_id = CORBA::string_dup(connector_name.c_str());
	    prof.name = CORBA::string_dup(connector_name.c_str());
	    prof.ports.length(2);
	   
	    int startOrder = service_list[index].startup_order - 1; /// startup order begins 1
	    prof.ports[0] = service_list[index].service_ptr;
	    ///prof.ports[1] = operator_service_list[index].service_ptr;
	    prof.ports[1] = operator_service_list[startOrder].service_ptr;
	    //std::cerr << "====> index = " << startOrder << std::endl;
	   
	    operator_service_list[startOrder].comp_id = service_list[index].comp_id;

	    ReturnCode_t ret = RTC::RTC_OK;
	    try {
		ret = prof.ports[0]->connect(prof);
	    }
	    catch(...) {
		std::cerr << "connenct: Exception occured\n";
	    }
	    assert(ret == RTC::RTC_OK);
	    
	    index_operator++;
	}
    }

    return 0;
}
#endif


int connect_data_ports()
{
    std::cerr << "*** connect comps" << std::endl;
    //std::cerr << "    inport size:" << (int)inport_list.size() << std::endl;
    
    std::cerr << "--------------------------------------------------------\n";
    for(int i=0; i< (int)inport_list.size(); i++) {
	std::cerr << "inport name: " << inport_list[i].inport_name << std::endl;
	std::cerr << "from name: "   << inport_list[i].from_name << std::endl;
    }

    for(int i=0; i< (int)outport_list.size(); i++) {
	std::cerr << "outport name: " << outport_list[i].outport_name << std::endl;
    }
    std::cerr << "--------------------------------------------------------\n";


   ///connect OutPorts to InPorts respectively
    for(int index = 0; index < (int)inport_list.size(); index ++) {
	if (debug) {
	    std::cerr << "InPort name:" << inport_list[index].inport_name << std::endl;
	}
	ConnectorProfile prof;
	std::string connector_name = "connector" + index;
	prof.connector_id = CORBA::string_dup(connector_name.c_str());
	prof.name = CORBA::string_dup(connector_name.c_str());
	prof.ports.length(2);
	prof.ports[0] = inport_list[index].inport_ptr;

	int conn_false_cnt = 0;

	for(int index2 = 0; index2 < (int)outport_list.size(); index2 ++) {
	    //std::cerr << " = OutPort name:" << outport_list[index2].outport_name << std::endl;
	    //std::cerr << " = From name:" << inport_list[index].from_name << std::endl;
	    if (inport_list[index].from_name == outport_list[index2].outport_name) {
		if (debug) {
		    std::cerr << " === Connect ";
		    std::cerr << "  === OutPort name:" << outport_list[index2].outport_name << std::endl;
		    std::cerr << "  === InPort name:" << inport_list[index].inport_name << std::endl;
		}

		prof.ports[1] = outport_list[index2].outport_ptr;

		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.interface_type",
						       ///"CORBA_Any"));
						       "corba_cdr"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.dataflow_type",
						       "push"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.subscription_type",
						       "flush"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.push_interval",
						       "1.0"));
		/**
		 *  Added new buffer properties from OpenRTM-aist-1.0.0
		 */
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.inport.buffer.read.empty_policy",
						       "block"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.inport.buffer.write.full_policy",
						       "block"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.inport.buffer.read.timeout",
						       "0.1"));
		CORBA_SeqUtil::push_back(prof.properties,
					 NVUtil::newNV("dataport.inport.buffer.write.timeout",
						       "0.1"));

		ReturnCode_t ret;
		ret = prof.ports[0]->connect(prof);
		assert(ret == RTC::RTC_OK);

		//std::cout << "Connector ID: " << prof.connector_id << std::endl;
		if (debug) {
		    NVUtil::dump(prof.properties);
		}
	    }
	    else {
		conn_false_cnt++;
		if (conn_false_cnt == (int)outport_list.size()) {
		    std::cerr << "### ERROR: DaqOperatorComp: connection failed\n";
		    std::cerr << "### Check config.xml file\n";
		    exit(1);
		}
		//std::cerr << "not match\n";
		//std::cerr << "  === OutPort name:" << outport_list[index2].outport_name << std::endl;
		//std::cerr << "  === InPort name:" << inport_list[index].from_name << std::endl;
	    }
	}
    }
    return 0;
}


int connect_service_ports()
{
    std::cerr << "service_list.size() = " << service_list.size() << std::endl;
    std::cerr << "operator_port.size() = " << operator_service_list.size() << std::endl;
    int index_operator = 0;

    ///connect DAQ-Componets servicePorts to DaqOperators respectively
    for(int index=0; index < (int)service_list.size(); index++) {
	if (debug) {
	    std::cerr << "service_list[" << index << "]:" << service_list[index].comp_id << std::endl;
	}

	if (service_list[index].comp_id != "DaqOperator") { /// not a DaqOperator
	    ConnectorProfile prof;
	    //std::string connector_name = "connector_svc_" + service_list[index].comp_id;
	    std::string connector_name = service_list[index].comp_id;
	    //std::cerr << "connector name: " << connector_name << std::endl;
	    prof.connector_id = CORBA::string_dup(connector_name.c_str());
	    prof.name = CORBA::string_dup(connector_name.c_str());
	    prof.ports.length(2);
	   
	    int startOrder = service_list[index].startup_order - 1; /// startup order begins 1
	    prof.ports[0] = service_list[index].service_ptr;
	    ///prof.ports[1] = operator_service_list[index].service_ptr;
	    prof.ports[1] = operator_service_list[startOrder].service_ptr;
	    //std::cerr << "====> index = " << startOrder << std::endl;
	   
	    operator_service_list[startOrder].comp_id = service_list[index].comp_id;

	    ReturnCode_t ret = RTC::RTC_OK;
	    try {
		ret = prof.ports[0]->connect(prof);
	    }
	    catch(...) {
		std::cerr << "connenct: Exception occured\n";
	    }
	    assert(ret == RTC::RTC_OK);
	    
	    index_operator++;
	}
    }

    return 0;
}

bool find_connect_comps()
{
    CORBA::ORB_var orb;
    CorbaNaming* naming;

    CompGroupList compGroupList;

    ///Read and Parse Configuration file
    ConfFileParser MyParser;
    std::cerr << "Conf file: " << xml_file.c_str() << std::endl;


    try {
	MyParser.readConfFile(xml_file.c_str(), false);
	std::cerr << "-----------readconfile\n";
	compGroupList  = MyParser.getGroupList();
	int group_num = compGroupList.size();
	std::cerr << "DaqOperatorComp: group_num = " << group_num << std::endl;
	std::cerr << "Corba NS:port = " << host_ns << std::endl;
	
	int    _argc(0);
	char** _argv(0);

	//try {
	orb = CORBA::ORB_init(_argc, _argv);
	std::cerr << "hostname: " << host_ns << std::endl;
	std::cerr << " ORB_init done\n";

	naming = new CorbaNaming(orb, host_ns.c_str());
	std::cerr << " CorbaNaming done\n";

	//eclist[0]->activate_component(RTObject::_duplicate( rtobj ));

	///Find Components in CORBA Name Server using info in Config. file
	find_comps(naming, &compGroupList);
	std::cerr << "find comps done\n";
	///Connect Comps if found

	std::cerr << "***** service_list.size(): " << service_list.size()  << std::endl;
	if (service_list.size() > 1) {
	    connect_data_ports();
	}
	connect_service_ports();
	///eclist[0]->activate_component(RTObject::_duplicate( rtobj ));

    }
    catch(std::exception& e)
    {
	std::cerr << "### DaqOperator: find_connect_comps: Exception occured: " 
		  << e.what() << std::endl;
	std::cerr << "### NO omniNames?" << std::endl;
	throw;
    }
    catch(...) {
	std::cerr << "### DaqOperator: find_connect_comps: Unknown exception occured" 
		  << std::endl;
	throw;
    }
    return true;
}

void MyModuleInit(RTC::Manager* manager)
{
    std::cerr << "*** MyModuleInit\n" ;
    std::cerr << "conf:" << xml_file << std::endl;
    DaqOperatorInit(manager);
    RTC::RtcBase* comp;

    struct operator_service_info operator_service_info;
    // Create a component
    std::cerr << "Creating a component: \"DaqOperator\"....";
    comp = manager->createComponent("DaqOperator");
    std::cerr << "succeed." << std::endl;

    DaqOperator* daq = (DaqOperator*)comp;
    daq->set_console_flag(isConsoleMode);
    //daq->set_port_no(port_param_server);
    //daq->set_conf_file(xml_file.c_str());
    std::cerr << "conf:" << xml_file << std::endl;

    RTC::ComponentProfile_var prof;
    prof = comp->get_component_profile();

#ifdef DEBUG  
    std::cout << "=================================================" << std::endl;  
    std::cout << " Component Profile" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;  
    std::cout << "InstanceID:     " << prof->instance_name << std::endl;
    std::cout << "Implementation: " << prof->type_name << std::endl;
    std::cout << "Description:    " << prof->description << std::endl;
    std::cout << "Version:        " << prof->version << std::endl;
    std::cout << "Maker:          " << prof->vendor << std::endl;
    std::cout << "Category:       " << prof->category << std::endl;
    std::cout << "  Other properties   " << std::endl;
    NVUtil::dump(prof->properties);
    std::cout << "=================================================" << std::endl;
#endif

    ///PortList* portlist;
    PortServiceList* portlist;
    portlist = comp->get_ports();

    for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i) {
	///Port_ptr port;
	PortService_ptr port;
	port = (*portlist)[i];

//#ifdef DEBUG
	std::cout << "================================================="
		  << std::endl;
	std::cout << "Port" << i << " (name): ";
	std::cout << port->get_port_profile()->name << std::endl;
	std::cout << "-------------------------------------------------"
		  << std::endl;
//#endif

	RTC::PortInterfaceProfileList iflist;
	iflist = port->get_port_profile()->interfaces;

	std::string port_type = NVUtil::toString(
	    port->get_port_profile()->properties, "port.port_type");

	if (port_type == "CorbaPort") {
	    operator_service_info.service_ptr = port;
	    operator_service_list.push_back(operator_service_info);
	}

	NVUtil::dump(port->get_port_profile()->properties);
	//std::cout << "-------------------------------------------------" << std::endl;
    }

    RTC::RTObject_var rtobj;
    rtobj = RTC::RTObject::_narrow(manager->getPOA()->servant_to_reference(comp));
    ///ExecutionContextServiceList_var eclist;
    ///eclist = rtobj->get_execution_context_services();

    ExecutionContextList_var eclist;
    eclist = rtobj->get_owned_contexts();
    //eclist[0]->activate_component(RTObject::_duplicate( rtobj ));
    eclist[(CORBA::ULong)0]->activate_component(RTObject::_duplicate(rtobj));

    eclist[0]->activate_component(RTObject::_duplicate( rtobj ));
#ifdef ORG
    CORBA::ORB_var orb;
    CorbaNaming* naming;

    CompGroupList compGroupList;

    ///Read and Parse Configuration file
    ConfFileParser MyParser;
    std::cerr << "Conf file: " << xml_file.c_str() << std::endl;


    try {
	MyParser.readConfFile(xml_file.c_str(), false);
	std::cerr << "-----------readconfile\n";
	compGroupList  = MyParser.getGroupList();
	int group_num = compGroupList.size();
	std::cerr << "DaqOperatorComp: group_num = " << group_num << std::endl;
	std::cerr << "Corba NS:port = " << host_ns << std::endl;
	
	int    _argc(0);
	char** _argv(0);

	//try {
	orb = CORBA::ORB_init(_argc, _argv);
	std::cerr << "hostname: " << host_ns << std::endl;
	std::cerr << " ORB_init done\n";

	naming = new CorbaNaming(orb, host_ns.c_str());
	std::cerr << " CorbaNaming done\n";

	//eclist[0]->activate_component(RTObject::_duplicate( rtobj ));

	///Find Components in CORBA Name Server using info in Config. file
	find_comps(naming, &compGroupList);
	std::cerr << "find comps done\n";
	///Connect Comps if found

	std::cerr << "***** service_list.size(): " << service_list.size()  << std::endl;
	if (service_list.size() > 1) {
	    connect_data_ports();
	}
	connect_service_ports();
	///eclist[0]->activate_component(RTObject::_duplicate( rtobj ));

    }
    catch(std::exception& e)
    {
	std::cerr << "### Exception occured: " << e.what() << std::endl;
	std::cerr << "### NO omniNames?" << std::endl;
	throw;
    }
    catch(...) {
	std::cerr << "### Exception occured" << std::endl;
	throw;
    }
#endif

	
    std::cerr << "MyModuleInit exit\n";
    return;
}

int confFilePath(std::string path)
{
    std::string conf_file_path = "./.confFilePath";
    try {
        std::ofstream outFile( conf_file_path.c_str() );
	outFile << path << std::endl;
    }
    catch(...) {
        std::cerr << "### ERROR: open file: exception occured\n";
	return -1;
    }

    return 0;
}

int main (int argc, char** argv)
{
    int result;

    /**
       f: File name of Configuration file
       w: Port NO. of Parameter Server
       h: Host name of Name Server of Omni ORB
       p: Port NO. of Name Server of Omni ORB
       c: Use console mode
    */

    while( (result = getopt(argc, argv, "x:w:h:p:f:c")) != -1 ) {
        switch(result) {
        case 'c':
            isConsoleMode = true;
            std::cerr << "Use console mode" << std::endl;
            break;
        case 'x':
            xml_file = optarg;
            std::cerr << "Configuration file: " << xml_file << std::endl;
            break;
        case 'w':
            port_param_server = atoi(optarg);
            std::cerr << "Port NO. of Param. Server: "
                      << port_param_server << std::endl;
            break;
        case 'h':
            host_ns = optarg;
            std::cerr << "Host name of Corba NS: " << host_ns << std::endl;
            break;
        case 'p':
            port_ns = optarg;
            std::cerr << "Port NO. of Corba NS: "
                      << port_ns << std::endl;
            break;
        }
    }

    std::cerr << "*******************\n";
    std::cerr << "Use console mode" << std::endl;
    std::cerr << "Configuration file    : " << xml_file << std::endl;
    std::cerr << "Port NO. of Param. Svr: " << port_param_server << std::endl;
    std::cerr << "Host name of Corba NS : " << host_ns << std::endl;
    std::cerr << "Port NO. of Corba NS  : " << port_ns << std::endl;
    std::cerr << "*******************\n";
    std::cerr << "isConsoleMode: " << isConsoleMode << std::endl;
    std::cerr << "port_no: " << port_no << std::endl;
    host_ns += ":";
    host_ns += port_ns;
    std::cerr << "Host name of Corba NS: " << host_ns << std::endl;
    std::cerr << "*******************\n";

    if ( confFilePath(xml_file) != 0) {
	std::cerr << "### ERROR: cannot create .confFilePath file\n";
	return -1;
    }


    RTC::Manager* manager;

    argc = 0;
    argv = 0;
    manager = RTC::Manager::init(argc, argv);

    // Initialize manager
    ///manager->init(argc, argv);

    // Set module initialization proceduer
    // This procedure will be invoked in activateManager() function.
    manager->setModuleInitProc(MyModuleInit);



    // Activate manager and register to naming service
    manager->activateManager();

    find_connect_comps();

    for(int i = 0; i < (int)operator_service_list.size(); i++) {
	std::cerr << "Comp ID: " 
		  << operator_service_list[i].comp_id << std::endl;
    }

    // run the manager in blocking mode
    // runManager(false) is the default.
    std::cerr << "runManager...\n";
    manager->runManager();

    // If you want to run the manager in non-blocking mode, do like this
    // manager->runManager(true);

    return 0;
}

