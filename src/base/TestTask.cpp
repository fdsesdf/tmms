#include "TaskMgr.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace tmms::base;
void TestTask()
{
    TaskPtr task1=std::make_shared<Task>(
        [](const TaskPtr &task){
            std::cout<<"Task 1 executed\n"<<std::endl;
        },1000
    );
    TaskPtr task2=std::make_shared<Task>(
        [](const TaskPtr &task){
            std::cout<<"Task 2 executed\n"<<std::endl;
            task->Restart();
        },1000
    );
    TaskPtr task3=std::make_shared<Task>(
        [](const TaskPtr &task){
            std::cout<<"Task 3 executed\n"<<std::endl;
            task->Restart();
        },500
    );
    TaskPtr task4=std::make_shared<Task>(
        [](const TaskPtr &task){
            std::cout<<"Task 4 executed\n"<<std::endl;
            task->Restart();
        },30000
    );
    sTaskMgr->Add(task1);
    sTaskMgr->Add(task2);
    sTaskMgr->Add(task3);
    sTaskMgr->Add(task4);
}
// int main(int argc, char** argv)
// {
//     TestTask();
//     while(true)
//     {
//         sTaskMgr->OnWork();
//         std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     }
//     return 0;
// }