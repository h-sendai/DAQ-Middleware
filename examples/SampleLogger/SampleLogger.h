// -*- C++ -*-
/*!
 * @file SampleLogger.h
 * @brief Event data logging component.
 * @date
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2011
 *     Kazuo Nakayoshi
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 */

#ifndef SAMPLELOGGER_H
#define SAMPLELOGGER_H

#include "DaqComponentBase.h"
#include "FileUtils.h"

using namespace RTC;

class SampleLogger
    : public DAQMW::DaqComponentBase
{
public:
    SampleLogger(RTC::Manager* manager);
    virtual ~SampleLogger();

    // The initialize action (on CREATED->ALIVE transition)
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    RTC::TimedOctetSeq m_in_data;
    ///InPort<TimedOctetSeq, MyRingBuffer> m_InPort;
    RTC::InPort< RTC::TimedOctetSeq > m_InPort;

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
    int reset_InPort();
    void toLower(std::basic_string<char>& s);

    FileUtils* fileUtils;
    bool m_isDataLogging;
    bool m_filesOpened;
    std::string m_dirName;
    unsigned int m_maxFileSizeInMByte;
    BufferStatus m_in_status;
    int m_update_rate;
    bool m_debug;
};

extern "C"
{
    void SampleLoggerInit(RTC::Manager* manager);
};

#endif // SAMPLELOGGER_H
