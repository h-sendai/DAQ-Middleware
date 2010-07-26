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
    PortService_ptr inport_ptr;
    ConnectorProfile prof;
};

struct outport_info {
    std::string outport_name;
    PortService_ptr outport_ptr;
};

struct service_info {
    std::string comp_id;
    int startup_order;
    PortService_ptr service_ptr; /// DAQ-Components servicePorts ptr
};

struct operator_service_info {
    std::string comp_id;
    int startup_order;
    PortService_ptr service_ptr; /// DaqOperator servicePorts ptr
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
bool debug = false;

bool isConsoleMode = false;        //initial value
std::string xml_file = "";         //initial value
int port_param_server = 30000;     //initial value
std::string host_ns = "localhost"; //initial value
std::string port_ns = "9876";      //initial value
const int port_no = 30000;
const int FIND_COMP_RETRY_MAX_CNTS = 20;

CorbaNaming* init_orb(const char* hostname, CORBA::ORB_var orb)
{
    int    _argc(0);
    char** _argv(0);
    CorbaNaming* naming;

    try {
        orb = CORBA::ORB_init(_argc, _argv);
        if (debug) {
            std::cerr << " hostname: " << hostname << std::endl;
            std::cerr << " ORB_init done\n";
        }

        naming = new CorbaNaming(orb, hostname);
        if (debug) {
            std::cerr << " CorbaNaming done\n";
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "### Exception occured: " << e.what() << std::endl;
        std::cerr << "### NO omniNames?" << std::endl;
        exit(-1);
    }
    return naming;
}

int find_comps(CorbaNaming* naming, CompGroupList* daq_group_list)
{
    if (debug) {
        std::cerr << "*** find_components()\n";
    }

    CompInfoList compInfoList;
    ComponentGroup g;

    ComponentInfoContainer p;
    int group_num = daq_group_list->size();
    if (debug) {
        std::cerr << "group num = " << group_num << std::endl;
    }
    std::string host_info = ".host_cxt/";

    CorbaConsumer<RTObject> comp;

    for(int gindex = 0; gindex < group_num; gindex++) {
        //std::cerr << "gindex = " << gindex << std::endl;
        g = daq_group_list->at(gindex);
        std::string gid = g.getGroupId();
        if (debug) {
            std::cerr << "===> gid = " << gid << std::endl;
        }
        compInfoList = g.getCompInfoList();
        int comp_num = compInfoList.size();
        if (debug) {
            std::cerr << "comp num = " << comp_num << std::endl;
        }

        for(int index = 0; index < comp_num; index++) {

            struct inport_info  inport_info;
            struct outport_info outport_info;
            struct service_info service_info;

            p = compInfoList.at(index);
            if (debug) {
                std::cerr << "*** id   = " << p.getId()   << std::endl;
                std::cerr << "*** name = " << p.getName() << std::endl;
            }

            //std::string instName = p.getAddress() + ".host_cxt/" + p.getName();
            std::string instName = p.getAddress() + host_info + p.getName();
            if (debug) {
                std::cerr << "**** instName: " << instName << std::endl;
            }
            //int retry_counts = 0;
            int retry_counts = FIND_COMP_RETRY_MAX_CNTS;

            bool found_comp = false;
            while (retry_counts) {
                try {
                    bool ret 
                        = comp.setObject(naming->resolve((const char*)instName.c_str() ));
                    if (ret == true) {
                        found_comp = true;
                        break;
                    }
                }
                catch(CosNaming::NamingContext::NotFound& e) {
                    std::cerr << "Could not find a Component, retry..." << std::endl;
                    retry_counts--;
                    usleep(500000); //500ms
                }
                catch(CosNaming::NamingContext::CannotProceed& e) {
                    std::cerr << "Caught exception: Cannot Proceed" << std::endl;
                    throw;
                }
                catch(CosNaming::NamingContext::InvalidName& e) {
                    std::cerr << "Caught exception: Invalid Name" << std::endl;
                    throw;
                }
                catch(...) {
                    std::cerr << "Caught exception: Unknown exception" << std::endl;
                    throw;
                }
            }

            if (found_comp == false) {
                std::cerr << "### ERROR: Could not found a DAQ-Component:"
                          << instName << std::endl;
                std::cerr << "Check your configuration file" << std::endl;
                exit(-1);
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

            PortServiceList* portlist;
            portlist = comp->get_ports();

            int outport_count = 0;
            int inport_count  = 0;

            for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i)
            {
                PortService_ptr port;
                port = (*portlist)[i];
                std::string pname = (std::string)port->get_port_profile()->name;
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
                    if (debug) {
                        std::cerr << "    myInport:" << myInport[inport_count] << std::endl;
                        std::cerr << "    myFrom:"   << myFrom[inport_count]   << std::endl;
                    }

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
                    outport_info.outport_name = gid + p.getId() + ":" + myOutport[outport_count];
                    outport_info.outport_ptr  = port;
                    outport_list.push_back(outport_info);
                    outport_count++;
                }
                else if (port_type == "CorbaPort") {
                    service_info.comp_id = gid + ":" +p.getId();
                    service_info.startup_order = atoi(p.getStartupOrder().c_str());
                    service_info.service_ptr = port;
                    service_list.push_back(service_info);
                }
            }
        }
    }
    if (debug) {
        std::cerr << "inport_list.size() =" << inport_list.size() << std::endl;
        std::cerr << "outport_list.size()=" << outport_list.size() << std::endl;
        std::cerr << "service_list.size()=" << service_list.size() << std::endl;
    }
    return 0;
}

int connect_data_ports()
{
    if (debug) {
        std::cerr << "*** connect comps" << std::endl;
        std::cerr << "--------------------------------------------------------\n";
        for(int i=0; i< (int)inport_list.size(); i++) {
            std::cerr << "inport name: " << inport_list[i].inport_name << std::endl;
            std::cerr << "from name: "   << inport_list[i].from_name << std::endl;
        }

        for(int i=0; i< (int)outport_list.size(); i++) {
            std::cerr << "outport name: " << outport_list[i].outport_name << std::endl;
        }
        std::cerr << "--------------------------------------------------------\n";
    }

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
                CORBA_SeqUtil::push_back(prof.properties,
                                         NVUtil::newNV("dataport.inport.buffer.length",
                                                       "128"));
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
                    exit(-1);
                }
            }
        }
    }
    return 0;
}


