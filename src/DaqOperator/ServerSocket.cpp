// Implementation of the ServerSocket class
#include <iostream>
#include "ServerSocket.h"

namespace DAQMW {

ServerSocket::ServerSocket ( int port )
{
  std::cerr << "ServerSocket: enter : port = " << port << std::endl;

  if ( Sock::create() != 0)
  {
      throw SockException ( "Could not create server socket." );
  }

  std::cerr << "ServerSocket: create:done" << std::endl;

  if ( Sock::bind ( port ) != 0)
  {
      throw SockException ( "Could not bind to port." );
  }

  std::cerr << "ServerSocket: bind: done" << std::endl;
  if ( Sock::listen() != 0)
  {
      throw SockException ( "Could not listen to socket." );
  }
  std::cerr << "ServerSocket: listen : done and exit" << std::endl;

}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( Sock::send ( s ) != 0)
  {
      throw SockException ( "Could not write to socket." );
  }
  
  return *this;

}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
/*
  if ( ! Sock::recv ( s ) )
  {
      throw SockException ( "Could not read from socket." );
    }

std::cout << "buf=" << s << std::endl;
*/

    int buf[2];
    if ( Sock::recv((unsigned int*)buf, sizeof(int)) < 0)
    {
	throw SockException ( "Could not read from socket." );
    }

    //std::cerr << "buf[0]=" << buf[0] << std::endl;
    int size = buf[0];

    char *buf2 = ((char *)calloc(size+1, sizeof(char)));
    if ( Sock::recv((unsigned int*)buf2, size) < 0)
    {
	throw SockException ( "Could not read from socket." );
    }

    buf2[size] = '\0';
    
    s = buf2;
    delete(buf2);

    return *this;
}

void ServerSocket::write( const unsigned int* s, int size )
{
    if ( Sock::send ( s, size ) != 0)
    {
      throw SockException ( "Could not write to socket." );
    }

}

int ServerSocket::read( unsigned int* s, int size )
{
    int status;
    std::cerr << "size:" << size << std::endl;
    status =  Sock::recv ( s, size );
    std::cerr << "status:" << status << std::endl;
    if ( status < 0 )
    {
	throw SockException ( "Could not read from socket." );
    }

  return status;
}

void ServerSocket::accept ( ServerSocket& sock )
{
    //if ( ! Sock::accept ( sock ) )
    if ( Sock::accept ( sock ) != 0)
    {
	throw SockException ( "Could not accept socket." );
    }
}

}///namespace
