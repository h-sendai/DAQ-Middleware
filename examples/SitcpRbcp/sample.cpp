#include <cstdio>
#include <iostream>

#include <daqmw/SitcpRbcp.h>

int write_sample()
{
    SitcpRbcp bcp;

    int address = 0x1ad;
    int length  = 1;
    unsigned char buf[length];

    buf[0] = 100;
    bcp.set_verify_mode();
    std::cerr << "trying to write" << std::endl;
    if (bcp.write_registers("192.168.10.16", address, length, buf) < 0) {
        std::cerr << "ERROR!" << std::endl;
    }

    return 0;
}

int read_sample()
{
    // read MAC address.
    // address map: http://research.kek.jp/people/uchida/technologies/SiTCP/doc/SiTCP.pdf
    // page 29.

    SitcpRbcp bcp;

    int address = 0xffffff12;
    int length  = 6;
    unsigned char mac[length];

    if (bcp.read_registers("192.168.10.16", address , sizeof(mac), mac, 100) < 0) {
        return 1;
    }

    for (int i = 0; i < length; i++) {
        printf("%02x %02x\n", address + i, mac[i]);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    read_sample();
    // write_sample();

    return 0;
}
