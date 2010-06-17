#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <sys/time.h>

namespace DAQMW {

class Timer
{
public:
    Timer(unsigned int alarmInSec) 
  //: m_timer_alarm(false)
    {
	std::cerr << "Timer constructor\n";
	m_alarm_time_usec = alarmInSec*1000000;
	gettimeofday(&m_previous_time, 0);
    }

    virtual ~Timer()
    {
	std::cerr << "Timer deconstructr\n";
    }

    bool checkTimer()
    {
	bool ret = false;
	if (gettimeofday(&m_current_time, 0) != 0) {
	    perror("gettimeofday:");
	}

	suseconds_t elapsed_utime = (m_current_time.tv_sec - m_previous_time.tv_sec) * 1000000 
	  + (m_current_time.tv_usec - m_previous_time.tv_usec);

	if(elapsed_utime > m_alarm_time_usec) {
	    m_previous_time.tv_sec  = m_current_time.tv_sec;
	    m_previous_time.tv_usec = m_current_time.tv_usec;
	    ret = true;
	}

	return ret;
    }

    int resetTimer()
    {
      //m_timer_alarm = false;
	gettimeofday(&m_previous_time, 0);
	return 0;
    }

    std::string getDate()
    {
        time_t now = time(0);

	std::string mydate = asctime(localtime(&now));
        mydate[mydate.length()-1] = ' ';
        mydate.erase(0, 4);
	return mydate;
    }   

private:
    suseconds_t m_alarm_time_usec;
    timeval m_previous_time;
    timeval m_current_time;
    //bool m_timer_alarm;
};

} //namespace
#endif
