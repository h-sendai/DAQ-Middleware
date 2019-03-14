/*!
 * @file Sock.cpp
 * @brief Implementation of the Sock class.
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

#include "Sock.h"

namespace DAQMW {

  SockException::SockException(const std::string& msg) : m_msg(msg) {}

  SockException::~SockException() throw() {}

  const char* SockException::what() {
    return m_msg.c_str();
  }

  Sock::Sock()
    : m_connectTimeout(2.0), m_debug(false) {
    memset ( &m_addr, 0, sizeof ( m_addr ) ); // This is for recvfrom(UDP)
    m_slen = sizeof(m_addr_other);            // This is for recvfrom(UDP)
    m_sock = -1;
    m_timeout = 2.0;
  }

  Sock::Sock(const std::string host, const int port ) 
    : m_connectTimeout(2.0), m_debug(false) {
    m_sock = -1;
    m_timeout = 2.0;
    m_ipAddress = host;
    m_port = port;
    memset ( &m_addr, 0, sizeof ( m_addr ) );
    if(m_debug) {
      std::cerr << "Sock::Sock:ipaddress = "<<  m_ipAddress 
		<< "  port = " << m_port << std::endl;
    }
    // for sendTo(UDP)
    memset(&m_addr_other, 0, sizeof (m_addr_other));
    m_addr_other.sin_family = AF_INET;
    m_addr_other.sin_port = htons(m_port);
    if (m_debug)
      std::cout << "Sock::Sock:m_addr_other.sin_port = " << m_addr_other.sin_port << std::endl;
    int status = inet_pton(AF_INET, m_ipAddress.c_str(),
                           &m_addr_other.sin_addr );
    if(status <= 0) {
      perror("### ERROR: Sock::constructor:inet_pton");
    }
    m_slen = sizeof(m_addr_other); // This is for recvfrom(UDP)
  }

  Sock::~Sock() {
    if ( Sock::isValidSock() )
      ::close ( m_sock );
  }

  // For Server
  int Sock::create(void) {
    return createTCP();
  }

  int Sock::createTCP() {
    m_sock = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( ! isValidSock() ) {
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

  int Sock::createUDP(void) {
    m_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (!isValidSock()) {
      perror("### ERROR: Sock::createUDP():socket");
      throw SockException("Sock::createUDP error");
    }
    if (m_debug)
      std::cerr << "Sock::createUDP() done\n";
    setOptReUse(true); // if error, error will be thrown.
    if (m_debug)
      std::cerr << "Sock::setsockopt() REUSE done\n";
    return SUCCESS;
  }

  int Sock::bind ( const int port ) {
    int status;
    if ( ! isValidSock() ){
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
    if ( ! isValidSock() ){
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
    } else {
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

  int Sock::connect ( const std::string host, const int port ) {
    if ( ! isValidSock() ) {
      m_sock = socket ( AF_INET, SOCK_STREAM, 0 );
      if (m_debug)
	std::cerr << "Sock::connect(string,int): socket is created" << std::endl;
      if (!isValidSock())
        return ERROR_FATAL;
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons ( port );
    
    int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );
    if (status < 0) {
      perror("### ERROR: Sock::connect(string, int) inet_pton");
      return ERROR_FATAL;
    } else if(status == 0) { // specified by hostname not ip
      struct hostent *hostinfo = gethostbyname(host.c_str());
      if(hostinfo != NULL) {
	m_addr.sin_addr.s_addr = *(unsigned int*)hostinfo->h_addr_list[0];
      } else {
	std:: cerr << "### ERROR: Sock::connect(string, int) gethostbyname" << std::endl;
	return ERROR_FATAL;
      }
    }
    if (m_debug)
      std::cerr << "Sock::connect(string,int): inet_pton() done" << std::endl;
    setAlarmTimer();

    status = setOptReUse(true);
    if (status < 0)
      return status;
    status = setOptRecvTimeOut(m_timeout);
    if (status < 0)
      return status;
    status = setOptSendTimeOut(m_timeout);
    if (status < 0)
      return status;

    status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
    alarm(0);
    if(status < 0) {
      if(errno== EINTR)
	return ERROR_TIMEOUT;
      perror("### ERROR: Sock::connect(string, int) connect");
      return ERROR_FATAL;
    }
    if (m_debug)
      std::cerr << "Sock::connect(string,int): connect done" << std::endl;

    return SUCCESS;
  }

  int Sock::connect(int type) {
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
      return ERROR_FATAL;
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
    if (m_debug)
      std::cerr << "Sock::connect(int):setsockopt:RecvTimeout done" << std::endl;
    // Send(send/write) timeout is set.
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_SNDTIMEO, 
				&tv, sizeof(tv))) < 0) {
      perror("### ERROR: Sock::connect(int):setsockopt:SendTimeout");
      return ERROR_FATAL;
    }
    if (m_debug)
      std::cerr << "Sock::connect(int):setsockopt:SendTimeout done" << std::endl;

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
    if(nleft != 0)
      throw SockException("Sock::sendAll(const string) error:not same size");
    else
    return SUCCESS;
  }

  int Sock::sendAll ( const unsigned int* s, int size ) const {
    ssize_t nwritten;
    unsigned char* ptr = (unsigned char*)s;
    size_t nleft = size;
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
    if(nleft != 0)
      throw SockException("Sock::sendAll(const unsigned int*, int) error: not same size");
    else
      return SUCCESS;
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
      perror("### ERROR: Sock::recv(string&)");
      throw SockException("Sock::recv(string&) fatal error");	
    } else if(status == 0) {  // far end node link will be off.
      perror("### ERROR: Sock::recv(string&)");
      throw SockException("Sock::recv(string&) fatal error: far end node link off");
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
	perror("### ERROR: Sock::recv(unsigned int*, int)");
	throw SockException("Sock::recv(int*, int) error");
      }
    } else if(status == 0) {  // far end node link will be off.
      perror("### ERROR: Sock::recv(unsigned int*,int)");
      throw SockException("Sock::recv(unsigned int*,int) fatal error: far end node link off");
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
      perror("### ERROR: Sock::recvAll(string&,int&):recv fatal error");
      throw SockException("Sock::recvAll(string&, int&) fatal error");
    } else if(status == 0) {  // far end node link will be off.
      perror("### ERROR: Sock::recvAll(string&,int&):recv far end node link off");
      throw SockException("Sock::recv(string&, int&) fatal error: far end node link off");
    } else {
      s = buf;
      if (status != size)
        throw SockException("Sock::recv(string&, int&) fatal error: not same size");
      else
        return SUCCESS;
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
	perror("### ERROR: Sock::recvAll(unsigned int, int):recv fatal error");
	throw SockException("Sock::recv(unsigned int*, int) fatal error");
      }
    } else if(status == 0) { // far end node link will be off.
      perror("### ERROR: Sock::recvAll(unsigned int, int):recv far end node link off");
      throw SockException("Sock::recv(unsigned int*, int) fatal error: far end node link off");
      if (status != size)
	throw SockException("Sock::recv(unsigned int*, int) fatal error: not same size");
    }
    return SUCCESS;
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
      } else if (errno == EPIPE) {
        perror("### ERROR: Sock::write(unsigned char*,int):send far end node link off");
      } else {
        perror("### ERROR: Sock::write(unsigned char*,int):send fatal error");
      }
      return ERROR_FATAL;
    } else { // success
      return status;
    }
  }

  int Sock::read ( unsigned char* buffer, int nbytes ) const {
  again:
    int n = ::read ( m_sock, buffer, nbytes );
    if (m_debug)
      std::cout << "n = ::read() = " << n << std::endl;
    if(n < 0) {
      if(errno == EINTR) {
	goto again;
      } else if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      } else {
	perror("### ERROR: Sock::read(unsigned char*,int):read fatal error");
	return ERROR_FATAL;
      }
    } else if(n == 0) { // far end node link will be off.
      perror("### ERROR: Sock::read(unsigned char*,int):read far end node link off");
      return ERROR_FATAL;
    }
    return n;
  }

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
	} else if (errno == EPIPE) {
          perror("### ERROR: Sock::write(unsigned char*,int):send far end node link off");
	  return ERROR_FATAL;
        } else {
	  perror("### ERROR: Sock::writeAll(unsigned char*,int):send fatal error");
	  return ERROR_FATAL;
	}
      }
      nleft -= nwritten;
      ptr   += nwritten;
    }
    if (nleft != 0) {
      perror("### ERROR: Sock::writeAll(unsigned int, int):send not same size");
      return ERROR_NOTSAMESIZE;
    } else
      return SUCCESS;
  }

  int Sock::readAll(unsigned char* buffer, int nbytes) const {
  again:
    int status = ::recv ( m_sock, buffer, nbytes, MSG_WAITALL);
    if ( status < 0 ) {
      if(errno == EINTR) {
	goto again;
      }
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	return ERROR_TIMEOUT;
      } else {
	perror("### ERROR: Sock::readAll(unsigned char*,int):recv fatal error");
	return ERROR_FATAL;
      }
    } else if(status == 0) { // far end node link will be off.
      perror("### ERROR: Sock::readAll(unsigned char*,int):recv far end node link off");
      return ERROR_FATAL;
    }
    if(status != nbytes) {
      perror("### ERROR: Sock::readAll(unsigned int, int):recv not same size");
      return ERROR_NOTSAMESIZE;
    }
    return SUCCESS;
  }

  int Sock::writeTo(const unsigned char* buffer, int nbytes) {
    m_slen = sizeof(m_addr_other);
  again:
    int status = ::sendto(m_sock, buffer, nbytes, MSG_NOSIGNAL,
                          (struct sockaddr*)&m_addr_other, m_slen);
    if (m_debug)
      std::cout << "Sock::writeTo:port # sent = " << m_addr_other.sin_port << std::endl;
    if (status < 0) {
      if(errno == EINTR) {
        goto again;
      }
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
        return ERROR_TIMEOUT;
      } else if (errno == EPIPE) {
        perror("### ERROR: Sock::writeTo(unsigned char*,int):sendto far end node link off");
      } else {
	perror("### ERROR: Sock::writeTo(unsigned char*,int):sendto fatal error");
        return ERROR_FATAL;
      }
    }
    return status; // number of data received
  }

  int Sock::readFrom(unsigned char* buffer, int nbytes)
  {
  again:
    int status = ::recvfrom(m_sock, buffer, nbytes, 0,
                            (struct sockaddr*)&m_addr_other, &m_slen);
    if (m_debug)
      std::cout << "Sock::readFrom:port # sent = " << m_addr_other.sin_port << \
	std::endl;
    if (status < 0) {
      if(errno == EINTR) {
	std::cerr << "readFrom(unsigned char*, int): signal is received.";
	std::cerr << std::endl;
        goto again;
      }
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
        return ERROR_TIMEOUT;
      } else {
        perror("### ERROR: Sock::readFrom(unsigned char*,int):recvfrom fatal error");
        return ERROR_FATAL;
      }
    } else if(status == 0) { // far end node link will be off.
      perror("### ERROR: Sock::readFrom(unsigned char*,int):recvfrom far end node link off");
      return ERROR_FATAL;
    }
    return status; // number of data received
  }

  void Sock::disconnect(void) {
    ::close(m_sock);
    m_sock = -1;
    if (m_debug)
      std::cerr << "Sock::disconnect:close done" << std::endl;
  }

  int Sock::setOptNonBlocking ( const bool flag ) const {
    int opts;
    
    opts = fcntl ( m_sock, F_GETFL );
    if ( opts == -1 ) {
      perror("### ERROR: Sock::setNonBlocking(F_GETFL) error");
      throw SockException("Sock::setNonBlocking(F_GETFL) error");
    }
    if ( flag ) {
      opts = ( opts | O_NONBLOCK );
    } else {
      opts = ( opts & ~O_NONBLOCK );
    }
    fcntl ( m_sock, F_SETFL,opts );
    if ( opts == -1 ) {
      perror("### ERROR: Sock::setNonBlocking(F_SETFL) error");
      throw SockException("Sock::setNonBlocking(F_SETFL) error");
    }
    if (m_debug)
      std::cerr << "Sock::setNonBlocking() done" << std::endl;
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
      perror("### ERROR: Sock::setOptReUse:setsockopt(SO_REUSEADDR) error");
      throw SockException("Sock::create setsockopt(SO_REUSEADDR) error");
    }
    if (m_debug)
      std::cerr << "Sock::setOptReUse() done" << std::endl;
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
      perror("### ERROR: Sock::setOptNoDelay() error");
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
	perror("### ERROR: Sock::setOptRecvTimeOut():float2timeval error");
    }
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_RCVTIMEO, 
				&tv, sizeof(tv))) < 0) {
      perror("### ERROR: Sock::setOptRecvTimeOut:setsockopt error");
      throw SockException("### Sock::connect setsockopt(SO_RCVTIMEO) error");
    }
    if (m_debug)
      std::cerr << "Sock::setOptRecvTimeOut() done\n";
    return SUCCESS;
  }

  int Sock::setOptSendTimeOut(float time) {
    struct timeval tv;
    int status;
    m_timeout = time;
    if ((status = float2timeval(m_timeout, &tv)) < 0) {
      perror("### ERROR: Sock::setOptSendTimeOut():float2timeval error");
    }
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_SNDTIMEO, 
				&tv, sizeof(tv))) < 0) {
      perror("### ERROR: Sock::setOptSendTimeOut: fatal error");
      throw SockException("### Sock::connect setsockopt(SO_SNDTIMEO) error");
    }
    if (m_debug)
      std::cerr << "Sock::setOptSendTimeOut() done\n";
    return SUCCESS;
  }

  int Sock::setOptRecvBuf(int value) const {
    int val = value;
    int status;
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_RCVBUF, 
				&val, sizeof(val))) < 0) {
      perror("### ERROR: Sock::setOptRecvBuf: fatal error");
      throw SockException("### Sock::connect setsockopt(SO_RCVBUF) error");
    }
    if (m_debug)
      std::cerr << "Sock::setOptRecvBuf() done\n";
    return SUCCESS;
  }

  int Sock::setOptSendBuf(int value) const {
    int val = value;
    int status;
    if ( (status = setsockopt ( m_sock, SOL_SOCKET, SO_SNDBUF, 
				&val, sizeof(val))) < 0){
      perror("### ERROR: Sock::setOptSendBuf: fatal error");
      throw SockException("### Sock::connect setsockopt(SO_SNDBUF) error");
    }
    if (m_debug)
      std::cerr << "Sock::setOptSendBuf() done\n";
    return SUCCESS;
  }

  int Sock::float2timeval(float sec, struct timeval *tv) const {
    unsigned long tv_sec;
    unsigned long tv_usec;
    
    tv_sec = static_cast<int>(sec);
    tv_usec = sec*1000000 - static_cast<int>(sec)*1000000;
    
    tv->tv_sec  = tv_sec;
    tv->tv_usec = tv_usec;
    
    return SUCCESS;
  }
  
  int Sock::getSockFd() const {
    return m_sock;
  }

  int Sock::readNum(int* msgSize) const {
    int status = ioctl(m_sock, FIONREAD, msgSize);
    if(status < 0) {
      perror("### ERROR: Sock::readNum:ioctl fatal error");
      return ERROR_FATAL;
    }
    if (m_debug)
      std::cerr << "Sock::readNum() done\n";
    return SUCCESS;
  }

}