int connect_service_ports()
{
    if (debug) {
        std::cerr << "service_list.size() = " << service_list.size() << std::endl;
        std::cerr << "operator_port.size() = " << operator_service_list.size() << std::endl;
    }
    int index_operator = 0;

    ///connect DAQ-Componets servicePorts to DaqOperators respectively
    for(int index=0; index < (int)service_list.size(); index++) {
        if (debug) {
            std::cerr << "service_list[" << index << "]:" << service_list[index].comp_id << std::endl;
        }

        if (service_list[index].comp_id != "DaqOperator") { /// not a DaqOperator
            ConnectorProfile prof;
            std::string connector_name = service_list[index].comp_id;
            prof.connector_id = CORBA::string_dup(connector_name.c_str());
            prof.name = CORBA::string_dup(connector_name.c_str());
            prof.ports.length(2);

            int startOrder = service_list[index].startup_order - 1; /// startup order begins 1
            prof.ports[0] = service_list[index].service_ptr;
            prof.ports[1] = operator_service_list[startOrder].service_ptr;

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
    if (debug) {
        std::cerr << "Conf file: " << xml_file.c_str() << std::endl;
    }
    try {
        MyParser.readConfFile(xml_file.c_str(), false);

        compGroupList  = MyParser.getGroupList();
        int group_num = compGroupList.size();
        if (debug) {
            std::cerr << "*** readconfile\n";
            std::cerr << "DaqOperatorComp: group_num = " << group_num << std::endl;
            std::cerr << "Corba NS:port = " << host_ns << std::endl;
        }
        int    _argc(0);
        char** _argv(0);

        orb = CORBA::ORB_init(_argc, _argv);
        if (debug) {
            std::cerr << "hostname: " << host_ns << std::endl;
            std::cerr << " ORB_init done\n";
        }
        naming = new CorbaNaming(orb, host_ns.c_str());
        if (debug) {
            std::cerr << " CorbaNaming done\n";
        }

        ///Find Components in CORBA Name Server using info in Config. file
        find_comps(naming, &compGroupList);

        if (debug) {
            std::cerr << "find comps done\n";
        }
        ///Connect Comps if found
        if (debug) {
            std::cerr << "***** service_list.size(): " << service_list.size()  << std::endl;
        }
        if (service_list.size() > 1) {
            connect_data_ports();
        }
        connect_service_ports();
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
    if (debug) {
        std::cerr << "*** MyModuleInit\n" ;
        std::cerr << "conf:" << xml_file << std::endl;
    }
    DaqOperatorInit(manager);
    RTC::RtcBase* comp;

    struct operator_service_info operator_service_info;
    // Create a component
    if (debug) {
        std::cerr << "Creating a component: \"DaqOperator\"....";
    }
    comp = manager->createComponent("DaqOperator");
    if (debug) {
        std::cerr << "succeed." << std::endl;
    }
    DaqOperator* daq = (DaqOperator*)comp;
    daq->set_console_flag(isConsoleMode);
    if (debug) {
    std::cerr << "conf:" << xml_file << std::endl;
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

    for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i) {
        PortService_ptr port;
        port = (*portlist)[i];

#ifdef DEBUG
       std::cout << "================================================="
                 << std::endl;
       std::cout << "Port" << i << " (name): ";
       std::cout << port->get_port_profile()->name << std::endl;
       std::cout << "-------------------------------------------------"
                 << std::endl;
#endif

       RTC::PortInterfaceProfileList iflist;
       iflist = port->get_port_profile()->interfaces;

       std::string port_type = NVUtil::toString(
           port->get_port_profile()->properties, "port.port_type");

       if (port_type == "CorbaPort") {
           operator_service_info.service_ptr = port;
           operator_service_list.push_back(operator_service_info);
       }

       NVUtil::dump(port->get_port_profile()->properties);
    }

    RTC::RTObject_var rtobj;
    rtobj = RTC::RTObject::_narrow(manager->getPOA()->servant_to_reference(comp));

    ExecutionContextList_var eclist;
    eclist = rtobj->get_owned_contexts();
    eclist[(CORBA::ULong)0]->activate_component(RTObject::_duplicate(rtobj));
    eclist[0]->activate_component(RTObject::_duplicate( rtobj ));
    if (debug) {
        std::cerr << "MyModuleInit exit\n";
    }
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

    host_ns += ":";
    host_ns += port_ns;
    if (debug) {
        std::cerr << "*******************\n";
        std::cerr << "Use console mode"         << std::endl;
        std::cerr << "Configuration file    : " << xml_file << std::endl;
        std::cerr << "Port NO. of Param. Svr: " << port_param_server << std::endl;
        std::cerr << "Host name of Corba NS : " << host_ns << std::endl;
        std::cerr << "Port NO. of Corba NS  : " << port_ns << std::endl;
        std::cerr << "*******************\n";
        std::cerr << "isConsoleMode: "          << isConsoleMode << std::endl;
        std::cerr << "port_no: "                << port_no << std::endl;
        std::cerr << "Host name of Corba NS: "  << host_ns << std::endl;
        std::cerr << "*******************\n";
    }
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

    if (debug) {
        for(int i = 0; i < (int)operator_service_list.size(); i++) {
            std::cerr << "Comp ID: "
                      << operator_service_list[i].comp_id << std::endl;
        }
    }
    // run the manager in blocking mode
    // runManager(false) is the default.
    manager->runManager();

    // If you want to run the manager in non-blocking mode, do like this
    // manager->runManager(true);

    return 0;
}

