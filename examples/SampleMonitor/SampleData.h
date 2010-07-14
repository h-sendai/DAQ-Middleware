#ifndef SAMPLEDATA_H
#define SAMPLEDATA_H
struct sampleData {
  unsigned char magic;
  unsigned char format_ver;
  unsigned char module_num;
  unsigned char reserved;	
  unsigned int  data;
};

#endif
