// -*- C++ -*-
/*!
 * @file Sock.h
 * @brief Definition of the Sock class
 * @date 25-May-2011
 * @author Yoshiji Yasu (yoshiji.yasu@kek.jp)
 * @version 1.0
 *
 * Copyright (C) 2008-2011
 *     Yoshiji Yasu
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

#ifndef SOCK_H
#define SOCK_H

#include <iostream>
#include <string>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*!
 * @namespace DAQMW
 * @brief common namespace of DAQ-Middleware
 */
namespace DAQMW {

  /*!
   * @class SockException
   * @brief SockException class
   * 
   * This class supports Sock exception.
   *
   */
  class SockException : public std::exception {
  public:
    SockException(const std::string& msg);
    virtual ~SockException() throw();
    const char* what();
    //    int reason();

  private:
    //    int m_reason;
    std::string m_msg;
  };

  /*!
   * @class Sock
   * @brief Sock class
   * 
   * This class supports basic sock functions.
   *
   */
  class Sock {
  public:
    /* 
     * @brief constructor
     * network address structure wil be initialzed as zero.
     * socket is NOT created. connection timeout time and read/write timeout
     * time will be initialized.
     * @parameter host IP address
     * @parameter port port number
     */
    Sock();
    Sock(const std::string host, const int port );
    /*
     * @brief deconstructor
     */
    virtual ~Sock();
    
    /*
     * @brief socket creation for Server
     * creates a socket and then sets an option of SO_REUSEADDR. 
     * if the return value is SUCCESS, success. 
     * Otherwise, Fatal errors. Those will be thrown.
     */
    int create();
    int createTCP(void);
    int createUDP(void);

    /*
     * @brief binding socket for Server
     * binds the socket. 
     * if the return value is SUCCESS, success.
     * Otherwise, Fatal errors. Those will be thrown.
     * bind(const int port) binds any clients while bind(const int port,
     * const char* ipAddress) binds a specific client.
     * @parameter port port number
     * @parameter ipAddress IP address
     */
    int bind ( const int port );
    int bind ( const int port, const char* ipAddress );

    /*
     * @brief listening socket for Server
     * listen the socket. MAXCONNECTIONS connections will be open.
     * if the return value is SUCCESS, success.
     * Otherwise, Fatal errors. Those will be thrown.
     */
    int listen() const;

    /*
     * @brief accepting socket for Server
     * accepts the socket.
     * if the return value is SUCCESS, success.
     * Otherwise, Fatal errors. Those will be thrown.
     * New socket should be specified.
     */
    int accept ( Sock& ) const;
    
    /*
     * @brief connecting socket for client
     * connect method is for TCP connection.
     * connectTCP method is for TCP connection. 
     * connectUDP method is for UDP connection.
     * It creates if socket is not created.
     * It creates network address structure and then connect.
     * It sets REUSE sockopt. If fatal error occurred, ERROR_FATAL will return.
     * If Timeout occcurred, ERROR_TIMEOUT will return.
     * Otherwise, SUCCESS will return.
     * @parameter host IP address
     * @parameter port port number
     */
    int connect ( const std::string host, const int port );
    int connectTCP (void);
    int connectUDP (void);

    /*
     * @brief closing socket
     * The method closes the socket.
     */
    void disconnect (void);

    /*
     * @brief write data
     * The method sends data.
     * Exception is NOT thrown.
     * The method will send data with the specified size in bytes. But, 
     * it may not send the specified size of data. When fatal error occurred,
     * ERROR_FATAL will return. If timeout occurred, ERROR_TIMEOUT will return.
     * Otherwise, the method returns size of the sent data in bytes.  
     */
    int write(unsigned char* buffer, int nbytes) const;

    /*
     * @brief read data
     * The method receives data.
     * Exception is NOT thrown.
     * The method will receive data with the specified size in bytes. But, 
     * it may not receive the specified size of data. When fatal error
     * occurred, ERROR_FATAL will return. If timeout occurred, 
     * ERROR_TIMEOUT will return. Otherwise, the method returns 
     * size of the received data in bytes.  
     */
    int read(unsigned char* buffer, int nbytes) const;

    /*
     * @brief write all of data
     * The method sends all of data.
     * Exception is NOT thrown.
     * The method will send all of data with the specified size in bytes. 
     * When fatal error occurred ERROR_FATAL will return. 
     * If timeout occurred, ERROR_TIMEOUT will return.
     * Otherwise, the method returns size of the sent data in bytes.  
     */
    int writeAll(unsigned char* buffer, int nbytes) const;

    /*
     * @brief read all of data
     * The method receives all of data.
     * Exception is NOT thrown.
     * The method will receive all of data with the specified size in bytes. 
     * When fatal error occurred ERROR_FATAL will return. 
     * If timeout occurred, ERROR_TIMEOUT will return.
     * If SUCCESS returns, success.
     */
    int readAll(unsigned char* buffer, int nbytes) const;

    /*
     * @brief send data
     * The method sends data.
     * Exception IS thrown.
     * The method will send a string. But, it may not send all of characters
     * in the string. When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return.
     * Otherwise, the method returns size of the sent data in bytes.
     */
    int send ( const std::string ) const;

    /*
     * @brief receive data
     * The method sends data.
     * Exception IS thrown.
     * The method will receive a string.
     * But, it may not receive all of characters in the string.
     * When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return.
     * Otherwise, the method returns size of the received data in bytes.
     */
    int recv ( std::string& ) const;

