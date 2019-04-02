// -*- C++ -*-
/*!
 * @file ParameterClient.h
 * @brief ParameterClient class
 * The ParameterClient class was designed corporated with ParamerServer
 * class for "shared parameters" over network.
 * Those classes are simplely used. The parameters in ParameterServer can
 * be read and written from/to the program used ParameterClient.
 * @date 1-January-2009
 * @author Yoshiji Yasu (yoshiji.yasu@kek.jp)
 *
 * Copyright (C) 2009-2011
 *     Yoshiji Yasu
 *     High Energy Accelerator Research Organization (KEK), Japan
 *     All rights reserved.
 *
 */

#ifndef PARAMETERCLIENT_H
#define PARAMETERCLIENT_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include "string.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <vector>
#include "Sock.h"

using namespace std;

namespace DAQMW {

  class ParameterClient {

  public:
    ParameterClient(std::string host, int port, string delimitor)
      : m_host(host), m_port(port), m_delimitor(delimitor),
      m_clientSock( m_host, m_port ) {
      // cout << "ParameterClient(string,int,string)::constructor host = " << m_host <<  "   port = " << m_port << "   delimitor = " << m_delimitor << endl;
      try {
	m_clientSock.connect(m_host, m_port);
      } catch ( SockException& e ) {
	std::cerr << "ParameterClient(string,int,string)::constructor Exception was caught:" 
		  << e.what();
      } catch (...) {
	cerr << "ParameterClient(string,int,string)::constructor connect error" << endl;
      }
    };
    ParameterClient(std::string host, int port)
      : m_host(host), m_port(port) {
      m_delimitor = ":";
      // cout << "ParameterClient(string,int)::constructor host = " << m_host <<  "   port = " << m_port << "   delimitor = " << m_delimitor << endl;
      try {
	m_clientSock.connect(m_host, m_port);
      } catch ( SockException& e ) {
	std::cerr << "ParameterClient(string,int)::constructor Sock Exception was caught at connection..." 
		  << e.what();
      } catch (...) {
	std::cerr << "ParameterClient(string,int)::constructor Exception was caught at connection" << endl;
      }
      char *timeout_p = getenv("PARAMETER_CLIENT_TIMEOUT");
      float timeout;
      if (timeout_p == NULL) {
        timeout = 20;
      }
      else {
        timeout = strtol(timeout_p, NULL, 0);
      }
      m_clientSock.setOptRecvTimeOut(timeout);
    };
    ParameterClient() {
      // cout << "ParameterClient()::constructor host = " << m_host <<  "   port = " << m_port << "   delimitor = " << m_delimitor << endl;
    }

    ~ParameterClient() {
      for(int i=0; i< m_stringP.size(); i++) {
        delete m_stringP[i];
      }
    }

    int put(std::string subId, std::string value, std::string& result) {
      return getOrPut("put", subId, value, result);
    }

    int get(std::string subId, std::string& result) {
      std::string value;
      return getOrPut("get", subId, value, result);
    }

    string* new_strp() {
      m_string = new string;
      m_stringP.push_back(m_string);
      return m_string;
    }
    string strp_value(string *self) {
      return *self;
    }

  private:
    int getOrPut(std::string com, std::string subId, 
		 std::string value, std::string& result) {
      std::string status;
      std::string msg;
      if (com == "put")
	msg = com + m_delimitor + subId + m_delimitor + value;
      else
	msg = com + m_delimitor + subId;
      int size = msg.length();
      int length = sizeof(int)+size;
      int st;
      int storeLength = length%sizeof(int);
      if(storeLength)
	storeLength = length/sizeof(int) + 1;
      else
	storeLength = length/sizeof(int);
      storeLength++; // for terminator
      unsigned int* buf = new unsigned int[storeLength];
      try {
	buf[0] = size;
	memcpy(&buf[1], msg.c_str(), size);
	// std::cerr << "ParameterClient::"+com+"() send lenghth = " << buf[0] << std::endl;
	st = m_clientSock.sendAll(buf, length);
	if (st == Sock::ERROR_TIMEOUT) {
	  std::cerr << "ParameterClient::"+com+"() sendAll Timeout..." << std::endl;
	  delete [] buf;
	  return -1;
	}
	// std::cerr << "ParameterClient::"+com+"(): recvAll now calling for length... "<< std::endl;
	st = m_clientSock.recvAll((unsigned int*)buf, sizeof(int));
	if (st == Sock::ERROR_TIMEOUT) {
	  std::cerr << "ParameterClient::"+com+"() recvAll Timeout for getting size..." << std::endl;
	  delete [] buf;
	  return -1;
	}
      } catch (...) {
        delete [] buf;
        return -1;
      }

      int size2 = buf[0];
      delete [] buf;
      // std::cerr << "ParameterClient::"+com+"() receive lenghth = " << size2 << std::endl;
      char *buf2 = (char *)malloc(size2+1);
      buf2[size2] = 0; // terminator
      try {
	// std::cerr << "ParameterClient::"+com+"(): recvAll now calling for message... "<< std::endl;
	st = m_clientSock.recvAll((unsigned int*)buf2, size2);
	if (st == Sock::ERROR_TIMEOUT) {
	  std::cerr << "ParameterClient::"+com+"() recvAll Timeout for getting message..." << std::endl;
	  delete [] buf2;
	  return -1;
	}
      } catch (...) {
        delete [] buf2;
        return -1;
      }
      status = buf2;
      // std::cerr << "ParameterClient::"+com+"() received message: ";
      // std::cerr << status << endl;
      delete [] buf2;
      result = status;
      return 0;
    };
    std::string m_host;
    int m_port;
    string m_delimitor;
    DAQMW::Sock m_clientSock;
    string* m_string;
    vector<string* > m_stringP;
    };
};

#endif
