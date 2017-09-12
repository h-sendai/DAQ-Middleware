#include "logUtil.h"

/*************************************************************************
    System : KEK MLF Module Tester.
    Author : wada@bbtech.co.jp
    Description : BBT MLF Utilites for Linux
    $KMM_TESTER_BEGIN_LICENSE$
    $KMM_TESTER_END_LICENSE$
*************************************************************************/

/****************************************************************
 *fprintf with time - for logging function
 *****************************************************************/
int fprintfwt(FILE *pFILE,const char *fmt,...)
{
    struct timeval tv;
    struct tm time_struct;
    int nRET = -1;
    if( pFILE != NULL)
    {
        nRET = -2;
        char buff[2048];
        char timestamp_buf[128];
        va_list ap;
        va_start(ap, fmt);
        int n = vsnprintf(buff ,sizeof(buff)-2, fmt, ap);
        va_end(ap);
        if( n > 0 ){
            gettimeofday(&tv, NULL);
            nRET = 0;
            //time_t tmt;
            //time(&tmt);
            //struct tm *pTm = localtime(&tmt);;
            localtime_r(&tv.tv_sec, &time_struct);
            strftime(timestamp_buf, sizeof(timestamp_buf), "%F %T", &time_struct);
            //nRET += fprintf(pFILE,"%4d/%02d/%02d %02d:%02d:%02d ",
            //  1900+pTm->tm_year,pTm->tm_mon+1,pTm->tm_mday,
            //pTm->tm_hour,pTm->tm_min,pTm->tm_sec);
            nRET += fprintf(pFILE, "%s.%06ld ", timestamp_buf, tv.tv_usec);
            nRET += fprintf(pFILE,buff);
        }
    }
    return nRET;
}
