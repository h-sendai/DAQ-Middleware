#ifndef PARAMETERCLIENT_H
#define PARAMETERCLIENT_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include "string.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include "Sock.h"

using namespace std;

namespace DAQMW {

  class ParameterClient {

  public:
    ParameterClient(std::string host, int port, string delimitor)
      : m_host(host), m_port(port), m_delimitor(delimitor),
      m_clientSock( m_host, m_port ) {
      cout << "ParameterClient::constructor host = " << m_host <<  "   port = " << m_port << "   delimitor = " << m_delimitor << endl;
      try {
	m_clientSock.connect(m_host, m_port);
      } catch ( SockException& e ) {
	std::cerr << "ParameterClient::constructor Exception was caught:" 
		  << e.what();
      } catch (...) {
	cerr << "ParameterClient::constructor connect error" << endl;
      }
    };
  ParameterClient(std::string host, int port)
    : m_host(host), m_port(port) {
    //    m_clientSock( m_host, m_port ) {
      m_delimitor = ":";
      cout << "ParameterClient::constructor host = " << m_host <<  "   port = " << m_port << "   delimitor = " << m_delimitor << endl;
      try {
	m_clientSock.connect(m_host, m_port);
      } catch ( SockException& e ) {
	std::cerr << "ParameterClient::constructor Exception was caught:" 
		  << e.what();
      } catch (...) {
	cerr << "connect error" << endl;
      }
    };
  ParameterClient() {
    cout << "no parameter" << endl;
  }

  ~ParameterClient() {}

  int put(std::string id, std::string value, std::string& result) {
    std::string status;

    std::string msg = "put"+ m_delimitor + id + m_delimitor + value;

    int size = msg.length();
    int length = sizeof(int)+size;
    int storeLength = length%sizeof(int);
    if(storeLength)
      storeLength = length/sizeof(int) + 1;
    else
      storeLength = length/sizeof(int);
    storeLength++; // for terminator
    unsigned int* buf = new unsigned int[storeLength];
    buf[0] = size;
    memcpy(&buf[1], msg.c_str(), size);
    std::cerr << "ParameterClient:put send lenghth = " << buf[0] << std::endl;
    m_clientSock.sendAll(buf, length);

    m_clientSock.recvAll((unsigned int*)buf, sizeof(int));
    int size2 = buf[0];
    std::cerr << "ParameterClient:put receive lenghth = " << buf[0] << std::endl;
    char *buf2 = (char *)malloc(size2+1);
    buf2[size2] = 0; // terminator
    m_clientSock.recvAll((unsigned int*)buf2, size2);
    status = buf2;
    std::cerr << "status = " << status << endl;
    delete buf2;
  
    if(status.length() > 0) {
      result = status; // success
      return 1;
    }
    return 0;
  }
  
  int put2(std::string id, std::string value, std::string& result) {
    return put(id, value, result);
  }

  int get(std::string id, std::string& value) {
    std::string status, valuetmp;

    std::string msg = "get" + m_delimitor + id;

    int size = msg.length();
    int length = sizeof(int)+size;
    int storeLength = length%sizeof(int);
    if(storeLength)
      storeLength = length/sizeof(int) + 1;
    else
      storeLength = length/sizeof(int);
    storeLength++; // for terminator
    unsigned int* buf = new unsigned int[storeLength];
    buf[0] = size;
    memcpy(&buf[1], msg.c_str(), size);
    std::cerr << "ParameterClient:put send lenghth = " << buf[0] << std::endl;
    m_clientSock.sendAll(buf, length);

    m_clientSock.recvAll((unsigned int*)buf, sizeof(int));
    int size2 = buf[0];
    char *buf2 = (char *)malloc(size2+1);
    buf2[size2] = 0; // terminator
    m_clientSock.recvAll((unsigned int*)buf2, size2);
    status = buf2;
    std::cerr << "status = " << status << endl;
    delete buf2;
  
    if(status.length() > 0) { 
      value = status; // success
      return 1;
    }
    return 0;
  }

  string* new_strp() {
    return (new string);
  }
  string strp_value(string *self) {
    return *self;
  }

  private:
  std::string m_host;
  int m_port;
  string m_delimitor;
  DAQMW::Sock m_clientSock;
  
  };
};

#endif
