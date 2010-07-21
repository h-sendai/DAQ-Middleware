// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include "SampleMonitor.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::HEADER_DATA_MISMATCH;
using DAQMW::FatalType::FOOTER_DATA_MISMATCH;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* samplemonitor_spec[] =
{
    "implementation_id", "SampleMonitor",
    "type_name",         "SampleMonitor",
    "description",       "SampleMonitor component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

SampleMonitor::SampleMonitor(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("samplemonitor_in",   m_in_data),
      m_in_status(BUF_SUCCESS),

      m_canvas(0),
      m_hist(0),
      m_bin(0),
      m_min(0),
      m_max(0),
      m_monitor_update_rate(10),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort ("samplemonitor_in",  m_InPort);

    init_command_port();
    init_state_table();
    set_comp_name("SAMPLEMONITOR");

    //init histograms
    m_hist = new TH1F* [CH_NUM];
    for (int i = 0; i < CH_NUM; i++) {
        m_hist[i] = 0;
    }
}

SampleMonitor::~SampleMonitor()
{
}

RTC::ReturnCode_t SampleMonitor::onInitialize()
{
    if (m_debug) {
        std::cerr << "SampleMonitor::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t SampleMonitor::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int SampleMonitor::daq_dummy()
{
    if (m_canvas) {
        m_canvas->Update();
    }

    return 0;
}

int SampleMonitor::daq_configure()
{
    std::cerr << "*** SampleMonitor::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int SampleMonitor::parse_params(::NVList* list)
{

    std::cerr << "param list length:" << (*list).length() << std::endl;

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

        std::cerr << "sname: " << sname << "  ";
        std::cerr << "value: " << svalue << std::endl;
    }

    return 0;
}

int SampleMonitor::daq_unconfigure()
{
    std::cerr << "*** SampleMonitor::unconfigure" << std::endl;
    if (m_canvas) {
        delete m_canvas;
        m_canvas = 0;
    }

    for (int i = 0; i < CH_NUM; i++){
        if (m_hist[i]) {
            delete m_hist[i];
            m_hist[i] = 0;
        }
    }
    return 0;
}

int SampleMonitor::daq_start()
{
    std::cerr << "*** SampleMonitor::start" << std::endl;

    m_in_status  = BUF_SUCCESS;

    //////////////// CANVAS FOR HISTOS ///////////////////
    if (m_canvas) {
        delete m_canvas;
        m_canvas = 0;
    }
    m_canvas = new TCanvas("c1", "histos", 0, 0, 800, 400);
    m_canvas->Divide(4,2);

    ////////////////       HISTOS      ///////////////////
    for (int i = 0; i < CH_NUM; i++){
        if (m_hist[i]) {
            delete m_hist[i];
            m_hist[i] = 0;
        }
    }

    int m_hist_bin = 100;
    double m_hist_min = 0.0;
    double m_hist_max = 1000.0;

    gStyle->SetStatW(0.4);
    gStyle->SetStatH(0.2);
    gStyle->SetOptStat("em");

    for (int i = 0; i < CH_NUM; i++) {
        m_hist[i] = new TH1F(Form("h%d", i), Form("h#%d",i), m_hist_bin,
                             m_hist_min, m_hist_max);
        m_hist[i]->GetXaxis()->SetNdivisions(5);
        m_hist[i]->GetYaxis()->SetNdivisions(4);
        m_hist[i]->GetXaxis()->SetLabelSize(0.07);
        m_hist[i]->GetYaxis()->SetLabelSize(0.06);
    }
    std::cerr << "daq_start exit" << std::endl;
    return 0;
}

int SampleMonitor::daq_stop()
{
    std::cerr << "*** SampleMonitor::stop" << std::endl;
    reset_InPort();

    return 0;
}

int SampleMonitor::daq_pause()
{
    std::cerr << "*** SampleMonitor::pause" << std::endl;

    return 0;
}

int SampleMonitor::daq_resume()
{
    std::cerr << "*** SampleMonitor::resume" << std::endl;

    return 0;
}

int SampleMonitor::reset_InPort()
{
    int ret = true;
    while(ret == true) {
        ret = m_InPort.read();
    }

    return 0;
}

int SampleMonitor::decode_data(struct sampleData* mydata, int size)
{
    for (int i = 0; i < size/(int)sizeof(struct sampleData); i++) {
        if (m_debug) {
            std::cerr << "mydata.magic: " << std::hex << (int)mydata->magic
                      << std::endl;
            std::cerr << "mydata.format_ver: " << std::hex
                      << (int)mydata->format_ver << std::endl;
            std::cerr << "mydata.module_num: " << std::hex
                      << (int)mydata->module_num << std::endl;
            std::cerr << "mydata.reserved: " << (int)mydata->reserved
                      << std::endl;
            std::cerr << "mydata.data: " << std::dec
                      << (int)ntohl(mydata->data)  << std::endl;
        }

        float fdata = ntohl(mydata->data)/1000.0; // 1000 times value is received
        m_hist[(int)mydata->module_num]->Fill(fdata);

        mydata++;
    }
    return 0;
}

unsigned int SampleMonitor::read_InPort()
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = m_InPort.read();

    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL
        m_in_status = check_inPort_status(m_InPort);
        if (m_in_status == BUF_TIMEOUT) { // Buffer empty.
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }
        else if (m_in_status == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else {
        recv_byte_size = m_in_data.data.length();
    }
    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

int SampleMonitor::daq_run()
{
    if (m_debug) {
        std::cerr << "*** SampleMonitor::run" << std::endl;
    }

    unsigned int recv_byte_size = read_InPort();
    if (recv_byte_size == 0) { // Timeout
        return 0;
    }

    check_header_footer(m_in_data, recv_byte_size); // check header and footer
    unsigned int event_byte_size = get_event_size(recv_byte_size);
    //std::cerr << "event_byte_size:" << event_byte_size << std::endl;

    /////////////  Write component main logic here. /////////////
    // online_analyze();
    unsigned char mydata[1024];
    memcpy(&mydata[0], &m_in_data.data[HEADER_BYTE_SIZE], event_byte_size);

    decode_data((struct sampleData*)&mydata[0], event_byte_size);

    if((m_loop % m_monitor_update_rate) == 0) {
        for (int i = 0; i < 8; i++) {
            m_canvas->cd(i + 1);
            m_hist[i]->Draw();
        }
        m_canvas->Update();
    }
    /////////////////////////////////////////////////////////////

    m_loop++;                            // increase sequence num.
    m_total_size += event_byte_size;     // increase total data byte size

    return 0;
}

extern "C"
{
    void SampleMonitorInit(RTC::Manager* manager)
    {
        RTC::Properties profile(samplemonitor_spec);
        manager->registerFactory(profile,
                    RTC::Create<SampleMonitor>,
                    RTC::Delete<SampleMonitor>);
    }
};
