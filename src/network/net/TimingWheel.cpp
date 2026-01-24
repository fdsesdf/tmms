#include "TimingWheel.h"
#include "network/base/Network.h"
using namespace tmms::network;
TimingWheel::TimingWheel():wheels_(4)
{
    wheels_[kTmingTypeSecond].resize(60);
    wheels_[kTmingTypeMinute].resize(60);
    wheels_[kTmingTypeHour].resize(24);
    wheels_[kTmingTypeDay].resize(30);
}
TimingWheel::~TimingWheel(){}

void TimingWheel::InsertSecondEntry(uint32_t delay, EntryPtr  entryPtr)
{
    if (!entryPtr) return;
    if (delay == 0) delay = 1;
    wheels_[kTmingTypeSecond][delay-1].emplace(entryPtr);
}
void TimingWheel::InsertMinuteEntry(uint32_t delay, EntryPtr  entryPtr)
{
    if (!entryPtr) return;
    auto minute=delay/kTimingMinute;
    auto second=delay%kTimingMinute;
    if (second == 0) second = 1;     
    if (minute == 0) minute = 1; 
    CallbackEntryPtr newEntryPtr=std::make_shared<CallbackEntry>([this,second,entryPtr](){
        InsertSecondEntry(second,entryPtr);
    });
    wheels_[kTmingTypeMinute][minute-1].emplace(newEntryPtr);      
}
void TimingWheel::InsertHourEntry(uint32_t delay, EntryPtr  entryPtr)
{
    if (!entryPtr) return;
    auto hour=delay/kTimingHour;
    auto minute=delay%kTimingHour;
    if (minute == 0) minute = 1;
    if (hour == 0) hour = 1;
    CallbackEntryPtr newEntryPtr=std::make_shared<CallbackEntry>([this,minute,entryPtr](){
        InsertMinuteEntry(minute,entryPtr);
    });
    wheels_[kTmingTypeHour][hour-1].emplace(newEntryPtr);
}
void TimingWheel::InsertDayEntry(uint32_t delay, EntryPtr  entryPtr)
{
    if (!entryPtr) return;
    auto day=delay/kTimingDay;
    auto hour=delay%kTimingDay;
    if (hour == 0) hour = 1;
    if (day == 0) day = 1;
    CallbackEntryPtr newEntryPtr=std::make_shared<CallbackEntry>([this,hour,entryPtr](){
        InsertHourEntry(hour,entryPtr);
    });
    wheels_[kTmingTypeDay][day-1].emplace(newEntryPtr);
}







void TimingWheel::InsertEntry(uint32_t delay, EntryPtr entryPtr)
{
    if (!entryPtr) return;
    if (delay == 0) delay = 1;
    if (delay < kTimingMinute) {
        // 1~59 秒
        InsertSecondEntry(delay, entryPtr);
    } else if (delay < kTimingHour) {
        // 60~3599 秒
        InsertMinuteEntry(delay, entryPtr);
    } else if (delay < kTimingDay) {
        // 3600~86399 秒
        InsertHourEntry(delay, entryPtr);
    } else {
        // >= 1 天
        uint32_t day = delay / kTimingDay;
        if (day > 30) {
            NETWORK_LOG_ERROR << "day must be less than 30";
            return;
        }
        InsertDayEntry(delay, entryPtr); 
    }
}

void TimingWheel::OnTimer(int64_t now)
{
    if (last_ts_ == 0) {
        last_ts_ = now;
        return;
    }

    if (now <= last_ts_) {
        return;
    }

    int64_t diff = now - last_ts_;
    last_ts_ = now;

    for (int i = 0; i < diff; ++i) {
        ++tick_;
        PopUp(wheels_[kTmingTypeSecond]);

        if (tick_ % kTimingMinute == 0)
            PopUp(wheels_[kTmingTypeMinute]);

        if (tick_ % kTimingHour == 0)
            PopUp(wheels_[kTmingTypeHour]);

        if (tick_ % kTimingDay == 0)
            PopUp(wheels_[kTmingTypeDay]);
    }
}

void TimingWheel::PopUp(Wheel &bq)
{
    WheelEntry tmp;
    bq.front().swap(tmp);
    bq.pop_front();
    bq.push_back(WheelEntry());
}
void TimingWheel::RunAfter(double delay, const Func &cb)
{
    CallbackEntryPtr cbEntry=std::make_shared<CallbackEntry>([cb](){
        cb();
    });
    InsertEntry(delay,cbEntry);
}
void TimingWheel::RunAfter(double delay,  Func &&cb)
{
    CallbackEntryPtr cbEntry=std::make_shared<CallbackEntry>([cb](){
        cb();
    });
    InsertEntry(delay,cbEntry);
}
void TimingWheel::RunEvery(double interval, const Func &cb)
{
    CallbackEntryPtr cbEntry=std::make_shared<CallbackEntry>([this,cb,interval](){
        cb();
        RunEvery(interval,cb);
    });
    InsertEntry(interval,cbEntry);
}
void TimingWheel::RunEvery(double interval,  Func &&cb)
{
    CallbackEntryPtr cbEntry=std::make_shared<CallbackEntry>([this,cb,interval](){
        cb();
        RunEvery(interval,cb);
    });
    InsertEntry(interval,cbEntry);
}