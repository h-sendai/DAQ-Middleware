#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include "Sock.h"
using namespace DAQMW;
using namespace std;

int main(int argc, char** argv) {

  int status;
  unsigned int packet[2], data;
  unsigned char buffer[1000];

  Sock* sock = new Sock("192.168.0.16", 23);
  //  Sock* sock = new Sock("192.168.0.19", 23);
  status = sock->connectTCP();
  if(status < 0) {
    cout << "Sock connect fail" << endl;
    return 1;
  }
  int len = 8;
  packet[0] = 0xa3;
  packet[1] = 0x28000000; // write length of 10 events
  status = sock->write((unsigned char*)packet, len);
  if(status < 0) {
    cout << "Sock write fail" << endl;
    return 1;
  }
  cout << "Lenght of written data  = " << len;
  cout << "    Length of Actual written data = " << status << endl;

  len = 4; // read actual length of data which can be read.
  status = sock->read((unsigned char*)&data, len);
  if(status < 0) {
    cout << "Sock read fail" << endl;
    return 1;
  }
  
  cout << "actual length of data in short word = " << ntohl(data) << endl;

  len = ntohl(data)*2; // actual length in bytes
  status = sock->read(buffer, len);
  if(status < 0) {
    cout << "Sock read fail" << endl;
    return 1;
  }
  cout << "Actual length of read data in byte = " << status << endl;
}
