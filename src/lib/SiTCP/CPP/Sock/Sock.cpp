/*!
 * @file Sock.cpp
 * @brief Implementation of the Sock class.
 * @date
 * @author Yoshiji Yasu
 *
 * Copyright (C) 2008
 *     Yoshiji Yasu
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 * Modified 6-Nov-2008 by Y.Y
 *
 */

#include <iostream>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "Sock.h"

namespace DAQMW {

  /// Implementation of SockException class
  SockException::SockException(const std::string& msg) : m_msg(msg) {
    m_reason = 0;
  }

  SockException::~SockException() throw() {}

  const char* SockException::what() {
    return m_msg.c_str();
  }

  int SockException::reason() {
    return m_reason;
  }

  Sock::Sock()
    : m_sock ( -1 ), m_timeout(2.0), m_connectTimeout(2.0), m_debug(false) {
    memset ( &m_addr, 0, sizeof ( m_addr ) );
  }

  Sock::Sock(const std::string host, const int port ) 
    : m_sock ( -1 ), m_timeout(2.0), m_connectTimeout(2.0), m_debug(false) {
    m_ipAddress = host;
    m_port = port;
    memset ( &m_addr, 0, sizeof ( m_addr ) );
    if(m_debug) {
      std::cerr << "ipaddress = "<<  m_ipAddress 
		<< "  port = " << m_port << std::endl;
    }
  }

  Sock::~Sock() {
    if ( Sock::is_valid() )
      ::close ( m_sock );
  }

  // For Server
  int Sock::create() {
    m_sock = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( ! is_valid() ) {
      perror("### ERROR: Sock::create():socket");
      throw SockException("Sock::create error");
    }
    if(m_debug)
      std::cerr << "Sock::create() done\n";
    setOptReUse(true); // if error, error will be thrown.
    if(m_debug)
      std::cerr << "Sock::setsockopt() REUSE done\n";
    return SUCCESS;
  }

  int Sock::bind ( const int port ) {
    int status;
    if ( ! is_valid() ){
      perror("### ERROR: Sock::bind(const int):invalid socket");
      throw SockException("Sock::bind invalid socket");
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons ( port );
    status = ::bind ( m_sock,
			   ( struct sockaddr * ) &m_addr,
			   sizeof ( m_addr ) );
    if ( status == -1 ) {
      perror("### ERROR: Sock::bind(int):bind");
      throw SockException("Sock::bind error");
    } else {
      if(m_debug)
	std::cerr << "Sock::bind(int) done\n";
      return SUCCESS;
    }
  }

  int Sock::bind ( const int port, const char* ipAddress ) {
    int status;
    if ( ! is_valid() ){
      perror("### ERROR: Sock::bind(int, char*):invalid socket");
      throw SockException("Sock::bind invalid socket");
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ipAddress);
    m_addr.sin_port = htons ( port );
    status = ::bind ( m_sock,
			   ( struct sockaddr * ) &m_addr,
			   sizeof ( m_addr ) );
    if ( status == -1 ) {
      perror("### ERROR: Sock::bind(int, char*):bind");
      throw SockException("Sock::bind error");
    } else {
      if(m_debug)
	std::cerr << "Sock::bind(int, char*) done\n";
      return SUCCESS;
    }
  }

    int Sock::listen() const {
      int status = ::listen ( m_sock, MAXCONNECTIONS );
      if ( status == -1 ) {
	perror("### ERROR: Sock::listen():listen");
	throw SockException("Sock::listen error");
      } else {
	if(m_debug)
	  std::cerr << "Sock::listen() done\n";
	return SUCCESS;
      }
    }

  int Sock::accept ( Sock& new_socket ) const {
    int addr_length = sizeof ( m_addr );
    new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr,
				   ( socklen_t * )&addr_length );
    if ( new_socket.m_sock < 0 ) { 
      perror("### ERROR: Sock::accept(Sock&):accept");
      throw SockException("Sock::accept error");
    }
    else {
      if(m_debug)
	std::cerr << "Sock::accept(Sock&) done\n";
      return SUCCESS;
    }
  }

  void Sock::setConnectTimer(float time) {
    m_connectTimeout = time;
  }

  void Sock::connectAlarm(int signo) {
    return;
  }

