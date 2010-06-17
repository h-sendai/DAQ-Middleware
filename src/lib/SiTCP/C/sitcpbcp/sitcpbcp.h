/*
 sitcpbcp.h, written by Yoshiji Yasu (KEK), 2008
 modified: July 2008, TCP interface is now not necessary
*/

#ifndef SITCPBCP_H
#define SITCPBCP_H

#include <stdlib.h>
#include <string.h>

#include "sock.h"

/* 
 * sitcpbcp_debug: if greater than 0, print debug messages in this library
 * Default value is 0 (does not print debug messages).
 */
extern int sitcpbcp_debug;

//The data type is defined in network byte order
//Therefore, the header should not be converted into network order except
// "address"(htons(address)).
typedef struct bcp_header {
  unsigned char type;
  unsigned char command;
  unsigned char id;
  unsigned char length;
  unsigned int  address;
} bcp_header;

typedef struct sitcpbcp_header {
  unsigned char* packet;
  sock_header sock_udp_header;
  bcp_header send_header;
  bcp_header recv_header;
} sitcpbcp_header;

// definition of specific error codes: none

// public methods
// 
int sitcpbcp_open(sitcpbcp_header* sitcpbcp_header, char* ip_address,
                  int udp_port, int maxBuf);
void sitcpbcp_close(sitcpbcp_header* sitcpbcp_header);

// The method connects to the SiTCP with not only TCP but also UDP 
int sitcpbcp_connect(sitcpbcp_header* sitcpbcp_header);

void sitcpbcp_disconnect(sitcpbcp_header* sitcpbcp_header);

// for UDP
// The methods writes data into registers in the UDP space of SiTCP
int sitcpbcp_write_registers(sitcpbcp_header* sitcpbcp_header,
			     int offsetAddress, unsigned char* sendBuf,
			     int length, int* retlen);
// The methods reads data from regsiters in the UDP space of SiTCP
int sitcpbcp_read_registers(sitcpbcp_header* sitcpbcp_header,
			    int offsetAddress, unsigned char* recvBuf,
			    int length, int* retlen);

// private methods
void sitcpbcp_set_header(bcp_header* header, unsigned char type,
			 unsigned char command,
			 unsigned char id,
			 unsigned char length,
			 unsigned int address);
void sitcpbcp_extract_header(bcp_header* header, unsigned char* type,
			 unsigned char* command,
			 unsigned char* id,
			 unsigned char* length,
			 unsigned int* address);
void sitcpbcp_udp_unpack(unsigned char* packet,
		     bcp_header* recv_header, unsigned char* buffer);
void sitcpbcp_udp_unpack_h(unsigned char* packet, bcp_header* recv_header);

#endif // SITCPBCP_H
