#include <errno.h>
#include <stdio.h>
#include "sitcpbcp.h"
#define MAX_BUF 1024

int main(int argc, char** argv) {

  sitcpbcp_header header;
  int i;
  int status, length, retlen;
  int offsetAddress;
  unsigned char buffer[MAX_BUF];

  if( argc < 4 ) {
    printf("usage : ./readcmd hostname length address(in hex)\n");
    exit(0);
  }

  length = atoi(argv[2]);
  offsetAddress = strtol(argv[3], (char**)NULL, 16);
  
  status = sitcpbcp_open(&header, argv[1], 4660, MAX_BUF);
  if(status < 0) {
    printf("sitcpbcp_open: error\n");
    exit(1);
  }
  status = sitcpbcp_connect(&header);
  if(status == 1)
    printf("moduletest:TimeOut occurred...\n");
  if(status < 0) {
    printf("sitcpbcp_connect: error\n");
    exit(1);
  }

  status = sitcpbcp_read_registers(&header, offsetAddress,
                                   buffer, length, &retlen);
  if(status < 0) {
    printf("sitcpbcp_read_registers: error\n");
    exit(1);
  }
  printf("registers(offset:0x%x): ", offsetAddress);
  for(i=0; i< length; i++) 
    printf("%02x ", buffer[i]);
  printf("\n");
  
  sitcpbcp_close(&header);
}
