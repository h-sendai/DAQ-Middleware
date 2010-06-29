// -*- C++ -*-
#ifndef PARAMETERSERVER_H
#define PARAMETERSERVER_H

#include <iostream>
#include <pthread.h>
#include <string>
#include <map>
#include "Parameter.h"
#include "Sock.h"

namespace DAQMW {

  class Sock;

    class ParameterServer {
    public:
	ParameterServer(int port);
	ParameterServer(int port, std::string host);
	ParameterServer(int port, std::string host, std::string delimitor);
	virtual ~ParameterServer();

	virtual void* Run();
	void bind(std::string id, std::string* valueP, CallBackFunction call);
	void bind(std::string id, std::string* valueP);
	int getParam(std::string id, Parameter* p);
	int extCmdVal(std::string command, std::string* com, std::string* value);
	void setMsg(std::string msg);

    private:
	Sock m_newSock;
	Sock m_server;
	std::map<std::string,Parameter> m_paramBank;
	Parameter m_param;
	int m_port;
        std::string m_host;
	std::string m_delimitor;
	std::string m_msg;
    };

    inline ParameterServer::ParameterServer(int port)
	:m_port(port), m_delimitor(":")
    {
      try {
	m_server.create();
	m_server.bind(port);
	m_server.listen();
	std::cout << "port only" <<  std::endl;
	std::cerr << "ParameterServer create: port =" << port << std::endl;
      } catch (...) {
	std::cerr << "ParameterServer create: Fail..." << port << std::endl;
      }
    }

    inline ParameterServer::ParameterServer(int port, std::string host)
	:m_port(port), m_host(host), m_delimitor(":")
    {
      try {
	m_server.create();
	m_server.bind(port, host.c_str());
	m_server.listen();
	std::cerr << "ParameterServer create: host = " << host << "  port =" << port << std::endl;
      } catch (...) {
	std::cerr << "ParameterServer create: Fail..." << port << std::endl;
      }
    }

    inline ParameterServer::ParameterServer(int port, std::string host, std::string delimitor)
	:m_port(port), m_delimitor(delimitor)
    {
      try {
	m_server.create();
	m_server.bind(port, host.c_str());
	m_server.listen();
	std::cout << "delimitor" <<  std::endl;
	std::cerr << "ParameterServer create: port =" << port << std::endl;
      } catch (...) {
	std::cerr << "ParameterServer create: Fail..." << port << std::endl;
      }
    }

    inline ParameterServer::~ParameterServer()
    {
    }

    inline void ParameterServer::bind(std::string id, std::string* valueP, CallBackFunction call)
    {
	m_param.set(valueP, call);
	m_paramBank.insert(std::map<std::string,Parameter>::value_type(id,m_param));
    }

    inline void ParameterServer::bind(std::string id, std::string* valueP)
    {
	m_param.set(valueP, (CallBackFunction)0);
	m_paramBank.insert(std::map<std::string,Parameter>::value_type(id,m_param));
    }

    inline int ParameterServer::getParam(std::string id, Parameter* p) {
	std::map<std::string,Parameter>::iterator it = m_paramBank.find(id);
	if (it != m_paramBank.end()) {
	    *p = it->second;
	    return 1;
	} else {
	    return 0;
	}
    }

    inline int ParameterServer::extCmdVal(std::string command, std::string* com, 
					  std::string* value) {
	int status;
	std::string tag;

	*com = command.substr(0,3); // "get" or "put"

	std::string command1 = command.substr(4); // tag (and value)
	std::string::size_type index = command1.find(m_delimitor);
	tag = command1.substr(0,index);

	std::string id = *com + m_delimitor + tag; // id = get:tag or put:tag
	//std::cout << "id = " << id << std::endl;
	status = getParam(id, &m_param);
	// check m_param
	if (!status) {
	  std::cout << "could not find command" << std::endl;
	    return status;
	}
	if(*com == "put")
	  *value = command1.substr(index+1);
	return status;
    }

    inline void ParameterServer::setMsg(std::string msg) {
	m_msg = msg;
    }

    inline void* ParameterServer::Run() {
	try {
	  //std::cerr << "ParameterServer::Run()\n";
	  m_server.accept ( m_newSock );

	  int status;
	  CallBackFunction callback;
	    
	  std::string command, com, value;
	  int msgSiz=0;
	  m_newSock.recvAll((unsigned int*)&msgSiz, 4);
	  m_newSock.recvAll(command, msgSiz);
	    
	  //std::cerr << "ParameterServer::Run command = " << command << std::endl;
	  // for "put", com="put" and value = string followed.
	  // for "get", com="get" and value is no meaning
	  // status = 1 for "get" and "put". Otherwise 0.
	  status = extCmdVal(command, &com, &value);
	  if (!status) {
	    std::cerr << "Invalid command" << std::endl;
	    std::string ng = "NG";
	    unsigned siz = ng.size();
	    m_newSock.sendAll(&siz, sizeof(siz));	    
	    m_newSock.sendAll(ng);
	  } else {
	    if(com == "put") {
	      //		std::cerr << "comand is put" << std::endl;
	      m_msg = value;
	      *(m_param.getValueP()) = value;
	    }
	    if(com == "get") {
	      //		std::cerr << "comand is get" << std::endl;
	      m_msg =  *(m_param.getValueP());
	    }
	    //std::cout << "ParameterServer::Run status = " << status 
	    //          << " com = " << com << " value = " << m_msg << std::endl;
	    // call callback function if it exists
	    if ((callback = m_param.getCallBackFunc()) != (CallBackFunction)0) {
	      (*(m_param.getCallBackFunc()))();
	    }

	    int size = m_msg.length();
	    int length = sizeof(int)+size;
	    int storeLength = length%sizeof(int);
	    if(storeLength)
	      storeLength = length/sizeof(int) + 1;
	    else
	      storeLength = length/sizeof(int);
	    storeLength++; // for terminator
	    unsigned int* buf = new unsigned int[storeLength];
	    buf[0] = size;
	    memcpy(&buf[1], m_msg.c_str(), size);
	    m_newSock.sendAll(buf, length);
	    delete [] buf;
	    m_newSock.disconnect();
	  }
	} catch ( SockException& e ) {
	  std::cerr << "ParameterServer: Exception was caught:" 
		    << e.what();
	} catch (...) {
	    std::cerr << "ParameterServer: Exception was caught"  << std::endl;
	}
	return 0;
    }

};//namespace

#endif // PARAMETERSERVER_H
