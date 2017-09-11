// -*- C++ -*-
/*!
 * @file Merger.cpp
 * @brief 複数のコンポーネントからデータを受け取り、一つのコンポーネントにデータを送る
 * @date 2014/11/12
 * @author E.Hamada
 *
 */

#include "Merger.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::HEADER_DATA_MISMATCH;
using DAQMW::FatalType::FOOTER_DATA_MISMATCH;
using DAQMW::FatalType::USER_DEFINED_ERROR1;


// Module specification
static const char* tinysink_spec[] =
{
    "implementation_id", "Merger",
    "type_name",         "Merger",
    "description",       "Merger component",
    "version",           "1.0",
    "vendor",            "Eitaro Hamada, KEK",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};


/**
 * @brief コンストラクタ. インポート、アウトポートの設定を行っている
 *
 * ヘッダファイルで設定したInPortNum の数だけ、インポートを用意する。
 */
Merger::Merger(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("merger_out", m_out_data),
      m_out_status(BUF_SUCCESS),
      m_in_get(-1),
      m_nextread_ID(0),
      m_recv_byte_size(0),
      m_stop_flag(0),
      m_debug(false),
      m_separate_flag(false)
{
    // Registration: InPort/OutPort/Service
    // Set InPort buffers
    char InPortName[256];
    for(int i = 0; i < InPortNum; i++){
        m_in_data[i] = new TimedOctetSeq;
        sprintf(InPortName, "merger_in%d", i);
        m_InPort[i] = new InPort<TimedOctetSeq>(InPortName, *m_in_data[i]);
        registerInPort (InPortName, *m_InPort[i]);
    }

    //Set OutPort
    registerOutPort("merger_out", m_OutPort);

    for(int i = 0; i < (int)InPortNum; i++){
        m_in_status[i] = BUF_SUCCESS;
    }

    init_command_port();
    init_state_table();
    set_comp_name("Merger");
}



/**
 * @brief デストラクタ.終了時にインポートの削除を行う
 */
Merger::~Merger()
{

    delete [] m_in_data;
    delete [] m_InPort;

}




RTC::ReturnCode_t Merger::onInitialize()
{
    if (m_debug) {
        std::cerr << "Merger::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t Merger::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

/**
 * @brief LOADED, CONFIGURED, PAUSED状態の時に、繰り返し実行.特に何もしない
 */
int Merger::daq_dummy()
{
    return 0;
}


/**
 * @brief configureが実行された場合の処理. 
 *
 * parse_paramsを呼び、コンフィグレーションファイルからパラメータを取得する
 */

int Merger::daq_configure()
{
    std::cerr << "*** Merger::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

/**
 * @brief コンフィグレーションファイルから、各パラメータを読み込む
 *
 * ～Specifiedは、初めは、falseになっている。、パラメータを適切に読み込むことができると、 \n
 * trueになり、最後のif文でエラーにならずに通過できる。
 */
int Merger::parse_params(::NVList* list)
{

    std::cerr << "param list length:" << (*list).length() << std::endl;

    bool separate_flagSpecified = false;


    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

        std::cerr << "sname: " << sname << "  ";
        std::cerr << "value: " << svalue << std::endl;

        if ( sname == "separate_flag" ) {
            separate_flagSpecified = true;
            char *offset;
            m_separate_flag = (bool)strtol(svalue.c_str(), &offset, 10);
        }
        
    }

    if (!separate_flagSpecified) {
        std::cerr << "### ERROR:separate_flagnot specified\n";
        fatal_error_report(USER_DEFINED_ERROR1, "NO SEPARATE_FLAG");
    }



    return 0;
}

/**
 * @brief unconfigureが実行された場合の処理. 特に何もしない
 *
 */
int Merger::daq_unconfigure()
{
    std::cerr << "*** Merger::unconfigure" << std::endl;

    return 0;
}

/**
 * @brief startが実行された時の処理
 *
 * インポートのデフォルト設定やアウトポートのコネクションのチェックを行う
 */

int Merger::daq_start()
{
    std::cerr << "*** Merger::start" << std::endl;

    for(int i = 0; i < (int)InPortNum; i++){
        m_in_status[i]  = BUF_SUCCESS;
    }


    // Check data port connections
    bool outport_conn = check_dataPort_connections( m_OutPort );
    if (!outport_conn) {
        std::cerr << "### NO Connection" << std::endl;
        fatal_error_report(DATAPATH_DISCONNECTED);
    }


    gettimeofday(&m_starttime, NULL);

    return 0;
}


/**
 * @brief stopが実行された場合の処理.
 *
 * reset_InPortを実行
 */
int Merger::daq_stop()
{
    std::cerr << "*** Merger::stop" << std::endl;
    reset_InPort();

    return 0;
}

/**
 * @brief pauseが実行された場合の処理.特に何もしない
 */
int Merger::daq_pause()
{
    std::cerr << "*** Merger::pause" << std::endl;

    return 0;
}

/**
 * @brief resumeが実行された場合の処理.特に何もしない
 */
int Merger::daq_resume()
{
    std::cerr << "*** Merger::resume" << std::endl;

    return 0;
}

/**
 * @brief インポートに格納されたデータを読み込む
 */
int Merger::reset_InPort()
{
    int ret = true;
    for(int i = 0; i < InPortNum; i++){
        while(ret == true) {
            ret = m_InPort[i]->read();
        }
    }


    return 0;
}




/**
 * @brief 一つのインポートからデータ読み込みを行う
 *
 * @param PortNum インポート番号
 * @param recv_byte_size 読み込んだデータのサイズ
 * @param ret データ受信に成功した場合はtrue、失敗した場合はfalse
 *
 * 選択されたインポート（引数のPortNumで）に送られたデータを読み込む。
 * データを受け取ることができたら、m_in_data[PortNum]にデータを書き込み、読み込んだデータのサイズを返す \n
 * データが送られてきていないなどで、受け取ることができなかったら、何もせず0を返す。
 */
unsigned int Merger::read_InPort(int PortNum)
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = false;

    ret = m_InPort[PortNum]->read();



    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL

        m_in_status[PortNum] = check_inPort_status(*m_InPort[PortNum]);


        if (m_in_status[PortNum] == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else {  // SUCCESS
        m_in_status[PortNum] = BUF_SUCCESS;

        recv_byte_size = m_in_data[PortNum]->data.length();
    }

    return recv_byte_size;
}

/**
 * @brief 次のコンポーネントに送るデータをsetする。
 * @param [in]  data_byte_size データのサイズ
 * @param header[8] ヘッダのバッファ
 * @param footer[8] フッターのバッファ
 *
 * 受取ったデータをそのまま次のコンポーネントに送る
 */
int Merger::set_data_full(unsigned int data_byte_size)
{
    m_out_data.data.length(HEADER_BYTE_SIZE + data_byte_size + FOOTER_BYTE_SIZE);
    memcpy(&(m_out_data.data[0]), &(m_in_data[m_in_get]->data[0]), HEADER_BYTE_SIZE + data_byte_size + FOOTER_BYTE_SIZE);

    //set footer
    unsigned char footer[8];
    set_footer(&footer[0]);
    
    memcpy(&(m_out_data.data[data_byte_size + FOOTER_BYTE_SIZE]), &footer[0], FOOTER_BYTE_SIZE);


    return 0;
}


/**
 * @brief 次のコンポーネントに送るデータをsetする。
 * @param [in]  send_data m_out_data.dataに格納したデータのバイト数（ヘッダは含めない)
 * @param header[8] ヘッダのバッファ
 * @param footer[8] フッターのバッファ
 *
 * m_in_dataに格納されているデータのうち、対象のイベントのみを抜き出し、m_out_data.dataにセットしている。 <br>
 * 
 */
unsigned int Merger::set_data_separate(unsigned int send_data)
{
    unsigned int next_send_data = send_data;

    //get port
    union u_port_f{
        unsigned char port_char[2];
        unsigned short port_short;
    }u_port;

    u_port.port_char[0] = m_in_data[m_in_get]->data[HEADER_BYTE_SIZE + send_data];
    u_port.port_char[1] = m_in_data[m_in_get]->data[HEADER_BYTE_SIZE + send_data + 1];
    //unsigned short port = ntohs(u_port.port_short);

    //get event size
    union u_eventsize_f{
        unsigned char eventsize_char[4];
        unsigned int eventsize_int;
    }u_eventsize;

    u_eventsize.eventsize_char[0] = m_in_data[m_in_get]->data[HEADER_BYTE_SIZE + send_data + PORT_BUFFER_SIZE];
    u_eventsize.eventsize_char[1] = m_in_data[m_in_get]->data[HEADER_BYTE_SIZE + send_data + PORT_BUFFER_SIZE + 1];
    u_eventsize.eventsize_char[2] = m_in_data[m_in_get]->data[HEADER_BYTE_SIZE + send_data + PORT_BUFFER_SIZE + 2];
    u_eventsize.eventsize_char[3] = m_in_data[m_in_get]->data[HEADER_BYTE_SIZE + send_data + PORT_BUFFER_SIZE + 3];
    unsigned int event_size = ntohl(u_eventsize.eventsize_int);

    //set length
    m_out_data.data.length(HEADER_BYTE_SIZE + event_size + FOOTER_BYTE_SIZE);

    //set header    
    unsigned char header[8];
    set_header(&header[0], event_size);
    memcpy(&header[2], &u_port.port_short, PORT_BUFFER_SIZE);
    memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);

    //set data
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), 
            &(m_in_data[m_in_get]->data[HEADER_BYTE_SIZE + send_data + PORT_BUFFER_SIZE + EVENTSIZE_BUFFER_SIZE]), 
            event_size);
    next_send_data += PORT_BUFFER_SIZE + EVENTSIZE_BUFFER_SIZE + event_size;

    //set footer
    unsigned char footer[8];
    set_footer(&footer[0]);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + event_size]), &footer[0], FOOTER_BYTE_SIZE);

    return event_size;
}



