// -*- C++ -*-
/*!
 * @file FileUtils.cpp
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

#include <stdio.h>
#include "FileUtils.h"

FileUtils::FileUtils()
    : m_run_no(0), m_max_size(0), m_compress_flag(false), 
      m_file_num(0), m_ext_name("dat"), m_dir_name(""), 
      m_auto_fname(false), m_debug(false)
{
    if (m_debug) {
	std::cerr << "FileUtils create\n";
    }
}

FileUtils::~FileUtils()
{
    if (m_debug) {
	std::cerr << "FileUtils deleted\n";
    }
    m_file_info.clear();
}

int FileUtils::check_dir(std::string dir_name)
{
    int ret = 0;
    DIR *dir;

    if ((dir = opendir(dir_name.c_str() )) == NULL) {
        std::cerr << "### FileUtils: could not open directory\n";
        //perror ("Cannot open .");
        ret = -1;
    }

    return ret;
}

int FileUtils::make_dir(std::string dir_prefix, std::string daq_name)
{
    int ret = 0;

    std::stringstream str_stream;
    str_stream << std::setw(6) << std::setfill('0') << m_run_no;

    m_dir_name = dir_prefix + "/" 
	///+ daq_name 
	+ str_stream.str() 
	+ "_" 
	+ gettime();

    std::cerr << "dirname: " << m_dir_name << std::endl;

    m_dir_list.clear();

    ret = mkdir(m_dir_name.c_str(), 
		S_IRUSR|S_IWUSR|S_IXUSR|
		S_IRGRP|S_IXGRP|
		S_IROTH|S_IXOTH);

    if (ret != 0) {
	std::cerr << "### ERROR: FileUtils: could not make a directory!!!\n";
    }
    else {
	ret = chdir(m_dir_name.c_str());
	if (ret!= 0)
	    std::cerr << "### ERROR: FileUtils: could not change directory!!!\n";
    }

    return ret;
}

void FileUtils::set_extension(std::string ext_name)
{
    m_ext_name = ext_name;
}

void FileUtils::set_max_size(unsigned long long size)
{
    m_max_size = size;
}

void FileUtils::set_max_size_in_MegaBytes(unsigned int size)
{
    m_max_size = (unsigned long long) size*1024*1024;
    std::cerr << "set max size in bytes:" << m_max_size << std::endl;
}

void FileUtils::set_compress_flag(bool flag)
{
    m_compress_flag = flag;
}

void FileUtils::set_run_no(int no)
{
    if( no <= 0 ) {
	m_auto_fname = true;
    }
    else {
	m_auto_fname = false;
	m_run_no = no;
    }
}

#ifdef PSD
void FileUtils::set_daq_id(std::string daq_id)
{
    m_daq_id = daq_id;
}

int FileUtils::close_files(bool flag)
{
    for(int i = 0; i < m_file_num; i++) {
	close_file(i);
    }

    remove_dir(flag);

    for(int i=0; i< m_file_num; i++) {
	delete m_file_info[i].file;
    }

    ///m_file_info.clear();
    return 0;
}
#endif

void FileUtils::write(char* text, unsigned long size)
{
    int findex = 0;
    if (m_debug) {
	std::cerr << "findex = " << findex << std::endl;
    }
    m_file_info[findex].file->write(text, size);
    //m_file_info[findex].file->flush();

    m_file_info[findex].size += size;

    if (m_debug) {
	std::cerr << "m_max_size:" << m_max_size << std::endl;
	std::cerr << "m_file_info[findex].size:" 
		  << m_file_info[findex].size << std::endl;
    }

    if ( (0 < m_max_size) && (m_max_size < m_file_info[findex].size)) {
	close_file(findex);
	next_name(findex);
	open_file(m_dir_name);
    }
}

#ifdef PSD
void FileUtils::write(int modIndex, char* text, unsigned long size)
{
    int findex = 0;
    if (m_debug) {
	std::cerr << "findex = " << findex << std::endl;
    }
    m_file_info[findex].file->write(text, size);
    //m_file_info[findex].file->flush();

    m_file_info[findex].size += size;

    if (m_debug) {
	std::cerr << "m_max_size:" << m_max_size << std::endl;
	std::cerr << "m_file_info[findex].size:" 
		  << m_file_info[findex].size << std::endl;
    }

    if (0 < m_max_size && m_max_size < m_file_info[findex].size) {
	close_file(findex);
	next_name(findex);
	open_file(findex);
    }
}
#endif

std::string FileUtils::gettime()
{
    struct tm *time_ptr;
    time_t now;

    time(&now);
    time_ptr = localtime(&now);

    std::stringstream str_stream;
    std::stringstream year;
    std::stringstream month;
    std::stringstream day;
    std::stringstream hour;
    std::stringstream min;
    std::stringstream sec;

#ifndef ORG
    year << time_ptr->tm_year+1900;
#else
    if (time_ptr->tm_year+1900-2000 < 10) {
	year << 0 << time_ptr->tm_year+1900-2000;
    }
    else {
	year << time_ptr->tm_year+1900-2000;
    }
#endif

    if (time_ptr->tm_mon+1 < 10)
	month << 0 << time_ptr->tm_mon+1;
    else
	month << time_ptr->tm_mon+1;

    if (time_ptr->tm_mday < 10)
	day << 0 << time_ptr->tm_mday;
    else
	day  << time_ptr->tm_mday;

    if (time_ptr->tm_hour < 10)
	hour << 0 << time_ptr->tm_hour;
    else
	hour << time_ptr->tm_hour;

    if (time_ptr->tm_min < 10)
	min  << 0 << time_ptr->tm_min;
    else
	min  << time_ptr->tm_min;

    if (time_ptr->tm_sec < 10)
	min  << 0 << time_ptr->tm_sec;
    else
	min  << time_ptr->tm_sec;

/*
    str_stream << year.str()
	       << month.str()
#ifdef ORG
	       << day.str();
#else
             << day.str()
             << hour.str()
             << min.str()
	     << sec.str();
#endif
*/

    if (!m_auto_fname) {
	str_stream << year.str()
		   << month.str()
		   << day.str();
    }
    else {
	str_stream << year.str()
		   << month.str()
		   << day.str()
		   << hour.str()
		   << min.str()
		   << sec.str();
    }

    std::cerr << "FileUtils: gettime:" << str_stream.str() << std::endl;
    return str_stream.str();
}



