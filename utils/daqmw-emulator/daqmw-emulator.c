#include "daqmw-emulator.h"
#include "my_signal.h"

int debug   = 0;
int rate    = DEFAULT_BYTES_RATE;
int bufsize = DEFAULT_BUFSIZE;

struct timeval start_time;

void sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		;
	}
	return;
}

void usage(void)
{
	char *usage_message =
"usage: emulator [ ]\n"
"Options:\n"
"    -d            debug\n"
"    -v            verbose\n";

	fprintf(stderr, usage_message);
	return;
}

void print_priv_port_notice(int port)
{
	char *message;
	message = "You need root privilege to use port %d.\n"
			  "Use -p option to change listening port.\n";
	fprintf(stderr, message, port);
	return;
}
  
int main(int argc, char *argv[])
{
	char			   *ip_address = "";
	int					ch;
	int					listenfd, connfd;
	int					port;
	int					on = 1;
	int					hflag = 0;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	port    = DEFAULT_PORT;
	bufsize = DEFAULT_BUFSIZE;
	while( (ch = getopt(argc, argv, "b:dh:p:t:")) != -1) {
		switch(ch) {
			case 'b':
				bufsize = get_num(optarg);
				break;
			case 'd':
				debug = 1;
				break;
			case 'h':
				hflag = 1;
				ip_address = optarg;
				break;
			case 'p':
				port = strtol(optarg, NULL, 0);
				break;
			case 't':
				rate = get_num(optarg);
				break;
			case '?':
			default:
				break;
		}
	}
	
	argc -= optind;
	argv += optind;

	if (hflag) {
		if (inet_addr(ip_address) == INADDR_NONE) {
			errx(1, "invalid IP address");
		}
	}

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		err(1, "socket");
	}
	setsockopt(listenfd, SOL_SOCKET,  SO_REUSEADDR, &on, sizeof(on));
	/* XXX */
	/* setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY , &on, sizeof(on)); */

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_port        = htons(port);
	if (hflag) {
		servaddr.sin_addr.s_addr = inet_addr(ip_address);
	}
	else {
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
		int uid = getuid();
		if ( (errno == EACCES) && (port < 1024) && (uid != 0)) {
			print_priv_port_notice(port);
			exit(1);
		}
		else {
			err(1, "bind");
		}
	}

	if (listen(listenfd, LISTENQ) < 0) {
		err(1, "listen");
	}
	if (strlen(ip_address) > 0) {
		printf("Ready.  Listening on %s, port %d.  ", ip_address, port);
	}
	else {
		printf("Ready.  Listening on all IP address(es), port %d.  ", port);
	}
	printf("Waiting connection ...\n");
	
	my_signal(SIGCHLD, sig_chld);
	my_signal(SIGPIPE, SIG_IGN);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR) {
				continue;
			}
			else {
				err(1, "accept");
			}
		}

		if ( (childpid = fork()) < 0) {
			err(1, "fork");
		}

		if (childpid == 0) { /* child */
            if (close(listenfd) < 0) {
                err(1, "close (child)");
            }
			if (child_proc(connfd) < 0) {
				errx(1, "send_data fail");
			}
			exit(0);
		}
		else { /* parent */
            if (close(connfd) < 0) {
                err(1, "close (parent)");
            }
		}
	}
}