/**
 * @brief set_dataでセットされたデータを次のコンポーネントへ送る
 * @param ret データ送信に成功した場合はtrue、失敗した場合はfalse
 *
 * タイムアウトの場合、m_out_statusがBUF_TIMEOUTとなる\n
 * データ送信がうまくいったら、m_out_statusをBUF_SUCCESSにし、\n
 * シーケンスや、トータルデータ処理量の更新を行なう
 */
int Merger::write_OutPort()
{
    ////////////////// send data from OutPort  //////////////////
    bool ret = m_OutPort.write();

    //////////////////// check write status /////////////////////
    if (ret == false) {  // TIMEOUT or FATAL
        m_out_status  = check_outPort_status(m_OutPort);
        if (m_out_status == BUF_FATAL) {   // Fatal error
            fatal_error_report(OUTPORT_ERROR);
        }
        if (m_out_status == BUF_TIMEOUT) { // Timeout

            if (m_debug) {
                std::cerr << "TIMEOUT" << std::endl;
            }

            return -1;
        }
    }
    else {
        m_out_status = BUF_SUCCESS; // successfully done
    }

    return 0;
}

/**
 * @brief データ送信処理
 *
 * timeout することなく、データを送ることができるまでfor文でまわり続けている。
 */
void Merger::data_send(unsigned int event_byte_size){


    //case of separete
    if(m_separate_flag == true){
        unsigned int send_data = 0;
        unsigned int one_send_data_size = 0;

        for(;;){
            //zenbu yonndara owari
            if(send_data >= event_byte_size){
                break;
            }

            //set data
            one_send_data_size = set_data_separate(send_data);

            //send data
            for(;;){
                if (write_OutPort() < 0) {  //Time Out
                    continue;
                }
                else{
                    m_out_status = BUF_SUCCESS;
                    inc_sequence_num();                     // increase sequence num.
                    inc_total_data_size(one_send_data_size);  // increase total data byte size
                    break;
                }
            }

            send_data += one_send_data_size + PORT_BUFFER_SIZE + EVENTSIZE_BUFFER_SIZE;
        }
    }
    //case of full 
    else {
        //set data
        set_data_full(event_byte_size);

        //send data
        for(;;){
            if (write_OutPort() < 0) {  //Time Out
                continue;
            }
            else{
                m_out_status = BUF_SUCCESS;
                inc_sequence_num();                     // increase sequence num.
                inc_total_data_size(event_byte_size);  // increase total data byte size
                break;
            }
        }
    }

    return;
}




