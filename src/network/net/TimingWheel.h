#pragma once
#include <memory>
#include <functional>
#include <vector>
#include <cstdint>
#include <deque>
#include <unordered_set>
namespace tmms
{
    namespace network
    {
        using EntryPtr=std::shared_ptr<void>;
        using WheelEntry=std::unordered_set<EntryPtr>;
        using Wheel=std::deque<WheelEntry>;
        using Wheels=std::vector<Wheel>;
        using Func=std::function<void()>;
        enum TimingType
        {
            kTmingTypeSecond=0,//秒级定时器
            kTmingTypeMinute=1,//分钟级定时器
            kTmingTypeHour=2,//小时级定时器
            kTmingTypeDay=3,//天级定时器
        };
        const int kTimingMinute=60;//分钟级定时器的时间间隔，单位是秒
        const int kTimingHour=60*kTimingMinute;//小时级定时器的时间间隔，单位是秒
        const int kTimingDay=24*kTimingHour;//天级定时器的时间间隔，单位是秒
        class CallbackEntry
        {
        public:
            CallbackEntry(const Func cb):cb_(cb){}
            ~CallbackEntry(){
                if(cb_)
                {
                    cb_();
                }
            }
        private:
            Func cb_;
        };
        using CallbackEntryPtr=std::shared_ptr<CallbackEntry>;
        class TimingWheel
        {
        public:
            TimingWheel();
            ~TimingWheel();

            void InsertEntry(uint32_t delay, EntryPtr  entryPtr);//插入一个任务，delay是延迟时间，单位是秒
            void OnTimer(int64_t now);//now是当前时间戳，单位是秒
            void PopUp(Wheel &bq);//弹出到期的任务
            void RunAfter(double delay, const Func &cb);//delay秒后执行一次
            void RunAfter(double delay, Func &&cb);//delay秒后执行一次
            void RunEvery(double interval, const Func &
            cb);//interval秒间隔执行一次
            void RunEvery(double interval,  Func &&cb);//interval秒间隔执行一次
        private:
            void InsertSecondEntry(uint32_t delay, EntryPtr  entryPtr);//插入一个秒级任务
            void InsertMinuteEntry(uint32_t delay, EntryPtr  entryPtr);//插入一个分钟级任务
            void InsertHourEntry(uint32_t delay, EntryPtr  entryPtr);//插入一个小时级任务
            void InsertDayEntry(uint32_t delay, EntryPtr  entryPtr);//插入一个天级任务
            Wheels wheels_;
            int64_t last_ts_{0};//上次执行时间戳，单位是秒
            uint64_t tick_{0};//当前时间戳，单位是秒
        };
    }
}