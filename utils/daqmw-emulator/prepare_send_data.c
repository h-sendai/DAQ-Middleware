#include "daqmw-emulator.h"

#define ONE_EVENT_BYTE_SIZE 8

/* write event data logic and fill the data to buf.
   RETURN VALUE: -1 on error
				 data length (in bytes) if succeed.
*/

/*
 Data Format
 +-----------+----------------+---------------+----------+------------+
 | Signature | Format Version | Module Number | Reserved | Event Data |
 +-----------+----------------+---------------+----------+------------+
    1 byte        1 byte            1 byte       1 byte      4 bytes
 Event Data is an positive inteter (Network byte order).
*/

typedef struct event_data_format_tag {
	char signature;
	char format_version;
	char module_num;
	char reserved;
	int  data;
} event_data_format;

extern double gaussian_rand(void);

int pack_data(char *bufpos, event_data_format *event_data)
{
	memcpy(&bufpos[0], &event_data->signature,      1);
	memcpy(&bufpos[1], &event_data->format_version, 1);
	memcpy(&bufpos[2], &event_data->module_num,     1);
	memcpy(&bufpos[3], &event_data->reserved,       1);
	memcpy(&bufpos[4], &event_data->data,           4);

	return 0;
}

int generate_data(int mod_num)
{
	double x;
	int rv;
	double sdev   = 5.0;
	double mean   = 100.0;
	double offset = 100.0;

	x = gaussian_rand();
	x = x*sdev + mean*(double)mod_num + offset;
	rv = (int) (1000.0 * x); 

	return htonl(rv);
}

int prepare_send_data(char *buf, int buflen)
{
	int i, num_events;
	event_data_format event_data;
	char *send_data_buf;
	int generated_event_bytes = 0;

	num_events = buflen / ONE_EVENT_BYTE_SIZE;
	send_data_buf = buf;
	for (i = 0; i < num_events; i ++) {
		event_data.signature      = 0x5a;
		event_data.format_version = 0x01;
		event_data.module_num     = (i % 8);
		event_data.reserved       = 0x00;
		event_data.data           = generate_data(event_data.module_num);

		pack_data(send_data_buf, &event_data);
		send_data_buf         += ONE_EVENT_BYTE_SIZE;
		generated_event_bytes += ONE_EVENT_BYTE_SIZE;
	}
	return generated_event_bytes;
}
