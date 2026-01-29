#include <stdio.h>
#include "base/Config.h"
#include <iostream>
#include "base/FileLog.h"
#include "base/FileMgr.h"
#include "base/Task.h"
#include "base/TaskMgr.h"
#include <thread>
using namespace tmms::base;
int main() {
    if (!sConfigMgr->LoadConfig("../config/config.json")) {
        std::cerr << "Load config failed." << std::endl;
        return -1;
    }

    ConfigPtr config = sConfigMgr->GetConfig();
    if (!config) {
        std::cerr << "Config is null." << std::endl;
        return -1;
    }

    LogInfoPtr log_info = config->GetLogInfo();
    if (!log_info) {
        std::cerr << "LogInfo is null." << std::endl;
        return -1;
    }

    std::cout << "log level:" << log_info->level << std::endl;
    std::cout << "log path:" << log_info->path << std::endl;
    std::cout << "log name:" << log_info->name << std::endl;

    FileLogPtr log = sFileMgr->GetFileLog(log_info->path + log_info->name);
    if (!log) {
        std::cerr << "log can't open. exit." << std::endl;
        return -1;
    }

    log->SetRotate(log_info->rotate_type);

    tmms::base::g_logger = new Logger(log);
    tmms::base::g_logger->SetLogLevel(log_info->level);


    TaskPtr task = std::make_shared<Task>(
        [](const TaskPtr &task){
            sFileMgr->OnCheck();
            task->Restart();
        }, 
        1000
    );

    sTaskMgr->Add(task);

    while (1) {
        sFileMgr->OnCheck();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}
