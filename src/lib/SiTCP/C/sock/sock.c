// -*- C++ -*-
/*!
 * @file sock.c
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

#include <stdio.h>
#include "sock.h"

int sock_debug = 0;

void sock_open(sock_header* header, char* ip_address,
               int port) {
  header->ip_address = ip_address;
  header->port = port;
  header->timeout = SOCK_TIMEOUT_TIME;
}

void sock_close(sock_header* header) {
  close(header->sockfd);
}


int sock_connect_tcp(sock_header* header) {
  int fd;

  if (sock_debug > 0) {
    printf("sock_connect_tcp:enter\n");
    printf("ip address = %s port = %d\n", header->ip_address, header->port);
  }
  fd = connect_sitcp(header->ip_address, header->port, 
		     header->timeout, SOCK_STREAM);
  header->sockfd = fd;
  if( fd == -1 ) {
    if(errno == EHOSTUNREACH) {
	  if (sock_debug > 0) {
        printf("sock_connect_tcp:TimeOut occurred...\n");
	  }
      return ERROR_TIMEOUT;
    }
    return ERROR_FATAL;
  }
  //  printf("sock_connect_tcp:success\n");
  return SUCCESS;
}

int sock_connect_udp(sock_header* header) {
  int fd;
  fd = connect_sitcp(header->ip_address, header->port, 
		     header->timeout, SOCK_DGRAM);
  header->sockfd = fd;
  return SUCCESS;
}

void sock_disconnect(sock_header* header) {
  close(header->sockfd);
}

int sock_write(sock_header* header, unsigned char* buffer, int nbytes) {
  int n;
  if ( (n = write(header->sockfd, buffer, nbytes)) < 0) {
    if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
      return ERROR_TIMEOUT;
    }
    return ERROR_FATAL;
  }
  return n;
}

int sock_read(sock_header* header, unsigned char* buffer, int nbytes) {
  int nread;

  nread = read(header->sockfd, buffer, nbytes);
  if ( nread < 0) {
    if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
      return ERROR_TIMEOUT;
    }
    return ERROR_FATAL;
  }
  return nread;
}

int sock_write_all(sock_header* header, unsigned char* buffer, int nbytes) {
  size_t  nleft;
  ssize_t nwritten;
  unsigned char    *ptr;

  ptr = buffer;
  nleft = nbytes;
  while (nleft > 0) {
    if ( (nwritten = write(header->sockfd, ptr, nleft)) <= 0) {
      if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
        return ERROR_TIMEOUT;
      }
      return ERROR_FATAL;
    }
    nleft -= nwritten;
    ptr   += nwritten;
  }
  return(nbytes - nleft);
}    

int sock_read_all(sock_header* header, unsigned char* buffer, int nbytes) {
  //  size_t  nleft;
  //  ssize_t nread;
  int  nleft;
  int nread;
  unsigned char    *ptr;

  ptr = buffer;
  nleft = nbytes;
  while (nleft > 0) {
    nread = read(header->sockfd, ptr, nleft);
    if ( nread < 0) {
      if (errno == EINTR)
	nread = 0; /* and call read() again */
      else {
	if((errno == ETIMEDOUT)||(errno == EAGAIN)) {
	  return ERROR_TIMEOUT;
	}
	if (sock_debug > 0) {
	  printf("sock_read_all:nread = %d\n", nread);
	}
	return ERROR_FATAL;
      }
    } else if (nread == 0)
      break;                          /* EOF */
    nleft -= nread;
    ptr   += nread;
	if (sock_debug > 0) {
      printf("sock_read_all:(nbytes -nleft) = %d\n", (nbytes - nleft));
	}
  }
  return(nbytes - nleft);              /* return >= 0 */
}

int sock_read_select(sock_header* header, unsigned char* buffer, int nbytes) {
  int n;
  fd_set setSelect;
  struct timeval timeout;
  int num_fds;

  FD_ZERO(&setSelect);
  FD_SET(header->sockfd, &setSelect);
  float2timeval(header->timeout, &timeout);
  num_fds = select(header->sockfd+1, &setSelect, NULL, NULL,&timeout);
  if(num_fds < 0) { // fatal
    return ERROR_FATAL;
  } else if(num_fds == 0){ /* time out */
    return ERROR_TIMEOUT;
  } else {
    /* receive packet */
    if(FD_ISSET(header->sockfd, &setSelect)){
      n = read(header->sockfd, buffer, nbytes);
      if(n < 0) 
	return ERROR_FATAL;
    } else
      return 0; // no data
  }
  return n;
}

// written by Hiroshi Sendai
int float2timeval(float sec, struct timeval *tv)
{
	unsigned int i;
	unsigned long tv_sec;
	unsigned long tv_usec;

	/*
	if (sec > 3600.0) {
		fprintf(stderr, "sleep time too large: %4.3f\n", sec);
		return -1;
	}
	*/

	/* round to mili sec */
	i = sec * 1000;
	tv_sec  = i / 1000;
	tv_usec = (sec - tv_sec) * 1000000;

	tv->tv_sec  = tv_sec;
	tv->tv_usec = tv_usec;

	return 0;
}

// written by Hiroshi Sendai
int connect_sitcp(char *ip_address, int port, float timeout, int type)
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct timeval tv;
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	if (port < 0 || port > 65535) {
		/* no errno */
		warnx("port number invalid: %d", port);
		return ERROR_FATAL;
	}
	servaddr.sin_port = htons(port);
	if (inet_aton(ip_address, &servaddr.sin_addr) == 0) {
		/* no errno */
		warnx("IP address invalid: %s", ip_address);
		return ERROR_FATAL;
	}

	if (timeout < 0) {
		/* no errno */
		warnx("timeout invalid: %f", timeout);
		return ERROR_FATAL;
	}
	if (float2timeval(timeout, &tv) < 0) {
		warnx("fail conversion from timeout values to timeval structure");
		return ERROR_FATAL;
	}

	if (type == SOCK_DGRAM) {
		if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			return ERROR_FATAL;
		}
	}
	else if (type == SOCK_STREAM) {
		if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			return ERROR_FATAL;
		}
	}
	else {
		warnx("unknown type: not SOCK_DGRAM.  not SOCK_STREAM");
		return ERROR_FATAL;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		return ERROR_FATAL;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
		return ERROR_FATAL;
	}

	if (connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		return ERROR_FATAL;
	}
	
	return sockfd;
}
