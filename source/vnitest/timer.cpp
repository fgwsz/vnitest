#include<vnitest/timer.h>

#include<chrono>//::std::chrono
#include<string>//::std::string
#include<format>//::std::format

namespace vnitest{

Timer::Timer(void)noexcept
    :start_time_(::std::chrono::steady_clock::now())
    ,total_duration_(0.0)
    ,state_(State::STOPPED)
{}
void Timer::start(void)noexcept{
    if(this->state_==State::STOPPED){
        this->start_time_=::std::chrono::steady_clock::now();
        this->state_=State::RUNNING;
    }
}
void Timer::stop(void)noexcept{
    if(this->state_==State::RUNNING){
        this->total_duration_+=
            ::std::chrono::duration_cast<::std::chrono::nanoseconds>(
                ::std::chrono::steady_clock::now()-this->start_time_
            ).count();
        this->state_=State::STOPPED;
    }
}
void Timer::reset(void)noexcept{
    this->total_duration_=0;
    if(this->state_==State::RUNNING){
        this->start_time_=::std::chrono::steady_clock::now();
    }
}
double Timer::duration_nanoseconds(void)const noexcept{
    return this->total_duration_;
}
double Timer::duration_microseconds(void)const noexcept{
    return this->total_duration_/1000.0;
}
double Timer::duration_milliseconds(void)const noexcept{
    return this->total_duration_/1000'000.0;
}
double Timer::duration_seconds(void)const noexcept{
    return this->total_duration_/1000'000'000.0;
}
double Timer::duration_minutes(void)const noexcept{
    return this->total_duration_/60'000'000'000.0;
}
double Timer::duration_hours(void)const noexcept{
    return this->total_duration_/3600'000'000'000.0;
}
::std::string Timer::nanoseconds_to_string(double ns)noexcept{
    struct TimeUnit{
        unsigned long long factor;
        char const* suffix;
    };
    static constexpr TimeUnit time_units[]={
       {3600'000'000'000,"h" },//1 h
       {60'000'000'000  ,"m" },//1 m
       {1'000'000'000   ,"s" },//1 s
       {1'000'000       ,"ms"},//1 ms
       {1'000           ,"us"},//1 us
    };
    //优先处理零值
    if(ns==0.0){
        return "0 ns";
    }
    ::std::string ret={};
    //处理负数
    if(ns<0.0){
        ret+="- ";
        ns=-ns;
    }
    //直接处理小数量级别的纳秒(< 1 us)
    if(ns<1000){
        ret+=::std::format("{} ns",ns);
        return ret;
    }
    bool first=true;
    unsigned long long value=0;
    unsigned long long integer_ns=static_cast<unsigned long long>(ns);//整数部分
    double remainder_ns=ns-integer_ns;//小数部分
    for(auto const& unit:time_units){
        if(integer_ns<unit.factor){
            continue;
        }
        value=integer_ns/unit.factor;
        integer_ns%=unit.factor;
        if(!first){
            ret+=" ";
        }
        first=false;
        ret+=::std::format("{} {}",value,unit.suffix);
    }
    ns=integer_ns+remainder_ns;
    //处理零纳秒
    if(ns==0){
        return ret;
    }
    //处理纳秒
    if(!first){
        ret+=" ";
    }
    ret+=::std::format("{} ns",ns);
    return ret;
}
::std::string Timer::duration_string(void)const noexcept{
    return this->nanoseconds_to_string(this->total_duration_);
}
Timer::State Timer::get_state(void)const noexcept{
    return this->state_;
}
bool Timer::is_stopped(void)const noexcept{
    return this->state_==State::STOPPED;
}
bool Timer::is_running(void)const noexcept{
    return this->state_==State::RUNNING;
}

}//namespace vnitest
