// -*- C++ -*-
/*!
 * @file sitcpbcp.c
 * @brief 
 * @date 1-July-2008
 * @author Hiroshi Sendai (hiroshi.sendai@kek.jp)
 * @author Yoshiji Yasu (yoshiji.yasu@kek.jp)
 *
 * Copyright (C) 2008-2011
 *     Hiroshi Sendai and Yoshiji Yasu
 *     High Energy Accelerator Research Organization (KEK), Japan
 *     All rights reserved.
 *
 */

#include <errno.h>
#include "sitcpbcp.h"

int sitcpbcp_debug = 0;

int sitcpbcp_open(sitcpbcp_header* sitcpbcp_header, char* ip_address,
                  int udp_port, int maxBuf) {
  //int status;

  sock_open(&sitcpbcp_header->sock_udp_header, 
	     ip_address, udp_port);
  sitcpbcp_header->packet =  malloc(sizeof(bcp_header) + maxBuf);
  if( sitcpbcp_header->packet < 0)
    return ERROR_FATAL;
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_open:success\n");
  }
  return SUCCESS;
}

void sitcpbcp_close(sitcpbcp_header* sitcpbcp_header) {
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_close:enter\n");
  }
  sock_close(&sitcpbcp_header->sock_udp_header);
  free(sitcpbcp_header->packet);
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_close:success\n");
  }
}

int sitcpbcp_connect(sitcpbcp_header* sitcpbcp_header) {
  int status;

  status = sock_connect_udp(&sitcpbcp_header->sock_udp_header);
  if(status < 0) {
    if(status == ERROR_TIMEOUT) {
	  if (sitcpbcp_debug > 0) {
        printf("sitcpbcp_connect:connection Timeout\n");
	  }
	}
    else {
	  if (sitcpbcp_debug > 0) {
        printf("sitcpbcp_connect:UDP fail\n");
	  }
	}
    return status;
  }
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_connect: udp connection success\n");
  }
  return status;
}

void sitcpbcp_disconnect(sitcpbcp_header* sitcpbcp_header) {
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_disconnect:success\n");
  }
  sock_disconnect(&sitcpbcp_header->sock_udp_header);
}

// for UDP
void sitcpbcp_udp_pack(unsigned char* packet,
                   bcp_header* send_header,
                   unsigned char* buffer) {
  bcp_header header;

  header.type = send_header->type;
  header.command = send_header->command;
  header.id = send_header->id;
  header.length = send_header->length;
  header.address = htonl(send_header->address);
  memcpy(packet, &header, sizeof(bcp_header));
  memcpy(packet+sizeof(bcp_header), buffer, header.length);
}

void sitcpbcp_udp_pack_h(unsigned char* packet,
			 bcp_header* send_header) {
  bcp_header header;

  header.type = send_header->type;
  header.command = send_header->command;
  header.id = send_header->id;
  header.length = send_header->length;
  header.address = htonl(send_header->address);
  memcpy(packet, &header, sizeof(bcp_header));
}

void sitcpbcp_udp_unpack(unsigned char* packet,
		       bcp_header* recv_header, unsigned char* buffer) {
  memcpy(recv_header, packet, sizeof(bcp_header));
  recv_header->address = ntohl(recv_header->address);
  memcpy(buffer, packet+sizeof(bcp_header), recv_header->length);
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_udp_unpack: length=%d  offsetAddress=%x\n",
	   recv_header->length, recv_header->address);
  }
}

void sitcpbcp_udp_unpack_h(unsigned char* packet,
		       bcp_header* recv_header) {
  memcpy(recv_header, packet, sizeof(bcp_header));
  recv_header->address = ntohl(recv_header->address);
}

void sitcpbcp_set_header(bcp_header* bcp_header,
			 unsigned char type,
			 unsigned char command,
			 unsigned char id,
			 unsigned char length,
			 unsigned int  address) {

  bcp_header->type = type;
  bcp_header->command = command;
  bcp_header->id = id;
  bcp_header->length = length;
  bcp_header->address = address;
}