/**
 * @brief run状態の間、繰り返し実行.データのreadと送信処理を行う。
 * @param event_byte_size 1回のリードでのデータサイズ
 *
 * 初めのfor文で、複数のInPortを順々にデータをreadする。リードできたら、リードを行ったインポートのm_in_status[PortNum]がBUF_SUCCESSになり、for文を抜け出す。 \n
 * （リードを行ったインポート以外のm_in_status[PortNum]はBUF_SUCCESSでない）\n
 * 次に、set_data(event_byte_size)でwriteするデータをsetする。 \n
 * ２つめのfor文で、データ送信処理をwrite_OutPort関数から行う。\n
 * for文を利用する理由は、前回のdaq_runにて、データ送信を行い際、TIMEOUTが発生し、再度送る必要ある場合に対応するため。
 * \n
 * リードしたインポートの番号をm_in_getに保存している。次のデータを読むとき（つまり、次回のdaq_runの一回目のfor文）、m_in_get(=m_nextread_ID)を利用して、 \n
 * 前回読み込んだインポートの次のインポートからreadできるかようにしている。\n
 * これは、一つ目のfor文で、リードを行うインポートがかたよらないための処理である。\n
 * \n
 * \n
 * if(m_in_get==-1){ \n
 *   m_nextread_ID = -1; \n
 * } \n
 *　は、データを読み込むことができるインポートがなかったときに、次のdaq_runで1番目のインポートからreadを行うために必要\n
 * \n
 *  if (m_in_get==-1){ \n
 *        if (check_trans_lock()) {     // Check if stop command has come. \n
 * は、ストップコマンドが実行された時に、リングバッファに格納されたデータを読み込むこために必要な処理
 */
