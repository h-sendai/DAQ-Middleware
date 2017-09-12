// -*- C++ -*-
/*!
 * @file  ReaderComp.cpp
 * @brief Standalone component
 * @date $Date$
 *
 * $Id$
 */

#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include "Reader.h"

void MyModuleInit(RTC::Manager* manager)
{
    ReaderInit(manager);
    RTC::RtcBase* comp;

    // Create a component
    comp = manager->createComponent("Reader");

    // Example
    // The following procedure is examples how handle RT-Components.
    // These should not be in this function.

    // Get the component's object reference
    RTC::RTObject_var rtobj;
    rtobj = RTC::RTObject::_narrow(manager->getPOA()->servant_to_reference(comp));

#ifdef RTM04x
    // Get the port list of the component
    PortList* portlist;
    portlist = rtobj->get_ports();


    // getting port profiles
    std::cerr << "Number of Ports: ";
    std::cerr << portlist->length() << std::endl << std::endl;
    for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i) {
        Port_ptr port;
        port = (*portlist)[i];
        std::cerr << "Port" << i << " (name): ";
        std::cerr << port->get_port_profile()->name << std::endl;

        RTC::PortInterfaceProfileList iflist;
        iflist = port->get_port_profile()->interfaces;
        std::cerr << "---interfaces---" << std::endl;
        for (CORBA::ULong i(0), n(iflist.length()); i < n; ++i) {
            std::cerr << "I/F name: ";
            std::cerr << iflist[i].instance_name << std::endl;
            std::cerr << "I/F type: ";
            std::cerr << iflist[i].type_name << std::endl;
            const char* pol;
            pol = iflist[i].polarity == 0 ? "PROVIDED" : "REQUIRED";
            std::cerr << "Polarity: " << pol << std::endl;
        }
        std::cerr << "---properties---" << std::endl;
        //NVUtil::dump(port->get_port_profile()->properties);
        std::cerr << "----------------" << std::endl << std::endl;
    }
#endif

    //ExecutionContextServiceList_var eclist;
    //eclist = rtobj->get_execution_context_services();
    //eclist[0]->activate_component(RTObject::_duplicate( rtobj ));
    ExecutionContextList_var eclist;
    eclist = rtobj->get_owned_contexts();
    eclist[(CORBA::ULong)0]->activate_component(RTObject::_duplicate( rtobj ));

    //CORBA::Double drate = 100000000.0;
    //eclist[0]->set_rate(drate);

    return;
}

int main (int argc, char** argv)
{
    RTC::Manager* manager;
    manager = RTC::Manager::init(argc, argv);

    // Initialize manager
    manager->init(argc, argv);

    // Set module initialization proceduer
    // This procedure will be invoked in activateManager() function.
    manager->setModuleInitProc(MyModuleInit);

    // Activate manager and register to naming service
    manager->activateManager();

    // run the manager in blocking mode
    // runManager(false) is the default.
    manager->runManager();

    // If you want to run the manager in non-blocking mode, do like this
    // manager->runManager(true);

  return 0;
}