int FileUtils::make_dir_loop(const char* name)
{
    int ret = 0;

    char name2[48];
    char* p = strchr(name, '/');
    if (p == NULL) {
	strcpy(name2, name);
	std::cerr << "p==NULL:" << name2 << std::endl;
	ret = make_dir2(name2);
    } else {

	int i = p-name;
	std::cerr << "i = " << i << std::endl;
	if (i != 0) {
	    strncpy(name2, name, i);
	    name2[i] = '\0';
	    std::cerr << "p!=NULL:" << name2 << std::endl;
	    ret = make_dir2(name2);
	}

	if (ret == 0) {
	    ret = make_dir_loop(&name[i+1]);
	}
    }
    return ret;
}

int FileUtils::make_dir2(const char* name)
{
    std::cout << " make_dir2  name:" << name << std::endl;

    int ret = mkdir(name, S_IRUSR|S_IWUSR|S_IXUSR);
    if (ret!= 0) {
	std::cerr << "FileUtils: can not make a directory!!!\n";
	perror("FileUtils: mkdir");
    }
    ret = chdir(name);
    if (ret!= 0) {
	std::cerr << "FileUtils: can not change directory!!!\n";
	perror("FileUtils: chdir");
    }
    if (ret== 0) {
	std::cerr << "name = " << name << std::endl;
	m_dir_list.push_back(name);
    }

    return ret;
}

std::string& FileUtils::replace(std::string& str, const std::string sb, 
				const std::string sa)
{
    std::string::size_type n, nb = 0;

    while((n = str.find(sb, nb)) != std::string::npos) {
	str.replace(n, sb.size(), sa);
	nb = n + sa.size();
    }
    return str;
}

void FileUtils::new_name(int index, std::string name)
{
    //replace(name, std::string("."), std::string("_")); /// replace "." to "_" 
    //m_file_info[index].addr = name;
      
    m_file_info[index].size = 0;
    m_file_info[index].no = 0;
}

void FileUtils::next_name(int index)
{
    m_file_info[index].size = 0;
    m_file_info[index].no += 1;
}