int Merger::daq_run()
{
    if (m_debug) {
        std::cerr << "*** Merger::run" << std::endl;
    }

//    if (check_trans_lock()) {     // Check if stop command has come.
//        set_trans_unlock();       // Transit to CONFIGURE state.
//    }



    unsigned int event_byte_size = 0;

    /////////// InPort //////////
    if(m_out_status != BUF_TIMEOUT)
    {
        for (int PortNum = m_nextread_ID + 1; PortNum < InPortNum; PortNum++){ 
            m_recv_byte_size = read_InPort(PortNum);

            if (m_recv_byte_size != 0) { // no Timeout
 
                event_byte_size = get_event_size(m_recv_byte_size);
        
                if (event_byte_size > RECV_BUFFER_SIZE) {
                    fatal_error_report(USER_DEFINED_ERROR1, "Length Too Large");
                }
        
                m_in_get = PortNum;
                break;


            }

        }


        if(m_in_get==-1){
            m_nextread_ID = -1;
        }


        if (m_in_get==-1){
            if (check_trans_lock()) {     // Check if stop command has come.
                m_stop_flag++;
                if(m_stop_flag > 100)set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }

        //case of TIMEOUT 
        if (m_in_get==-1) return 0; 

        //set_data(event_byte_size);

    }

    //for debug
    if(m_in_get == 0){
        gettimeofday(&m_endtime, NULL);
    }

    for(int PortNum = 0; PortNum < InPortNum; PortNum++){
        if(m_in_status[PortNum] != BUF_SUCCESS){
            continue;
        }

        data_send(event_byte_size);
        m_nextread_ID = m_in_get;
        m_in_get = -1;

        break;
    }

    return 0;
}

extern "C"
{
    void MergerInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tinysink_spec);
        manager->registerFactory(profile,
                    RTC::Create<Merger>,
                    RTC::Delete<Merger>);
    }
};
