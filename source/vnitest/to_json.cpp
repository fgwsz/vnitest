#include<vnitest/to_json.h>

#include<cmath>//::std::isnan ::std::isinf ::std::isnormal

#include<limits>//::std::numeric_limits
#include<array>//::std::array
#include<charconv>//::std::to_chars
#include<format>//::std::format
#include<string>//::std::string ::std::to_string
#include<ios>//::std::hex ::std::uppercase
#include<iomanip>//::std::setw ::std::setfill
#include<sstream>//::std::ostringstream

namespace vnitest{

namespace detail{

::std::string string_to_json(::std::string_view input)noexcept{
    ::std::ostringstream oss;
    oss<<"\"";
    for(char ch:input){
        switch(ch){
            case '\"':{ oss<<R"(\")"; break; }
            case '\\':{ oss<<R"(\\)"; break; }
            case '\n':{ oss<<R"(\n)"; break; }
            case '\r':{ oss<<R"(\r)"; break; }
            case '\t':{ oss<<R"(\t)"; break; }
            case '\b':{ oss<<R"(\b)"; break; }
            case '\f':{ oss<<R"(\f)"; break; }
            default  :{
                //ASCII 0-31
                if(static_cast<unsigned char>(ch)<=0x1F){
                    oss<<"\\u00"<<::std::hex<<::std::uppercase
                        <<::std::setw(2)<<::std::setfill('0')
                        <<static_cast<unsigned int>(
                            static_cast<unsigned char>(ch)
                        );
                }else{
                    oss<<ch;
                }
                break;
            }
        }
    }
    oss<<"\"";
    return oss.str();
}

}//namespace vnitest::detail

::std::string to_json(
    bool condition
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    return condition?"true":"false";
}

::std::string to_json(
    ::std::string_view sv
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    return ::vnitest::detail::string_to_json(sv);
}

::std::string to_json(
    ::std::string const& str
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    return ::vnitest::detail::string_to_json(str);
}

::std::string to_json(
    ::std::size_t size
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    return ::std::to_string(size);
}

::std::string to_json(
    double number
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    if(::std::isnan(number)){
        return "null";
    }
    if(::std::isinf(number)){
        return (number>0)?"1e999":"-1e999";
    }
    constexpr double u64_max=
        static_cast<double>(::std::numeric_limits<unsigned long long>::max());
    constexpr double i64_min=
        static_cast<double>(::std::numeric_limits<long long>::min());
    if(number>=i64_min&&number<=u64_max){
        if(number>=0){
            return ::std::to_string(static_cast<unsigned long long>(number));
        }else{
            return ::std::to_string(static_cast<long long>(number));
        }
    }
    ::std::array<char,32> buffer;
    auto[ptr,ec]=
        ::std::to_chars(buffer.data(),buffer.data()+buffer.size(),number);
    return(ec==::std::errc{})?::std::string(buffer.data(),ptr):"null";
}

::std::string to_json(
    ::vnitest::RuntimeAssertFailedException const& exception
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    ::std::string ret={"{"};
    ::std::size_t current_space_number=tab_width*current_tab_number;
    ::std::size_t next_space_number=tab_width*(current_tab_number+1);
    ::std::size_t next_tab_number=current_tab_number+1;
    auto ret_append=[&](::std::string_view name,auto const& value)noexcept{
        ret+=::std::format(
            "\n{}{}:{},"
            ,::std::string(next_space_number,' ')
            ,::vnitest::to_json(name)
            ,::vnitest::to_json(value,tab_width,next_tab_number)
        );
    };
    ret_append("file",exception.file());
    ret_append("line",exception.line());
    ret_append("info",exception.info());
    if(exception.has_msg()){
        ret_append("msg",exception.msg());
    }
    ret.pop_back();//remove last ','
    ret+=::std::format("\n{}}}",::std::string(current_space_number,' '));
    return ret;
}

::std::string to_json(
    ::vnitest::RuntimeCheckFailedError const& error
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    ::std::string ret={"{"};
    ::std::size_t current_space_number=tab_width*current_tab_number;
    ::std::size_t next_space_number=tab_width*(current_tab_number+1);
    ::std::size_t next_tab_number=current_tab_number+1;
    auto ret_append=[&](::std::string_view name,auto const& value)noexcept{
        ret+=::std::format(
            "\n{}{}:{},"
            ,::std::string(next_space_number,' ')
            ,::vnitest::to_json(name)
            ,::vnitest::to_json(value,tab_width,next_tab_number)
        );
    };
    ret_append("file",error.file);
    ret_append("line",error.line);
    ret_append("info",error.info);
    if(error.msg.has_value()){
        ret_append("msg",error.msg.value());
    }
    ret.pop_back();//remove last ','
    ret+=::std::format("\n{}}}",::std::string(current_space_number,' '));
    return ret;
}

::std::string to_json(
    ::vnitest::SkipException const& exception
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    ::std::string ret={"{"};
    ::std::size_t current_space_number=tab_width*current_tab_number;
    ::std::size_t next_space_number=tab_width*(current_tab_number+1);
    ::std::size_t next_tab_number=current_tab_number+1;
    auto ret_append=[&](::std::string_view name,auto const& value)noexcept{
        ret+=::std::format(
            "\n{}{}:{},"
            ,::std::string(next_space_number,' ')
            ,::vnitest::to_json(name)
            ,::vnitest::to_json(value,tab_width,next_tab_number)
        );
    };
    ret_append("file",exception.file());
    ret_append("line",exception.line());
    ret_append("info",exception.info());
    if(exception.has_msg()){
        ret_append("msg",exception.msg());
    }
    ret.pop_back();//remove last ','
    ret+=::std::format("\n{}}}",::std::string(current_space_number,' '));
    return ret;
}

::std::string to_json(
    ::std::vector<::vnitest::RuntimeCheckFailedError>const& errors
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    ::std::string ret={"["};
    for(auto const& element:errors){
        ret+=::std::format(
            "\n{}{},"
            ,::std::string(tab_width*(current_tab_number+1),' ')
            ,::vnitest::to_json(element,tab_width,current_tab_number+1)
        );
    }
    if(!errors.empty()){
        ret.pop_back();//remove last ','
        ret+=::std::format(
            "\n{}",::std::string(tab_width*current_tab_number,' ')
        );
    }
    ret+=']';
    return ret;
}

::std::string to_json(
    ::vnitest::ExecuteCaseInfo const& ei
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    if(ei.is_undefined()){
        return "null";
    }
    ::std::string ret={"{"};
    ::std::size_t current_space_number=tab_width*current_tab_number;
    ::std::size_t next_space_number=tab_width*(current_tab_number+1);
    ::std::size_t next_tab_number=current_tab_number+1;
    auto ret_append=[&](::std::string_view name,auto const& value)noexcept{
        ret+=::std::format(
            "\n{}{}:{},"
            ,::std::string(next_space_number,' ')
            ,::vnitest::to_json(name)
            ,::vnitest::to_json(value,tab_width,next_tab_number)
        );
    };
    //name
    ret_append("case name",ei.get_case_name());
    if(ei.has_group_name()){
        ret_append("group name",ei.get_group_name());
    }
    //duration
    ret_append("duration ns",ei.get_duration_ns());
    ret_append("duration string",ei.get_duration_string());
    //runtime check
    ret_append("runtime check total",ei.get_runtime_check_total());
    ret_append("runtime check passed",ei.get_runtime_check_passed());
    ret_append("runtime check failed",ei.get_runtime_check_failed());
    ret_append(
        "runtime check passed rate",ei.get_runtime_check_passed_rate()
    );
    //runtime assert
    ret_append("runtime assert total",ei.get_runtime_assert_total());
    ret_append("runtime assert passed",ei.get_runtime_assert_passed());
    ret_append("runtime assert failed",ei.get_runtime_assert_failed());
    ret_append(
        "runtime assert passed rate",ei.get_runtime_assert_passed_rate()
    );
    //state
    ret_append("is passed",ei.is_passed());
    ret_append("is failed",ei.is_failed());
    ret_append("is skipped",ei.is_skipped());
    ret_append("is undefined",ei.is_undefined());
    //runtime check errors
    if(ei.has_runtime_check_failed_errors()){
        ret_append(
            "runtime check failed errors"
            ,ei.get_runtime_check_errors()
        );
    };
    //runtime assert failed exception
    if(ei.has_runtime_assert_failed_exception()){
        ret_append(
            "runtime assert failed exception"
            ,ei.get_runtime_assert_failed_exception()
        );
    }
    //runtime exception
    if(ei.has_runtime_exception()){
        ret_append(
            "runtime exception"
            ,ei.get_runtime_exception()
        );
    }
    //skip
    if(ei.has_skip()){
        ret_append("skip",ei.get_skip());
    }
    //
    ret.pop_back();//remove last ','
    ret+=::std::format("\n{}}}",::std::string(current_space_number,' '));
    return ret;
}

::std::string to_json(
    ::std::vector<::vnitest::ExecuteCaseInfo> const& ei_vec
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    ::std::string ret={"["};
    for(auto const& ei:ei_vec){
        ret+=::std::format(
            "\n{}{},"
            ,::std::string(tab_width*(current_tab_number+1),' ')
            ,::vnitest::to_json(ei,tab_width,current_tab_number+1)
        );
    }
    if(!ei_vec.empty()){
        ret.pop_back();//remove last ','
        ret+=::std::format(
            "\n{}",::std::string(tab_width*current_tab_number,' ')
        );
    }
    ret+=']';
    return ret;
}

::std::string to_json(
    ::vnitest::ExecuteGroupInfo const& ei
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    if(ei.is_undefined()){
        return "null";
    }
    ::std::string ret={"{"};
    ::std::size_t current_space_number=tab_width*current_tab_number;
    ::std::size_t next_space_number=tab_width*(current_tab_number+1);
    ::std::size_t next_tab_number=current_tab_number+1;
    auto ret_append=[&](::std::string_view name,auto const& value)noexcept{
        ret+=::std::format(
            "\n{}{}:{},"
            ,::std::string(next_space_number,' ')
            ,::vnitest::to_json(name)
            ,::vnitest::to_json(value,tab_width,next_tab_number)
        );
    };
    //name
    ret_append("name",ei.get_name());
    //duration
    ret_append("duration ns",ei.get_duration_ns());
    ret_append("duration string",ei.get_duration_string());
    //runtime check
    ret_append("case total",ei.get_case_total());
    ret_append("case passed",ei.get_case_passed());
    ret_append("case failed",ei.get_case_failed());
    ret_append("case skipped",ei.get_case_skipped());
    ret_append("case undefined",ei.get_case_undefined());
    ret_append(
        "case passed rate",ei.get_case_passed_rate()
    );
    //state
    ret_append("is passed",ei.is_passed());
    ret_append("is failed",ei.is_failed());
    ret_append("is skipped",ei.is_skipped());
    ret_append("is undefined",ei.is_undefined());
    //data
    ret+=std::format(
        "\n{}{}:{{"
        ,::std::string(next_space_number,' ')
        ,::vnitest::to_json(std::string_view{"data"})
    );
    for(auto const& info:ei.get_data()){
        if(::std::holds_alternative<::vnitest::ExecuteCaseInfo>(info)){
            auto const& ci=::std::get<::vnitest::ExecuteCaseInfo>(info);
            ret+=::std::format(
                "\n{}{}:{},"
                ,::std::string(tab_width*(current_tab_number+2),' ')
                ,::vnitest::to_json(ci.get_case_name())
                ,::vnitest::to_json(ci,tab_width,current_tab_number+2)
            );
        }else{
            auto const& ri=::std::get<
                typename ::vnitest::ExecuteGroupInfo::case_regex_info_type
            >(info);
            ret+=::std::format(
                "\n{}{}:{},"
                ,::std::string(tab_width*(current_tab_number+2),' ')
                ,::vnitest::to_json(::std::get<0>(ri))
                ,::vnitest::to_json(
                    ::std::get<1>(ri),tab_width,current_tab_number+2
                )
            );
        }
    }
    if(!ei.get_data().empty()){
        ret.pop_back();//remove last ','
        ret+=::std::format(
            "\n{}",::std::string(next_space_number,' ')
        );
    }
    ret+='}';
    //
    ret+=::std::format("\n{}}}",::std::string(current_space_number,' '));
    return ret;
}

::std::string to_json(
    ::std::vector<::vnitest::ExecuteGroupInfo> const& ei_vec
    ,::std::size_t tab_width
    ,::std::size_t current_tab_number
)noexcept{
    ::std::string ret={"["};
    for(auto const& ei:ei_vec){
        ret+=::std::format(
            "\n{}{},"
            ,::std::string(tab_width*(current_tab_number+1),' ')
            ,::vnitest::to_json(ei,tab_width,current_tab_number+1)
        );
    }
    if(!ei_vec.empty()){
        ret.pop_back();//remove last ','
        ret+=::std::format(
            "\n{}",::std::string(tab_width*current_tab_number,' ')
        );
    }
    ret+=']';
    return ret;
}

}//namespace vnitest
