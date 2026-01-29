#include<vnitest/runtime_check_stream.hpp>

#include<iostream>//::std::cerr ::std::endl
#include<optional>//::std::nullopt

#include<vnitest/runtime_check_failed_error.h>
#include<vnitest/execute_case_info.h>
#include<vnitest/to_json.h>

namespace vnitest{

RuntimeCheckStream::RuntimeCheckStream(::vnitest::ConditionInfo const& ci)
noexcept
    :info_(ci)
    ,message_(::std::nullopt)
{}
RuntimeCheckStream::~RuntimeCheckStream(void)noexcept{
    ::std::optional<::vnitest::RuntimeCheckFailedError> error={};
    if(!this->info_.condition){
        error=::vnitest::RuntimeCheckFailedError{
            this->info_.file
            ,this->info_.line
            ,this->info_.info
        };
    }
    if(::vnitest::ExecuteCaseInfo::has_current()){//in VNITEST_CASE
        ::vnitest::ExecuteCaseInfo::get_current()
            .runtime_check_total_increment();
    }
    if(!error.has_value()){
        if(::vnitest::ExecuteCaseInfo::has_current()){//in VNITEST_CASE
            ::vnitest::ExecuteCaseInfo::get_current()
                .runtime_check_passed_increment();
        }
    }else{
        if(this->message_.has_value()){
            error.value().msg=this->message_.value();
        }
        if(::vnitest::ExecuteCaseInfo::has_current()){//in VNITEST_CASE
            ::vnitest::ExecuteCaseInfo::get_current()
                .runtime_check_failed_increment()
                .runtime_check_failed_errors_push_back(error.value());
        }else{//in other function
            ::std::cerr
                <<::vnitest::to_json(error.value())
                <<::std::endl;
        }
    }
}

}//namespace vnitest