void sitcpbcp_extract_header(bcp_header* bcp_header,
			 unsigned char* type,
			 unsigned char* command,
			 unsigned char* id,
			 unsigned char* length,
			 unsigned int*  address) {

  *type = bcp_header->type;
  *command = bcp_header->command;
  *id = bcp_header->id;
  *length = bcp_header->length;
  *address = bcp_header->address;
}

int sitcpbcp_write_registers(sitcpbcp_header* sitcpbcp_header,
			     int offsetAddress, unsigned char* sendBuf,
			     int length, int* retlen) {
  int status;
  unsigned char type;
  unsigned char command;
  unsigned char id;
  bcp_header* send_header;
  bcp_header* recv_header;
  sock_header* sock_udp_header;

  type = 0xFF;
  command = 0x80; // write
  id = 1;
  send_header = &sitcpbcp_header->send_header;
  recv_header = &sitcpbcp_header->recv_header;
  sock_udp_header = &sitcpbcp_header->sock_udp_header;

  sitcpbcp_set_header(send_header, type, command, id, length, offsetAddress);
  sitcpbcp_udp_pack(sitcpbcp_header->packet, send_header, sendBuf);
  status = sock_write_all(sock_udp_header, sitcpbcp_header->packet,
			  sizeof(bcp_header)+length);
  if(status <= 0)
    return status;
  
  status = sock_read_all(sock_udp_header, 
			 (unsigned char*)sitcpbcp_header->packet, 
			 sizeof(bcp_header));
  if(status < 0)
    return status;
  sitcpbcp_udp_unpack_h(sitcpbcp_header->packet, recv_header);
  *retlen = length;
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_write_registers:success\n");
  }

  return status;
}

int sitcpbcp_read_registers(sitcpbcp_header* sitcpbcp_header,
			    int offsetAddress, unsigned char* recvBuf,
			    int length, int* retlen) {
  int status, i;
  unsigned char type;
  unsigned char command;
  unsigned char id;
  bcp_header* send_header;
  bcp_header* recv_header;
  sock_header* sock_udp_header;
  
  type = 0xFF;
  command = 0xC0; // read
  id = 1;
  send_header = &sitcpbcp_header->send_header;
  recv_header = &sitcpbcp_header->recv_header;
  sock_udp_header = &sitcpbcp_header->sock_udp_header;

  sitcpbcp_set_header(send_header, type, command, id, length, offsetAddress);
  sitcpbcp_udp_pack_h(sitcpbcp_header->packet, send_header);
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_read_registers: write header:\n");
    for(i=0; i<sizeof(bcp_header); i++)
      printf("%2x ", (unsigned char)sitcpbcp_header->packet[i]);
    printf("\n");
  }
  status = sock_write(sock_udp_header,
		      (unsigned char*)sitcpbcp_header->packet,
		      sizeof(bcp_header));
  //  status = sock_write_all(sock_udp_header, (unsigned char*)send_header,
  if(status < 0)
    return status;
  if (sitcpbcp_debug > 0) {
    printf("sock_write_all is done...\n");
  }

  //  status = sock_read_all(sock_udp_header, 
  bzero((unsigned char*)sitcpbcp_header->packet, sizeof(bcp_header)+length);
  status = sock_read(sock_udp_header, 
			 (unsigned char*)sitcpbcp_header->packet,
			 sizeof(bcp_header)+length);
  if(status < 0)
    return status;
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_read_registers: read header: ");
  }
  if (sitcpbcp_debug > 0) {
    for(i=0; i<(sizeof(bcp_header)+length); i++)
      printf("%2x ", (unsigned char)sitcpbcp_header->packet[i]);
    printf("\n");
  }
  sitcpbcp_udp_unpack(sitcpbcp_header->packet, recv_header, recvBuf);
  *retlen = length;
  if (sitcpbcp_debug > 0) {
    printf("sitcpbcp_read_registers:success\n");
  }
  return status;
}
