// -*- C++ -*-
// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Sock.h"

namespace DAQMW {

//class ServerSocket : private Sock
class ServerSocket : public Sock
{
 public:

  ServerSocket ( int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;
  void write( const unsigned int* , int );
  int  read( unsigned int *, int );
  
  void accept ( ServerSocket& );

};

}///namespace

#endif
