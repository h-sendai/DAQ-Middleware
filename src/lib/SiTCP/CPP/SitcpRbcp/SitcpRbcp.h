#ifndef SITCP_RBCP
#define SITCP_RBCP
/*
SiTCP RBCP Read, Write function 

Usage: 

////////////////////////////////////////////////////////////////////////
// Read Registers
////////////////////////////////////////////////////////////////////////

SitcpRbcp rbcp;

int address = 0x10;
int length  = 4;
unsigned char buf[length];

if (rbcp.read_registers("192.168.0.16", address, length, buf) < 0) {
    std::cerr << "read_registers() fail" << std::endl;
}

read result is written in buf.
Return value: = 0 if success.
              < 0 if failure.

You may specify SiTCP RBCP header id as last argument:
read_registers("192.168.0.16", address, length, buf, 100);
Default id value is 1.

////////////////////////////////////////////////////////////////////////
// Write Registers
////////////////////////////////////////////////////////////////////////

SitcpRbcp rbcp;

int address = 0x20;
int length  = 4;
unsigned char buf[length];
// Prepare write contents
buf[0] = 0;
buf[1] = 1;
buf[2] = 2;
buf[3] = 3;
if (rbcp.write_registers("192.168.0.16", address, length, buf) < 0) {
    std::cerr << "write_registers() fail" << std::endl;
}
 
Return value: = 0 if success.
              < 0 if failure.

You may use set_verify_mode() to re-read the registers
and compare the original write contents.

rbcp.set_verify_mode();
rbcp.write_registers("192.168.0.16", address, length, buf);

Even if you do not specify set_verify_mode(), write_registers()
examine the Ack packet data (but this does not mean the write
was successfully done).

You may specify SiTCP RBCP header id as last argument:
write_registers("192.168.0.16", address, length, buf, 100);
Default id value is 1.
*/ 

#include <string>
#include <iostream>

#include <err.h>
#include "Sock.h"

struct sitcp_rbcp_header {
    unsigned char ver_type;
    unsigned char cmd_flag;
    unsigned char id;
    unsigned char length;
    unsigned int  address;
};

class SitcpRbcp {

public:
    SitcpRbcp() :
    m_need_verify(0) {}
    ~SitcpRbcp() {}
    int read_registers(
        std::string ip_address,
        int address,
        int length,
        unsigned char *buf,
        int id = 1
    );
    int write_registers(
        std::string ip_address,
        int address,
        int length,
        unsigned char *buf,
        int id = 1
    );
    int set_verify_mode();
    int unset_verify_mode();

private:
    DAQMW::Sock m_sock;
    int pack_sitcp_rbcp_header(unsigned char *buf, struct sitcp_rbcp_header *header);
    int unpack_sitcp_rbcp_header(unsigned char *buf, struct sitcp_rbcp_header *header);
    int print_packet_error_message(int ret, std::string function_name, std::string ip_address);
    int send_recv_command_packet(int command, std::string ip_address, int address, int length, unsigned char *buf, int id);
    int m_need_verify;
    const static int SITCP_RBCP_HEADER_LEN    = 8;
    const static unsigned char ACK_MASK       = 0x08;
    const static unsigned char BUS_ERROR_MASK = 0x01;
    const static int SITCP_RBCP_PORT          = 4660;
    const static int READ                     = 1;
    const static int WRITE                    = 2;

};
#endif
