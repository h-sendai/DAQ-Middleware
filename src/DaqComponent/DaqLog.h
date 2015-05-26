#ifndef DAQLOG_H
#define DAQLOG_H

// http://stackoverflow.com/questions/2212776/overload-handling-of-stdendl
// http://creativecommons.org/licenses/
// http://creativecommons.org/licenses/by-sa/3.0/

#include <sys/time.h>
#include <time.h>

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace DAQMW {
class DaqLog: public std::ostream
{
    class MyStreamBuf: public std::stringbuf
    {
        std::ostream& output;
        public:
            MyStreamBuf(std::ostream& str) : output(str)
            {}

            virtual int sync()
            {
                char timebuf[32];
                // 2015-05-05 12:34:56
                // 01234567890123456789

                struct timeval tv;
                gettimeofday(&tv, NULL);

                struct tm tm;
                localtime_r(&tv.tv_sec, &tm);
                strftime(timebuf, sizeof(timebuf), "%F %T", &tm);

                output << timebuf << "." << std::setw(3) << std::setfill('0') << tv.tv_usec / 1000 << " " << str();
                str("");
                output.flush();
                return 0;
            }
    };

    MyStreamBuf buffer;
    public:
        DaqLog(std::ostream& str): std::ostream(&buffer), buffer(str)
        {}
        //DaqLog(): std::ostream(&buffer), buffer(std::cerr)
        //{}
};
} // namespace
#endif
