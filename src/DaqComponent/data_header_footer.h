#ifndef DAQ_HEADER_FOOTER_H
#define DAQ_HEADER_FOOTER_H

namespace DAQMW
{

        static const unsigned int  HEADER_BYTE_SIZE = 8;
        static const unsigned int  FOOTER_BYTE_SIZE = 8;
        static const unsigned char HEADER_MAGIC     = 0xe7;
        static const unsigned char FOOTER_MAGIC     = 0xcc;
        static const unsigned int  EVENT_BUF_OFFSET = HEADER_BYTE_SIZE;


        /**
         *  The data structure transferring between DAQ-Components is
         *  Header data(8bytes) + Event data(8bytes x num_of_events) + Footer data(8bytes).
         *
         *  Header data includes magic number(2bytes), and data byte size(4bytes) 
         *  except header and footer.
         *  Footer data includes magic number(2bytes), sequence number(4bytes).
         *
         *                dat[0]   dat[1]   dat[2]    dat[3]   dat[4]     dat[5]     dat[6]    dat[7]
         *  Header        0xe7     0xe7     reserved  reserved siz(24:31) siz(16:23) siz(8:15) siz(0:7)
         *  Event data1
         *  ...
         *  Event dataN
         *  Footer        0xcc     0xcc     reserved  reserved seq(24:31) seq(16:23) seq(8:15) seq(0:7)
         */

        int set_header(unsigned char* header, unsigned data_byte_size) {
            header[0] = HEADER_MAGIC;
            header[1] = HEADER_MAGIC;
            header[2] = 0;
            header[3] = 0;
            header[4] = (data_byte_size & 0xff000000) >> 24;
            header[5] = (data_byte_size & 0x00ff0000) >> 16;
            header[6] = (data_byte_size & 0x0000ff00) >>  8;
            header[7] = (data_byte_size & 0x000000ff);
            return 0;
        }

        int set_footer(unsigned char* footer, unsigned sequence_num) 
        {
            footer[0] = FOOTER_MAGIC;
            footer[1] = FOOTER_MAGIC;
            footer[2] = 0;
            footer[3] = 0;
            footer[4] = (sequence_num & 0xff000000) >> 24;
            footer[5] = (sequence_num & 0x00ff0000) >> 16;
            footer[6] = (sequence_num & 0x0000ff00) >>  8;
            footer[7] = (sequence_num & 0x000000ff);
            return 0;
        }

        bool check_header(unsigned char* header, unsigned received_byte) 
        {
            bool ret = false;

            if (header[0] == HEADER_MAGIC && header[1] == HEADER_MAGIC) {
                unsigned int event_size = (  header[4] << 24 ) 
                                         + ( header[5] << 16 )
                                         + ( header[6] <<  8 )
                                         +   header[7];
                if (received_byte == event_size) {
                    ret = true;
                }
                else {
                    std::cerr << "### ERROR: Event byte size missmatch" << std::endl;
                }
            }
            else {
                std::cerr << "### ERROR: Bad Magic Num:" 
                          << std::hex << (unsigned)header[0] << " " << (unsigned)header[1] << std::endl;
            }
            std::cerr << std::dec;
            return ret;
        }

        bool check_footer(unsigned char* footer, unsigned loop_cnt) 
        {
            bool ret = false;
            if (footer[0] == FOOTER_MAGIC && footer[1] == FOOTER_MAGIC) {
                unsigned int seq_num = (  footer[4] << 24 )
                                      + ( footer[5] << 16 )
                                      + ( footer[6] <<  8 )
                                      +   footer[7];
                if (seq_num == loop_cnt) {
                    ret = true;
                }
                else {
                    std::cerr << "### ERROR: Sequence No. missmatch" << std::endl;
                    std::cerr << "sequece no. in footer :" << seq_num << std::endl;
                    std::cerr << "loop cnts at component:" << loop_cnt << std::endl;
                }
            }
            return ret;
        }
}

#endif
