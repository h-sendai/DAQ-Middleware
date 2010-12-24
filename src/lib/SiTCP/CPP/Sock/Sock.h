// -*- C++ -*-
/*!
 * @file Sock.h
 * @brief Definition of the Sock class
 * @date
 * @author Yoshiji Yasu
 *
 * Copyright (C) 2008
 *     Yoshiji Yasu
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 * Modified 5-Nov-2008 by Y.Y
 *   
 */

#ifndef SOCK_H
#define SOCK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>

namespace DAQMW {

  class SockException : public std::exception {
  public:
    SockException(const std::string& msg);
    virtual ~SockException() throw();
    const char* what();
    int reason();

  private:
    int m_reason;
    std::string m_msg;
  };

  class Sock {
  public:
    // network address structure wil be initialzed as zero.
    // socket is NOT created. connection timeout time and read/write timeout
    // time will be initialized.
    Sock();
    Sock(const std::string host, const int port );
    virtual ~Sock();
    
    /// Procedure to make server program...
    //  try {
    //    Sock sock = new Sock();
    //    sock->create():
    //    sock->bind(port);
    //    sock->listen();
    //    Sock newSock = new Sock();
    //    sock->accept(newSock);
    //       |
    //  } catch (SockException& e) {
    //    std::cerr << "Sock Fatal Error : " << e.what() << std::endl;
    //  } catch (...) {
    //    std::cerr << "Sock Fatal Error : Unknown" << std::endl;
    //  }

    // For Server
    // creates a socket and then sets an option of SO_REUSEADDR. 
    // if the return value is SUCCESS, success. 
    // Otherwise, Fatal errors. Those will be thrown.
    int create();

    // For Server
    // binds the socket. 
    // if the return value is SUCCESS, success.
    // Otherwise, Fatal errors. Those will be thrown.
    // bind(const int port) binds any clients while bind(const int port,
    // const char* ipAddress) binds a specific client.
    int bind ( const int port );
    int bind ( const int port, const char* ipAddress );

    // For Server
    // listen the socket. MAXCONNECTIONS connections will be open.
    // if the return value is SUCCESS, success.
    // Otherwise, Fatal errors. Those will be thrown.
    int listen() const;

    // For Server
    // accepts the socket.
    // if the return value is SUCCESS, success.
    // Otherwise, Fatal errors. Those will be thrown.
    // New socket should be specified.
    int accept ( Sock& ) const;
    
    /// Procedure to make client program...
    //  There are two types to make client program.
    //  One throws the exception of Fatal error while
    //  Another does not throws the exception, but returns ERROR_FATAL.
    //  1)
    //  try {
    //    Sock sock = new Sock();
    //    sock->connect(ipAddress, port);
    //    string command = "start";
    //    sock->send(command);
    //  } catch (SockException& e) {
    //    std::cerr << "Sock Fatal Error : " << e.what() << std::endl;
    //  } catch (...) {
    //    std::cerr << "Sock Fatal Error : Unknown" << std::endl;
    //  }
    //  2)
    //  unsigned char buffer[100];
    //  Sock sock = new Sock(ipAdrress, port);
    //  int status = sock->connectTCP();
    //  if(status==ERROR_FATAL)
    //    std::cerr << "fatal error..." << std::endl;
    //  else if(status==ERROR_TIMEOUT)
    //    std::cerr << "Timeout.. retry..." << std::endl;
    //  status = sock->writeAll(buffer, 100);
    //  if(status==ERROR_FATAL)
    //    std::cerr << "fatal error..." << std::endl;
    //  else if(status==ERROR_TIMEOUT)
    //    std::cerr << "Timeout.. retry..." << std::endl;

    // For client
    // The method creates a socket if not created.
    // It creates network address structure and then connect.
    // If fatal error occurred, those error will be thrown.
    // If Timeout occcurred, ERROR_TIMEOUT will return.
    // Otherwise, SUCCESS will return.
    int connect ( const std::string host, const int port );

    // For client
    // connectTCP method is for TCP connection. 
    // connectUDP method is for UDP connection.
    // It creates if socket is not created.
    // It creates network address structure and then connect.
    // It sets REUSE sockopt. If fatal error occurred, ERROR_FATAL will return.
    // If Timeout occcurred, ERROR_TIMEOUT will return.
    // Otherwise, SUCCESS will return.
    int connectTCP (void);
    int connectUDP (void);

    // For client
    // The method closes the socket.
    void disconnect (void);

    /// Data Transimission

    // Exception is NOT thrown.
    // The method will send data with the specified size in bytes. But, 
    // it may not send the specified size of data. When fatal error occurred,
    // ERROR_FATAL will return. If timeout occurred, ERROR_TIMEOUT will return.
    // Otherwise, the method returns size of the sent data in bytes.  
    int write(unsigned char* buffer, int nbytes) const;

    // Exception is NOT thrown.
    // The method will receive data with the specified size in bytes. But, 
    // it may not receive the specified size of data. When fatal error
    // occurred, ERROR_FATAL will return. If timeout occurred, 
    // ERROR_TIMEOUT will return. Otherwise, the method returns 
    // size of the received data in bytes.  
    int read(unsigned char* buffer, int nbytes) const;

