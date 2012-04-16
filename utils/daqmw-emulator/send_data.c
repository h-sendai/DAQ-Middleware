#include "daqmw-emulator.h"

long long so_far_bytes = 0;

int send_data(int sockfd)
{
	char *buf;
	struct timeval start_time, current_time, time_diff;
	double delta_t_sec;
	int n;
	int delta_t_usec;
	int data_len;
	int sleep_count    = 0;
	int no_sleep_count = 0;

	if (debug) {
		fprintf(stderr, "send_data() start\n");
		fprintf(stderr, "bufsize: %d\n", bufsize);
	}
	if ( (buf = malloc(bufsize)) == NULL) {
		err(1, "malloc");
	}
	memset(buf, 0, bufsize);

	if (gettimeofday(&start_time, NULL) < 0) {
		err(1, "gettimeofday of the start_time");
	}

	for ( ; ; ) {
		if (gettimeofday(&current_time, NULL) < 0) {
			err(1, "gettimeofday in send_data");
		}
		timersub(&current_time, &start_time, &time_diff);
		delta_t_sec = ( (double) so_far_bytes / (double) rate)
					- (double) time_diff.tv_sec - (double) time_diff.tv_usec * 0.000001;
		if (delta_t_sec > 0.0) {
			if (debug) {
				sleep_count ++;
			}
			delta_t_usec = (int) (delta_t_sec * 1000000.0);
			usleep(delta_t_usec);
		}
		else {
			if (debug) {
				no_sleep_count ++;
			}
		}

		if ( (data_len = prepare_send_data(buf, bufsize)) < 0) {
			errx(1, "prepare_send_data fail");
		}

		n = write(sockfd, buf, data_len);
		if (n < 0) {
			if (errno == ECONNRESET) {
				warnx("connection reset client");
				break;
			}
			else if (errno == EPIPE) {
				warnx("connection closed by client");
				break;
			}
			else {
				err(1, "write");
			}
		}
		else if (n > 0) {
			so_far_bytes += n;
		}
		else { /* n == 0 */
			err(1, "n == 0");
		}
	}
	if (debug) {
		fprintf(stderr, "sleep_count: %d , no_sleep_count: %d\n",
			sleep_count, no_sleep_count);
	}
		
	return 0;
}

int child_proc(int sockfd)
{
	struct timeval seed;
	if (gettimeofday(&seed, NULL) < 0) {
		perror("gettimeofday for seed");
		return -1;
	}
	srand(seed.tv_sec + seed.tv_usec);

	/* for example, we can implenent prepare() to sending data here. */

	if (send_data(sockfd) < 0) {
		return -1;
	}

	return 0;
}
