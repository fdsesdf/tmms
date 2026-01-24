#include <string.h>
#include "LogStream.h"
#include "Logger.h"
#include "TTime.h"
#include <sys/syscall.h>
#include <unistd.h>
using namespace tmms::base;
static thread_local pid_t t_thread_id=0;
const char *log_strings[] = {
    " TRACE ",
    " DEBUG ",
    " INFO ",
    " WARN ",
    " ERROR "
};
LogStream::LogStream(Logger* logger,const char* file,int line,LogLevel l,const char* func)
    :logger_(logger)
{
    const char* file_name= strrchr(file,'/');
    if(file_name)
    {
        file_name++;
    }
    else
    {
        file_name=file;
    }
    stream_<<TTime::ISOTime();
    if(t_thread_id==0)
    {
        t_thread_id=static_cast<pid_t>(::syscall(SYS_gettid));
    }
    stream_<<t_thread_id;
    stream_<<log_strings[static_cast<int>(l)];
    stream_<<"["<<file_name<<":"<<line<<"]";
    if(func)
    {
        stream_<<"["<<func<<"() ]";
    }

}
LogStream::~LogStream()
{
    stream_<<std::endl;
    logger_->Write(stream_.str());
}
