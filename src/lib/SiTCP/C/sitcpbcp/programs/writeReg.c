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
    printf("usage : ./writeReg hostname address(in hex) data[0] data[1]...(in hex and in bytes)\n");
    exit(0);
  }

  offsetAddress = strtol(argv[2], (char**)NULL, 16);
  
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

  length = argc - 3;
  for(i=0;i<length;i++)
    buffer[i] = strtol(argv[i+3], (char**)NULL, 16);

  printf("registers(offset:0x%x) to be written: ", offsetAddress);
  for(i=0; i< length; i++) 
    printf("%02x ", buffer[i]);
  printf("\n");

  status = sitcpbcp_write_registers(&header, offsetAddress,
                                   buffer, length, &retlen);
  if(status < 0) {
    printf("sitcpbcp_read_registers: error\n");
    exit(1);
  }
  
  sitcpbcp_close(&header);
}
