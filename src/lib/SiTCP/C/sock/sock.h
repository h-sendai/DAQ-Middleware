// -*- C++ -*-
/*!
 * @file sock.h
 * @brief Socket library
 * @date 1-January-2008
 * @author Hiroshi Sendai (hiroshi.sendai@kek.jp),
 * @author Yoshiji Yasu (yoshiji.yasu@kek.jp)
 *
 * Copyright (C) 2008-2011
 *     Hiroshi Sendai and Yoshiji Yasu
 *     High Energy Accelerator Research Organization (KEK), Japan
 *     All rights reserved.
 *
 */

#ifndef _SOCK_C_H
#define _SOCK_C_H 1

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

/* 
 * sock_debug: if greater than 0, print debug messages in this library
 * Default value is 0 (does not print debug messages).
 */
extern int sock_debug;

typedef struct sock_header {
  char* ip_address;
  int port;
  float timeout;
  int sockfd;
} sock_header;

#define SOCK_TIMEOUT_TIME 2.0

// definition of error codes
#define SUCCESS                 0 // success
#define ERROR_FATAL            -1 // fatal
#define ERROR_TIMEOUT          -2 // Timeout

void sock_open(sock_header* header, char* ip_address, int port);
void sock_close(sock_header* header);
int sock_connect_tcp(sock_header* header);
int sock_connect_udp(sock_header* header);
void sock_disconnect(sock_header* header);

int sock_write(sock_header* header, unsigned char* buffer, int nbytes);
int sock_read(sock_header* header, unsigned char* buffer, int nbytes);
int sock_write_all(sock_header* header, unsigned char* buffer, int nbytes);
int sock_read_all(sock_header* header, unsigned char* buffer, int nbytes);
int sock_read_select(sock_header* header, unsigned char* buffer, int nbytes);

// private 
int float2timeval(float sec, struct timeval *tv);
int connect_sitcp(char *ip_address, int port, float timeout, int type);

#endif // _SOCK_C_H
