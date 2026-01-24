#include "LogStream.h"
#include "Logger.h"
#include "FileLog.h"
#include "FileMgr.h"
#include <thread>
#include "TTime.h"
#include "TaskMgr.h"
using namespace tmms::base;

std::thread t;
void TestLog()
{
    t =std::thread([](){
        while(true)
        {
            for(int i=0;i<1000;++i)
            {
                LOG_TRACE<<"This is a trace log."<<TTime::NowMS();
                LOG_DEBUG<<"This is a debug log."<<TTime::NowMS();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    });

}
int main(int argc, char** argv)
{
    FileLogPtr file_log=sFileMgr->GetFileLog("test.log");
    tmms::base::g_logger=new Logger(file_log);
    tmms::base::g_logger->SetLogLevel(kTrace);file_log->SetRotate(RotateType::kRotateMinute);
    TaskPtr task4=std::make_shared<Task>(
        [](const TaskPtr &task){
            sFileMgr->OnCheck();
            task->Restart();
        },1000
    );
    sTaskMgr->Add(task4);
    TestLog();
    while (1)
    {
        sTaskMgr->OnWork();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    return 0;
}