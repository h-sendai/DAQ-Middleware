// -*- C++ -*-
/*!
 * @file Merger.h
 * @brief 複数のコンポーネントからデータを受け取り、一つのコンポーネントにデータを送る
 * @date 2014/11/12
 * @author E.Hamada
 */


#ifndef TINYSINK_H
#define TINYSINK_H

#include "DaqComponentBase.h"
#include <signal.h>
#include <arpa/inet.h>

typedef void    Sigfunc(int);

using namespace RTC;

/**
 * @brief  複数のコンポーネントからデータを受け取り、一つのコンポーネントにデータを送る
 */
class Merger
    : public DAQMW::DaqComponentBase
{
public:
    Merger(RTC::Manager* manager);
    ~Merger();

    // The initialize action (on CREATED->ALIVE transition)
    // former rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    static const int InPortNum = 3;                 //!< インポート数
    InPort<TimedOctetSeq>  *m_InPort[InPortNum];    //!< インポート
    TimedOctetSeq          *m_in_data[InPortNum];   //!< 各インポートがリードしたデータのバッファ
    
    OutPort<TimedOctetSeq> m_OutPort;               //!< アウトポート
    TimedOctetSeq          m_out_data;              //!< アウトポートのデータのバッファ



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
    unsigned int read_InPort(int PortNum);
    int set_data_full(unsigned int data_byte_size);
    unsigned int set_data_separate(unsigned int send_data);
    int write_OutPort();
    void data_send(unsigned int event_byte_size);

    static const unsigned int RECV_BUFFER_SIZE = 4096000;   //!< 1回のデータreadの最大データバイド数
    static const unsigned int PORT_BUFFER_SIZE = 2;
    static const unsigned int EVENTSIZE_BUFFER_SIZE = 4;

    /** @brief 各インポートのデータリードの結果. 成功すれば、BUF_SUCCESS。タイムアウトならば、BUF_TIMEOUT。致命的なエラーが発生したらBUF_FATAL */
    BufferStatus m_in_status[InPortNum];                    

    /** @brief データ送信の結果。成功すれば、BUF_SUCCESS。タイムアウトならば、BUF_TIMEOUT。致命的なエラーが発生したらBUF_FATAL */
    BufferStatus m_out_status;

    int m_in_get;                                           //!< リードしたインポートの番号
    int m_nextread_ID;                                      //!< リードしたインポートの番号.リードできるインポートがない場合、0になる 
    unsigned int m_recv_byte_size;                          //!< リードしたデータのサイズ 
    int m_stop_flag;                                        //!< stopが実行されたあとに、daq_runが実行された回数
    bool m_debug;                                           //!<  0--> 通常モード　1-->デバックモード。　コンフィグレーションファイルから設定
    
    
    bool            m_separate_flag;                        //!< 0-->受取ったデータをそのまま送る。1-->イベントごとに送る。コンフィグレーションファイルから設定 

    struct          timeval m_starttime;                    //!< start time  (use for debug)
    struct          timeval m_endtime;                      //!< end time (use for debug)


};


extern "C"
{
    void MergerInit(RTC::Manager* manager);
};


#endif // TINYSINK_H
