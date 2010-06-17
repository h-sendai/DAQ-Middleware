// -*- C++ -*-
/*!
 * @file FileUtils.h
 * @brief File operation utilities
 * @date
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2008
 *     Kazuo Nakayoshi
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 */

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

struct PsdFileInfo
{
    std::ofstream* file;
    std::string addr;
    std::string name;
    unsigned long long size;
    int no;
    char stream_buf[32*1024];
};

struct FileInfo
{
    std::ofstream* file;
    ///std::string addr;
    std::string name;
    unsigned long long size;
    int no;
    char stream_buf[32*1024];
};

class FileUtils
{
public:
    FileUtils();
    virtual ~FileUtils();

    int check_dir(std::string dir_name);
    int make_dir(std::string dir_prefix, std::string daq_name);
    void set_extension(std::string ext_name);
    void set_max_size(unsigned long long size);
    void set_max_size_in_MegaBytes(unsigned int size);
    void set_compress_flag(bool flag);
    void set_run_no(int no);
    //void set_inst_id(std::string inst_id);
    //void set_daq_id(std::string daq_id);
    int open_file();
    int open_file(std::string dir_name);
    int close_file();
    //int open_files(std::string inst_id, std::vector<std::string> name_list);
    //int close_files(bool flag);
    void write(char* text, unsigned long size);
    void write(int index, char* text, unsigned long size);

private:
    std::string gettime();
    int make_dir_loop(const char* name);
    int make_dir2(const char* name);
    std::string& replace(std::string& str, const std::string sb, 
			 const std::string sa);
    void new_name(int index, std::string name);
    void next_name(int index);
    std::string make_name();
    //std::string make_name(int index);
    //int open_file(int index);
    int close_file(int index);
    int remove_dir(bool flag);
    void rm_file(int index);

    //std::string m_inst_id;
    //std::string m_daq_id;
    int m_run_no;
    unsigned long long m_max_size;
    bool m_compress_flag;
    std::vector<FileInfo> m_file_info;
    int m_file_num;
    std::string m_ext_name;
    std::string m_dir_name;
    std::vector<std::string> m_dir_list;
    //std::map< int, int > m_modFileList; //map of modNo and output file
    bool m_auto_fname;
    bool m_debug;
};

#endif