int FileUtils::open_file()
{
    m_file_info.resize(1);
    std::string fileName = make_name();
    int index = 0;
    m_file_info[index].name = fileName;
    std::cerr << "Logger: fileName: " << fileName << std::endl;

    try {
	std::ofstream* outFile = new std::ofstream();
	outFile->rdbuf()->pubsetbuf(m_file_info[index].stream_buf,
		sizeof(m_file_info[index].stream_buf));
	outFile->open(fileName.c_str());
	m_file_info[index].file = outFile;
    }
    catch(...) {
	std::cerr << "### ERROR: open file: exception occured\n";
    }
    return 0;
}

int FileUtils::open_file(std::string dir_name)
{
    m_dir_name = dir_name;
    m_file_info.resize(1);
    std::string fileName = make_name();
    
    int index = 0;
    m_file_info[index].name = dir_name + "/" + fileName;
    std::cerr << "Logger: fileName: " << fileName << std::endl;

    try {
	std::ofstream* outFile = new std::ofstream();
	outFile->rdbuf()->pubsetbuf(m_file_info[index].stream_buf,
		sizeof(m_file_info[index].stream_buf));
	outFile->open(m_file_info[index].name.c_str());
	m_file_info[index].file = outFile;
    }
    catch(...) {
	std::cerr << "### ERROR: open file: exception occured\n";
    }
    return 0;
}

std::string FileUtils::make_name()
{
    //char run[9];
    int index = 0;
    std::stringstream run_no;
    std::stringstream file_no;
    if(!m_auto_fname) {
	run_no  << std::setw(6) << std::setfill('0') << m_run_no;
    }
    else {
	run_no  << m_run_no;
    }
    file_no << std::setw(3) << std::setfill('0') << m_file_info[index].no;
    std::string fileName 
	= gettime() 
	+ "_" 
	+ run_no.str() 
	+ "_"
	+ file_no.str() 
	+ "." 
	+ m_ext_name;
    std::cerr << "*** make_name:" << fileName << std::endl;
    return fileName;
}

#ifdef PSD
std::string FileUtils::make_name(int index)
{
    //char run[9];
    std::stringstream run_no;
    std::stringstream daq_id;
    std::stringstream module_no;
    std::stringstream file_no;

    run_no  << std::setw(6) << std::setfill('0') << m_run_no;
    daq_id  << std::setw(2) << std::setfill('0') << m_daq_id;
    module_no << std::setw(3) << std::setfill('0') << m_file_info[index].addr;
    file_no << std::setw(3) << std::setfill('0') << m_file_info[index].no;

    std::string fileName = 
	m_inst_id 
	+ run_no.str()
	+ "_"
	+ daq_id.str()
	+ "_"
	//+ m_file_info[index].addr 
	+ module_no.str()
	+ "_" 
	+ file_no.str() 
	+ ".edb";

    return fileName;
}
#endif 

int FileUtils::close_file()
{
    m_file_info[0].file->close();
    return 0;
}

int FileUtils::close_file(int index)
{
    m_file_info[index].file->close();

#ifdef USE_COMPRESS
    if (m_compress_flag == true) {
	const char* name = m_file_info[index].name.c_str();
//		std::cerr << "zip: " << name << std::endl;
		
	char com[128];
	sprintf(com, "zip -m %s.zip %s", name, name);
	
	system(com);
    }
#endif

    return 0;
}

#ifdef PSD
int FileUtils::remove_dir(bool flag)
{
    int ret = 0;

    if (flag == true) {
	for (int i = 0; i < m_file_num; ++i) {
	    rm_file(i);
	}
    }

    int len = m_dir_list.size();
    for (int i = 0; i < len; ++i) {
      if ( chdir("..") != 0 ) {
	std::cerr << "### ERROR: Logger: can not change a directory\n";
	return -1;
      }

	if (i == 0 && flag == true) {
	    ret = rmdir(m_dir_list[len-1].c_str());

	    if (ret != 0) {
		std::cerr << "Logger: can not remove a directory!!!\n";
		return -1;
	    }
	}
    }
 
    return ret;
}

void FileUtils::rm_file(int index)
{
    int no = m_file_info[index].no;
    for (int i = 0; i <= no; ++i) {
	m_file_info[index].no = i;
	std::string fileName = make_name(index);
	if (m_compress_flag == true) {
	    fileName += ".zip";
	}

	unlink(fileName.c_str());
    }
}
#endif
