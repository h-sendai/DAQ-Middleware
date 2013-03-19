#ifndef _DAQMW_EMULATOR_H
#define _DAQMW_EMULATOR_H 1

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "get_num.h"

extern int child_proc(int sockfd);
extern int prepare_send_data(char *buf, int buflen);
extern int rate;
extern int bufsize;
extern int debug;
extern long long so_far_bytes;
extern int no_random_data;

#define DEFAULT_PORT       2222
#define DEFAULT_BUFSIZE    1024
#define DEFAULT_BYTES_RATE 8 * 1024
#define LISTENQ            10
#define SA                 struct sockaddr

#endif /* _DAQMW_EMULATOR_H */
