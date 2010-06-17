// -*- C++ -*-
/*!
 * @file EchoReader.h
 * @brief EchoReader component. 
 * @date 2009/07/17
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2009
 *     Kazuo Nakayoshi
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 */

#ifndef ECHOREADER_H
#define ECHOREADER_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include "DaqComponentBase.h"

// Service implementation headers
#include "DAQServiceSVC_impl.h"
#include "Sock.h"

using namespace RTC;

class EchoReader
    : public DAQMW::DaqComponentBase
{
public:
    EchoReader(RTC::Manager* manager);
    ~EchoReader();

    // The initialize action (on CREATED->ALIVE transition)
    // formaer rtc_init_entry() 
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    ///TimedOctetSeq m_in_data;
    ///InPort<TimedOctetSeq, MyRingBuffer> m_InPort;
  
    TimedOctetSeq m_out_data;
    OutPort<TimedOctetSeq> m_OutPort;

private:
    int daq_dummy();
    int daq_configure();
    int daq_unconfigure();
    int daq_start();
    int daq_run();
    int daq_stop();
    int daq_pause();
    int daq_resume();

    int parse_params(::NVList* list);

    unsigned int getGauss();
    int send_data_to_echoServer();
    int recv_data_from_echoServer();
    int set_data_to_OutPortBuf(unsigned int seq_num);
    //int check_outPort_status(int status);

    BufferStatus m_out_status;

    DAQMW::Sock* m_sock;   
    int m_dataByteSize;    
    unsigned char* m_wdata;
    unsigned char* m_rdata;
    std::string m_srcAddr;
    int         m_srcPort;

    bool m_debug;
    //static const std::string HOSTADDR;
    //static const int PORTNO = 7; // echo(tcp) server port no.
};

extern "C"
{
  void EchoReaderInit(RTC::Manager* manager);
};

#endif // ECHOREADER_H
