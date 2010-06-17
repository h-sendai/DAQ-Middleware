#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include "Sock.h"
using namespace DAQMW;
using namespace std;

int main(int argc, char** argv) {

  int status;
  unsigned int packet[2], data;
  char buffer[1000];
  unsigned char p[8];

  try {
    Sock* sock = new Sock();
    for(int i=0; i<10; i++) {
      int status = sock->connect("192.168.0.16", 23);
      if(status==Sock::ERROR_TIMEOUT) {
	cout << "Sock connect timeout. retry..." << endl;
      }
      else
	break;
    }
    int len = 8;
    packet[0] = 0xa3;
    packet[1] = 0x28000000; // write length of 10 events
    for(int i=0; i<10; i++) {
      status = sock->sendAll(packet, len);
      if(status==Sock::ERROR_TIMEOUT) {
	cout << "Sock sendAll timeout. retry..." << endl;
      }
      else
	break;
    }
    cout << "Lenght of written data  = " << len;
    cout << "    Length of Actual written data = " << status << endl;

    len = 4; // read actual length of data which can be read.
    for(int i=0;i<10;i++) {
      status = sock->recvAll(&data, len);
      if(status==Sock::ERROR_TIMEOUT) {
	cout << "Sock recvAll Timeout. retry..." << endl;
      } else
	break;
    }
    cout << "actual length of data in short word = " << ntohl(data) << endl;

    len = ntohl(data)*2; // actual length in bytes
    for(int i=0; i<10; i++) {
      status = sock->recvAll((unsigned int*)buffer, len);
      if(status==Sock::ERROR_TIMEOUT) {
	cout << "Sock recvAll timeout. retry..." << endl;
      } else
	break;
    }
    cout << "Actual length of read data in byte = " << status << endl;
  } catch (SockException& e) {
    std::cerr << "Sock Fatal Error : " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Sock Fatal Error : Unknown" << std::endl;
  }
}