    /*
     * @brief send data
     * The method sends data.
     * Exception IS thrown.
     * The method will send data with the specified size in bytes.
     * But, it may not send the specified size of data.
     * When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return. 
     * Otherwise, the method returns size of the sent data in bytes.  
     */
    int send ( const unsigned int* , int ) const;
    
    /*
     * @brief receive data
     * The method receives data.
     * The method will receive data with the specified size in bytes.
     * But, it may not receive the specified size of data.
     * When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return. 
     * Otherwise, the method returns size of the received data in bytes.  
     */
    int recv ( unsigned int* , int ) const;

    /*
     * @brief send all of data
     * The method sends data.
     * Exception IS thrown.
     * The method will send all of characters in the specified string. 
     * When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return.
     * If SUCCESS returns, success.
     */
    int sendAll ( const std::string ) const;
    
    /*
     * @brief send all of data
     * The method sends all of data.
     * Exception IS thrown.
     * The method will send all of data with the specified size in bytes.
     * When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return. 
     * If SUCCESS returns, success.
     */
    int sendAll ( const unsigned int* , int ) const;
    
    /*
     * @brief receive all of data
     * The method receives all of data.
     * Exception IS thrown.
     * The method will receive the specified size of characters in the string. 
     * When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return.
     * Otherwise, the method returns number of the received characters.
     */
    int recvAll ( std::string& , int& ) const;

    /*
     * @brief receive all of data
     * The method receives all of data.
     * Exception IS thrown.
     * The method will receive all of data with the specified size in bytes.
     * When fatal error occurred, an exception will be thrown.
     * If timeout occurred, ERROR_TIMEOUT will return. 
     * Otherwise, the method returns size of the received data in bytes.  
     */
    int recvAll ( unsigned int* , int ) const;

    /*
     * @brief write data with UDP
     * The method sends data with UDP.
     */
    int writeTo(const unsigned char* buffer, int nbytes);

    /*
     * @brief read data with UDP
     * The method receives data with UDP.
     */
    int readFrom(unsigned char* buffer, int nbytes);

    /*
     * @brief set/reset in Non-Blocking mode
     * The method sets or resets Non-Blocking mode.  If bool is true, it sets.
     * if SUCCESS returns, success. Oterwise, fatal error will be thrown.
     */
    int setOptNonBlocking ( const bool ) const;
    
    /*
     * @brief set/reset ReUse flag
     * The method sets or resets ReUse flag.If bool is true, it sets.
     * if SUCCESS returns, success. Oterwise, fatal error will be thrown.
     */
    int setOptReUse( const bool ) const;

    /*
     * @brief set/reset Nagle algorithm
     * The method sets or resets Nagle algorithm. If bool is true, it sets.
     * if SUCCESS returns, success. Oterwise, fatal error will be thrown.
     */
    int setOptNoDelay( const bool) const;

    /*
     * @brief set/reset receive/read timeout
     * The method sets or resets receive/read timeout. 
     * If the specified value is 0, it resets the timeout. 
     * Otherwise, it sets the timeout value in seconds.
     * if SUCCESS returns, success. Oterwise, fatal error will be thrown.
     */
    int setOptRecvTimeOut(float);

    /*
     * @brief set/reset send/write timeout
     * The method sets or resets send/write timeout. 
     * If the specified value is 0, it resets the timeout. 
     * Otherwise, it sets the timeout value in seconds.
     * if SUCCESS returns, success. Oterwise, fatal error will be thrown.
     */
    int setOptSendTimeOut(float);

    /*
     * @brief set size of receive buffer
     * The method sets size of receive buffer in bytes.
     * if SUCCESS returns, success. Oterwise, fatal error will be thrown.
     */
    int setOptRecvBuf(int) const;

    /*
     * @brief set size of send buffer
     * The method sets size of send buffer in bytes.
     * if SUCCESS returns, success. Oterwise, fatal error will be thrown.
     */
    int setOptSendBuf(int) const;

    /*
     * @brief get socket file descriptor
     * The method returns socket file descriptor.
     */
    int getSockFd() const;

    /*
     * @brief read number of bytes in kernel buffer
     * The method returns number of bytes that can be read in kernel buffer.
     */
    int readNum(int* msgSize) const;

    static const int SUCCESS            =  0;
    static const int ERROR_FATAL        = -1;
    static const int ERROR_TIMEOUT      = -2;
    static const int ERROR_ILLPARM      = -3;
    static const int ERROR_NOTSAMESIZE  = -4;

  private:
    int connect(int type);
    int float2timeval(float time, timeval* tv) const;
    bool isValidSock() const { return m_sock != -1;};
    static void connectAlarm(int);
    int setAlarmTimer();
    void setConnectTimer(float);

    std::string m_ipAddress;
    int m_port;
    int m_sock;
    float m_timeout, m_connectTimeout;
    struct sockaddr_in m_addr, m_addr_other;
    socklen_t m_slen, m_rlen;
    bool m_debug;

    static const int TCP = 1;
    static const int UDP = 2;
    static const int MAXHOSTNAME = 200;
    static const int MAXCONNECTIONS = 1;
    static const int MAXRECV = 8000;
    
  };
};

#endif // SOCK_H