    // Exception is NOT thrown.
    // The method will send all of data with the specified size in bytes. 
    // When fatal error occurred ERROR_FATAL will return. 
    // If timeout occurred, ERROR_TIMEOUT will return.
    // Otherwise, the method returns size of the sent data in bytes.  
    int writeAll(unsigned char* buffer, int nbytes) const;

    // Exception is NOT thrown.
    // The method will receive all of data with the specified size in bytes. 
    // When fatal error occurred ERROR_FATAL will return. 
    // If timeout occurred, ERROR_TIMEOUT will return.
    // Otherwise, the method returns size of the received data in bytes.  
    int readAll(unsigned char* buffer, int nbytes) const;

    // Exception IS thrown.
    // The method will send a string. But, it may not send all of characters
    // in the string. When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return.
    // Otherwise, the method returns size of the sent data in bytes.
    int send ( const std::string ) const;

    // Exception IS thrown.
    // The method will receive a string.
    // But, it may not receive all of characters in the string.
    // When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return.
    // Otherwise, the method returns size of the received data in bytes.
    int recv ( std::string& ) const;

    // Exception IS thrown.
    // The method will send data with the specified size in bytes.
    // But, it may not send the specified size of data.
    // When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return. 
    // Otherwise, the method returns size of the sent data in bytes.  
    int send ( const unsigned int* , int ) const;
    
    // The method will receive data with the specified size in bytes.
    // But, it may not receive the specified size of data.
    // When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return. 
    // Otherwise, the method returns size of the received data in bytes.  
    int recv ( unsigned int* , int ) const;

    // Exception IS thrown.
    // The method will send all of characters in the specified string. 
    // When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return.
    // Otherwise, the method returns size of the sent data in bytes.
    int sendAll ( const std::string ) const;
    
    // Exception IS thrown.
    // The method will send all of data with the specified size in bytes.
    // When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return. 
    // Otherwise, the method returns size of the sent data in bytes.  
    int sendAll ( const unsigned int* , int ) const;
    
    // Exception IS thrown.
    // The method will receive the specified size of characters in the string. 
    // When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return.
    // Otherwise, the method returns number of the received characters.
    int recvAll ( std::string& , int& ) const;

    // Exception IS thrown.
    // The method will receive all of data with the specified size in bytes.
    // When fatal error occurred, an exception will be thrown.
    // If timeout occurred, ERROR_TIMEOUT will return. 
    // Otherwise, the method returns size of the received data in bytes.  
    int recvAll ( unsigned int* , int ) const;

    // The method sets or resets Non-Blocking mode.  If bool is true, it sets.
    // if SUCCESS returns, success. Oterwise, fatal error will be thrown.
    int setOptNonBlocking ( const bool ) const;
    
    // The method sets or resets ReUse flag.If bool is true, it sets.
    // if SUCCESS returns, success. Oterwise, fatal error will be thrown.
    int setOptReUse( const bool ) const;

    // The method sets or resets Nagle algorithm. If bool is true, it sets.
    // if SUCCESS returns, success. Oterwise, fatal error will be thrown.
    int setOptNoDelay( const bool) const;

    // The method sets or resets receive/read timeout. 
    // If the specified value is 0, it resets the timeout. 
    // Otherwise, it sets the timeout value in seconds.
   // if SUCCESS returns, success. Oterwise, fatal error will be thrown.
    int setOptRecvTimeOut(float);

    // The method sets or resets send/write timeout. 
    // If the specified value is 0, it resets the timeout. 
    // Otherwise, it sets the timeout value in seconds.
   // if SUCCESS returns, success. Oterwise, fatal error will be thrown.
    int setOptSendTimeOut(float);

    // The method sets size of receive buffer in bytes.
    // if SUCCESS returns, success. Oterwise, fatal error will be thrown.
    int setOptRecvBuf(int) const;

    // The method sets size of send buffer in bytes.
    // if SUCCESS returns, success. Oterwise, fatal error will be thrown.
    int setOptSendBuf(int) const;

    // The method returns socket file descriptor.
    int getSockFd() const;

    static const int SUCCESS            =  0;
    static const int ERROR_FATAL        = -1;
    static const int ERROR_TIMEOUT      = -2;
    static const int ERROR_ILLPARM      = -3;

  private:
    int connect(int type);
    int float2timeval(float time, timeval* tv) const;
    bool is_valid() const { return m_sock != -1;};
    static void connectAlarm(int);
    int setAlarmTimer();
    void setConnectTimer(float);

    std::string m_ipAddress;
    int m_port;
    int m_sock;
    float m_timeout, m_connectTimeout;
    sockaddr_in m_addr;
    bool m_debug;

    static const int TCP = 1;
    static const int UDP = 2;
    static const int MAXHOSTNAME = 200;
    static const int MAXCONNECTIONS = 5;
    static const int MAXRECV = 8000;
    
  };
};

#endif // SOCK_H