  int Sock::setAlarmTimer() {
    unsigned int time = static_cast<int>(m_connectTimeout);
    struct sigaction act, oact;
    act.sa_handler = &Sock::connectAlarm;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    if(sigaction(SIGALRM, &act, &oact) < 0) {
      return ERROR_FATAL;
    }
    if(alarm(time) != 0)
       std::cerr << "connect(string, int): alarm was already set\n";
    return SUCCESS;
  }

  int  Sock::connect ( const std::string host, const int port ) {
    //int time = static_cast<int>(m_connectTimeout);
    
    if ( ! is_valid() ) {
      m_sock = socket ( AF_INET, SOCK_STREAM, 0 );
      //      std::cerr << "Sock::connect: socket was created" << std::endl;
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons ( port );
    
    int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );
    if (status < 0) {
      perror("### ERROR: Sock::connect(string, int) inet_pton");
      throw SockException("Sock::connect(string, int) inet_pton error");
    } else if(status == 0) { // specified by hostname not ip
      struct hostent *hostinfo = gethostbyname(host.c_str());
      if(hostinfo != NULL) {
	m_addr.sin_addr.s_addr = *(unsigned int*)hostinfo->h_addr_list[0];
      } else {
	std:: cerr << "### ERROR: Sock::connect(string, int) gethostbyname" << std::endl;
	throw SockException("Sock::connect(string, int) gethostbyname error");
      }
    }
    setAlarmTimer();
    status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
    alarm(0);
    if(status < 0) {
      if(errno== EINTR)
	return ERROR_TIMEOUT;
      perror("### ERROR: Sock::connect(string, int) connect");
      throw SockException("Sock::connect(string, int) connect error");
    }
    return SUCCESS;
  }

  int Sock::connect(int type) {
    //int time = static_cast<int>(m_connectTimeout);
    struct timeval tv;
    int socketType;
    int status;
    int on;
    
    switch(type) {
    case TCP:
      if(m_debug)
	std::cerr << "Sock::connect(int): TCP was selected" << std::endl;
      socketType = SOCK_STREAM;
      break;
    case UDP:
      if(m_debug)
	std::cerr << "Sock::connect(int): UDP was selected" << std::endl;
      socketType = SOCK_DGRAM;
      break;
    default:
      std::cerr << "Sock::connect(int): invalide type" << std::endl;
      throw SockException("### Sock::connect(int) invalid type");
    }
    if(m_sock==-1) // if socket is not created, create now.
      m_sock = socket ( AF_INET, socketType, 0 );
    if(m_debug) {
      std::cerr << "Sock::connect(int): socket was created" << std::endl;
    }
    on = 1; // Reuse is active
    if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, 
		      ( const char* ) &on, sizeof ( on ) ) == -1 ) {
      perror("### ERROR: Sock::connect(int):setsockopt:ReUseAddr");
      return ERROR_FATAL;
    }
    if ((status = float2timeval(m_timeout, &tv)) < 0) {
      std::cerr << "### fail conversion from timeout values to timeval structure\n";
      return ERROR_FATAL;
    }

    // Receive(recv/read) timeout is set.
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_RCVTIMEO, 
				&tv, sizeof(tv))) < 0) {
      perror("### ERROR: Sock::connect(int):setsockopt:ReceiveTimeout");
      return ERROR_FATAL;
    }
    // Send(send/write) timeout is set.
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_SNDTIMEO, 
				&tv, sizeof(tv))) < 0) {
      perror("### ERROR: Sock::connect(int):setsockopt:SendTimeout");
      return ERROR_FATAL;
    }

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons ( m_port );
    status = inet_pton ( AF_INET, m_ipAddress.c_str(), &m_addr.sin_addr );
    if(status <= 0) {
      perror("### ERROR: Sock::connect(int):inet_pton");
      return ERROR_FATAL;
    }
    if(m_debug) {
      std::cerr << "Sock::connect(int): connecting now..." << std::endl;
    }
    setAlarmTimer(); // set connection timeout.
    status = ::connect ( m_sock, ( const sockaddr * ) &m_addr,
			     (socklen_t)sizeof ( m_addr ) );
    alarm(0);
    if(status < 0) {
      if(errno==EINTR) {
	perror("### ERROR: Sock::connect(int):connect:Time out");
	return ERROR_TIMEOUT;
      }
      perror("### ERROR: Sock::connect(int):connect:Fatal error");
      return ERROR_FATAL;
    }
    if(m_debug)
      std::cerr << "Sock::connect(int): connected..." << std::endl;
    return SUCCESS;
  }

  int Sock::connectTCP(void) {
    if(m_debug) {
      std::cerr << "Sock::connectTCP:enter" << std::endl;
    }
    return Sock::connect(TCP);
  }

  int Sock::connectUDP(void) {
    if(m_debug) {
      std::cerr << "Sock::connectUDP:enter" << std::endl;
    }
    return Sock::connect(UDP);
  }

  // MSG_NOSIGNAL is necessary. SIGPIPE is generated when the link is off.
  // As the result, the program will terminate. The flag avoids the
  // termination.
  int Sock::send ( const std::string s ) const {
    int status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );
    if ( status == -1 ) {
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      }
      perror("### ERROR: Sock::send(const string):send");
      throw SockException("Sock::send(const string) error");
    } else {
      return status;
    }
  }

  int Sock::send ( const unsigned int* s, int size ) const {
    int status = ::send ( m_sock, s, size, MSG_NOSIGNAL );
    if ( status == -1 ) {
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      }
      perror("### ERROR: Sock::send(const unsigned int*, int):send");
      throw SockException("Sock::send(const unsigned int*, int) error");
    } else {
      return status;
    }
  }

  int Sock::sendAll ( const std::string s ) const {
    ssize_t nwritten;
    unsigned char* ptr = (unsigned char*)s.c_str();
    size_t nleft = s.size();

    while (nleft > 0) {
    again:
      if ( (nwritten = ::send ( m_sock, ptr, nleft, MSG_NOSIGNAL )) < 0) {
	if(errno == EINTR) {
	  goto again;
	} else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	  return ERROR_TIMEOUT;
	}
	perror("### ERROR: Sock::sendAll(const string):send");
	throw SockException("Sock::sendAll(const string) error");	
      }
      nleft -= nwritten;
      ptr   += nwritten;
    }
    return(s.size() - nleft);
  }

  int Sock::sendAll ( const unsigned int* s, int size ) const {
    ssize_t nwritten;
    unsigned char* ptr = (unsigned char*)s;
    size_t nleft = size;
    //    std::cerr << "Sock::sendAll(const unsigned int*, int) enter" << std::endl;
    while (nleft > 0) {
    again:
      if ( (nwritten = ::send ( m_sock, ptr, nleft, MSG_NOSIGNAL )) < 0) {
	if(errno == EINTR) {
	  goto again;
	} else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	  return ERROR_TIMEOUT;
	}
	throw SockException("Sock::sendAll(const unsigned int*, int) error");
      }
      nleft -= nwritten;
      ptr   += nwritten;
    }
    return(size - nleft);
  }

  int Sock::recv ( std::string& s ) const {
    char buf [ MAXRECV + 1 ];
    memset ( buf, 0, MAXRECV + 1 );
  again:
    int status = ::recv ( m_sock, buf, MAXRECV, 0 );
    if ( status < 0 ) { 
      if(errno ==EINTR) {
	goto again;
      } else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      }
      std::cerr << "### ERROR: status == -1   errno == " 
		<< errno << "  in Socket::recv(string&)\n";
      throw SockException("Sock::recv(string&) fatal error");	
    } else if(status == 0) {  // far end node link will be off.
      std::cerr << "### ERROR: status == 0 in Socket::recv(string&)\n";
      throw SockException("Sock::recv(string&) fatal error");	
    } else {
      s = buf;
      return status;
    }
  }

  int Sock::recv ( unsigned int* s , int size) const {
  again:
    int status = ::recv ( m_sock, (char*)s, size, 0);
    if ( status < 0 ) {
      if(errno == EINTR)
	goto again;
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      } else {
	if(m_debug)
	  std::cerr << "status == -1   errno == " 
		    << errno << "  in Socket::recv\n";
	throw SockException("Sock::recv(int*, int) error");
      }
    } else if(status == 0) {  // far end node link will be off.
      std::cerr << "### ERROR: status == 0 in Socket::recv(string&)\n";
      throw SockException("Sock::recv(string&) fatal error");	
    }
    return status;
  }

  int Sock::recvAll ( std::string& s, int& size ) const {
    char buf [ MAXRECV + 1 ];
    if(size > MAXRECV) {
      std::cerr << "### ERROR: specified size is too large in Socket::recvAll(string&)\n";
      throw SockException("Sock::recvAll(string&, int&) fatal error");	
    }
    memset ( buf, 0, MAXRECV + 1 );
  again:
    int status = ::recv ( m_sock, buf, size, MSG_WAITALL );
    if ( status < 0 ) {
      if(errno == EINTR) {
	goto again;
      } else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      }
      std::cerr << "### ERROR: status == -1   errno == " 
		<< errno << "  in Socket::recvAll(string&)\n";
      throw SockException("Sock::recvAll(string&, int&) fatal error");
    } else if(status == 0) {  // far end node link will be off.
      std::cerr << "### ERROR: status == 0 in Socket::recv(string&, int&)\n";
      throw SockException("Sock::recv(string&, int&) fatal error");	
    } else {
      s = buf;
      return status;
    }
  }

  int Sock::recvAll ( unsigned int* s , int size) const {
  again:
    int status = ::recv ( m_sock, (char*)s, size, MSG_WAITALL);
    if ( status < 0 ) {
      if(errno == EINTR) {
	goto again;
      } else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      } else {
	if(m_debug)
	  std::cerr << "status == -1   errno == " 
		    << errno << "  in Socket::recv(int*, int)\n";
	throw SockException("Sock::recv(unsigned int*, int) fatal error");
      }
    } else if(status == 0) { // far end node link will be off.
      std::cerr << "### ERROR: status == 0 in Socket::recv(unsigned int*, int)\n";
      throw SockException("Sock::recv(unsigned int*, int) fatal error");	
    }
    return status;
  }

  // This flag requests that the implementation does not send SIGPIPE on
  // error on stream oriented sockets when the other end breaks
  // connection. The EPIPE error is still returned as normal.
  int Sock::write ( unsigned char* buffer, int nbytes ) const {
  again:
    int status = ::send ( m_sock, buffer, nbytes, MSG_NOSIGNAL );
    if ( status == -1 ) {
      if(errno == EINTR) {
	goto again;
      } else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      } else
	return ERROR_FATAL;
    } else {
      return status;
    }
  }
  /*
    int n = ::write ( m_sock, buffer, nbytes );
    if(n < 0) {
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
        return ERROR_TIMEOUT;
      }
      return ERROR_FATAL;
    }
    return n;
  }
  */
  int Sock::read ( unsigned char* buffer, int nbytes ) const {
  again:
    int n = ::read ( m_sock, buffer, nbytes );
    if(n < 0) {
      if(errno == EINTR) {
	goto again;
      } else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      } else
	return ERROR_FATAL;
    } else if(n == 0) // far end node link will be off.
      return ERROR_FATAL;
    return n;
  }

  // 
  // This flag requests that the implementation does not send SIGPIPE on
  // error on stream oriented sockets when the other end breaks
  // connection. The EPIPE error is still returned as normal.
  int Sock::writeAll(unsigned char* buffer, int nbytes) const {

    ssize_t nwritten;
    unsigned char* ptr = buffer;
    size_t nleft = nbytes;

    while (nleft > 0) {
    again:
      if ( (nwritten = ::send ( m_sock, ptr, nleft, MSG_NOSIGNAL )) < 0) {
	if(errno == EINTR) {
	  goto again;
	} else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	  return ERROR_TIMEOUT;
	} else 	
	  return ERROR_FATAL;
      }
      nleft -= nwritten;
      ptr   += nwritten;
    }
    return(nbytes - nleft);
  }
  /*
    ssize_t nwritten;
    unsigned char* ptr = buffer;
    size_t nleft = nbytes;

    while (nleft > 0) {
      if ( (nwritten = ::write(m_sock, ptr, nleft)) <= 0) {
	if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	  return ERROR_TIMEOUT;
	}
	return ERROR_FATAL;
      }
      nleft -= nwritten;
      ptr   += nwritten;
    }
    return(nbytes - nleft);
  }
  */

  int Sock::readAll(unsigned char* buffer, int nbytes) const {
  again:
    int status = ::recv ( m_sock, buffer, nbytes, MSG_WAITALL);
    if ( status < 0 ) {
      if(errno == EINTR) {
	std::cerr << "readAll(unsigned char*, int): signal is received." << std::endl;
	goto again;
      }
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      } else {
	if(m_debug)
	  std::cerr << "status == -1   errno == " 
		    << errno << "  in Socket::readAll(unsigned char*, int)\n";
	return ERROR_FATAL;
      }
    } else if(status == 0) { // far end node link will be off.
      return ERROR_FATAL;
    }
    return status;
  }
  /*
    ssize_t nread;
    unsigned char* ptr = buffer;
    size_t nleft = nbytes;
    while (nleft > 0) {
      if ( (nread = ::read(m_sock, ptr, nleft)) < 0) {
	if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	  return ERROR_TIMEOUT;
	}
	if (errno == EINTR) {
	  nread = 0;
	}
	else {
	  return ERROR_FATAL;
	}
      } else if (nread == 0) {
	break;
      }
      nleft -= nread;
      ptr   += nread;
    }
    return(nbytes - nleft);
  */

  void Sock::disconnect(void) {
    if(m_debug) {
      std::cerr << "Sock::disconnect:enter" << std::endl;
    }
    ::close(m_sock);
    m_sock = -1; // we cannot reuse the socket.
  }

  int Sock::setOptNonBlocking ( const bool b ) const {
    int opts;
    
    opts = fcntl ( m_sock, F_GETFL );
    if ( opts == -1 ) {
      throw SockException("Sock::setNonBlocking(F_GETFL) error");
    }
    if ( b ) {
      opts = ( opts | O_NONBLOCK );
    }
    else {
      opts = ( opts & ~O_NONBLOCK );
    }
    fcntl ( m_sock, F_SETFL,opts );
    if ( opts == -1 ) {
      throw SockException("Sock::setNonBlocking(F_SETFL) error");
    }
    return SUCCESS;
  }

  int Sock::setOptReUse(const bool flag) const {
    int on;
    if(flag)
      on = 1;
    else
      on = 0;
    if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, 
		      ( const char* ) &on, sizeof ( on ) ) == -1 ) {
      throw SockException("Sock::create setsockopt(SO_REUSEADDR) error");
    }
    return SUCCESS;
  }

  int Sock::setOptNoDelay(bool flag) const {
    int on;
    if(flag)
      on = 1;
    else
      on = 0;
    if( setsockopt ( m_sock, IPPROTO_TCP, TCP_NODELAY, 
		     ( const char* ) &on, sizeof (on) ) == -1){
      throw SockException("Sock::create setsockopt(TCP_NODELAY) error");
    }
    if(m_debug)
      std::cerr << "Sock::setsockopt() NODELAY done\n";
    return SUCCESS;
  }

  int Sock::setOptRecvTimeOut(float time) {
    struct timeval tv;
    int status;
    m_timeout = time;
    if ((status = float2timeval(m_timeout, &tv)) < 0) {
      if(m_debug)
	std::cerr << "### fail conversion from timeout values to timeval structure\n";
    }
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_RCVTIMEO, 
				&tv, sizeof(tv))) < 0) {
      throw SockException("### Sock::connect setsockopt(SO_RCVTIMEO) error");
    }
    return SUCCESS;
  }

  int Sock::setOptSendTimeOut(float time) {
    struct timeval tv;
    int status;
    m_timeout = time;
    if ((status = float2timeval(m_timeout, &tv)) < 0) {
      if(m_debug)
	std::cerr << "### fail conversion from timeout values to timeval structure\n";
    }
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_SNDTIMEO, 
				&tv, sizeof(tv))) < 0) {
      throw SockException("### Sock::connect setsockopt(SO_SNDTIMEO) error");
    }
    return SUCCESS;
  }

  int Sock::setOptRecvBuf(int value) const {
    int val = value;
    int status;
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_RCVBUF, 
				&val, sizeof(val))) < 0) {
      throw SockException("### Sock::connect setsockopt(SO_RCVBUF) error");
    }
    return SUCCESS;
  }

  int Sock::setOptSendBuf(int value) const {
    int val = value;
    int status;
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_SNDBUF, 
				&val, sizeof(val))) < 0){
      throw SockException("### Sock::connect setsockopt(SO_SNDBUF) error");
    }
    return SUCCESS;
  }

  // written by Hiroshi Sendai
  int Sock::float2timeval(float sec, struct timeval *tv) const {
    unsigned int i;
    unsigned long tv_sec;
    unsigned long tv_usec;
    
    /*
      if (sec > 3600.0) {
      fprintf(stderr, "sleep time too large: %4.3f\n", sec);
      return -1;
      }
    */
	
    /* round to mili sec */
    i = static_cast<int>(sec) * 1000;
    tv_sec  = i / 1000;
    tv_usec = (static_cast<int>(sec) - tv_sec) * 1000000;
    
    tv->tv_sec  = tv_sec;
    tv->tv_usec = tv_usec;
    
    return SUCCESS;
  }
  
  int Sock::getSockFd() const {
    return m_sock;
  }
}
