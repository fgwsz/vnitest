#include<vnitest/execute.h>

#include<format>//::std::format
#include<regex>//::std::regex_match

#include<vnitest/case_dict.h>
#include<vnitest/execute_case_info.h>
#include<vnitest/group_dict.h>
#include<vnitest/execute_group_info.h>
#include<vnitest/to_json.h>

namespace vnitest{

::vnitest::ExecuteCaseInfo execute_case(
    ::std::string_view case_name
)noexcept{
    auto& case_dict=::vnitest::CaseDict::get();
    if(!case_dict.contains(case_name)){
        return ::vnitest::ExecuteCaseInfo{case_name};
    }
    auto const& case_body=case_dict.get(case_name);
    ::vnitest::ExecuteCaseInfo ci;
    ::vnitest::ExecuteCaseInfo::set_current(ci);
    ci.set_case_name(case_name)
        .execute_start();
    try{
        case_body();
    }catch(
        ::vnitest::RuntimeAssertFailedException const&
        runtime_assert_failed_exception
    ){
        ci.execute_stop();
        ci.set_runtime_assert_failed_exception(
            runtime_assert_failed_exception
        );
        ::vnitest::ExecuteCaseInfo::reset_current();
        return ci;
    }catch(::vnitest::SkipException const& skip_exception){
        ci.execute_stop();
        ci.set_skip(skip_exception);
        ::vnitest::ExecuteCaseInfo::reset_current();
        return ci;
    }catch(::std::exception const& std_exception){
        ci.execute_stop();
        ci.set_runtime_exception(
            ::std::format("::std::exception{{{}}}",std_exception.what())
        );
        ::vnitest::ExecuteCaseInfo::reset_current();
        return ci;
    }catch(char const* c_str){
        ci.execute_stop();
        ci.set_runtime_exception(::std::format("char const*{{{}}}",c_str));
        ::vnitest::ExecuteCaseInfo::reset_current();
        return ci;
    }catch(::std::string_view std_sv){
        ci.execute_stop();
        ci.set_runtime_exception(
            ::std::format("::std::string_view{{{}}}",std_sv)
        );
        ::vnitest::ExecuteCaseInfo::reset_current();
        return ci;
    }catch(::std::string const& std_str){
        ci.execute_stop();
        ci.set_runtime_exception(
            ::std::format("::std::string{{{}}}",std_str)
        );
        ::vnitest::ExecuteCaseInfo::reset_current();
        return ci;
    }catch(...){
        ci.execute_stop();
        ci.set_runtime_exception("Unknown{...}");
        ::vnitest::ExecuteCaseInfo::reset_current();
        return ci;
    }
    ci.execute_stop();
    ::vnitest::ExecuteCaseInfo::reset_current();
    return ci;
}

::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::regex const& case_name_regex
)noexcept{
    ::std::vector<::vnitest::ExecuteCaseInfo> ret={};
    ::vnitest::CaseDict::get().foreach(
        [&](::std::string_view case_name,auto const& case_body)
        #ifndef _MSC_VER
            noexcept
        #endif
        {
            static ::std::string name={};
            name=case_name;//::std::string_view to ::std::string
            if(::std::regex_match(name,case_name_regex)){
                ret.emplace_back(::vnitest::execute_case(case_name));
            }
        }
    );
    return ret;
}

::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        case_name_list
)noexcept{
    ::std::vector<::vnitest::ExecuteCaseInfo> ret={};
    for(auto const& element:case_name_list){
        if(::std::holds_alternative<::std::string_view>(element)){
            auto const& case_name=::std::get<::std::string_view>(element);
            ret.emplace_back(::vnitest::execute_case(case_name));
        }else{
            auto const& case_regex=::std::get<::std::regex>(element);
            auto ri=::vnitest::execute_case(case_regex);
            for(auto const& info:ri){
                ret.emplace_back(info);
            }
        }
    }
    return ret;
}

::std::vector<::vnitest::ExecuteCaseInfo> execute_case_all(void)noexcept{
    ::std::vector<::vnitest::ExecuteCaseInfo> ret={};
    ::vnitest::CaseDict::get().foreach(
        [&](::std::string_view case_name,auto const& case_body)
        #ifndef _MSC_VER
            noexcept
        #endif
        {
            ret.emplace_back(::vnitest::execute_case(case_name));
        }
    );
    return ret;
}

::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        case_name_list
)noexcept{
    return ::vnitest::execute_case(
        ::std::vector<::std::variant<::std::string_view,::std::regex>>{
            case_name_list
        }
    );
}

::vnitest::ExecuteGroupInfo execute_group(
    ::std::string_view group_name
)noexcept{
    auto& group_dict=::vnitest::GroupDict::get();
    if(!group_dict.contains(group_name)){
        return ::vnitest::ExecuteGroupInfo{group_name};
    }
    auto const& group_data=*(group_dict.get(group_name));
    auto& case_dict=::vnitest::CaseDict::get();
    ::vnitest::ExecuteGroupInfo ei;
    auto ei_case_increment=[&](::vnitest::ExecuteCaseInfo const& ci){
        ei.case_total_increment();
        if(ci.is_passed()){
            ei.case_passed_increment();
        }else if(ci.is_failed()){
            ei.case_failed_increment();
        }else if(ci.is_skipped()){
            ei.case_skipped_increment();
        }else{
            ei.case_undefined_increment();
        }
    };
    ei.set_name(group_name);
    ei.execute_start();
    for(auto const& element:group_data){
        if(std::holds_alternative<::std::string_view>(element)){
            auto const& case_name=::std::get<::std::string_view>(element);
            auto ci=::vnitest::execute_case(case_name);
            ci.set_group_name(ei.get_name());
            ei_case_increment(ci);
            ei.data_push_back(case_name,ci);
        }else{
            auto const& case_regex=::std::get<::vnitest::RegEx>(element);
            auto ri=::vnitest::execute_case(case_regex.object);
            for(auto& ci:ri){
                ci.set_group_name(ei.get_name());
                ei_case_increment(ci);
            }
            ei.data_push_back(case_regex.pattern,ri);
        }
    }
    ei.execute_stop();
    return ei;
}

::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::regex const& group_name_regex
)noexcept{
    ::std::vector<::vnitest::ExecuteGroupInfo> ret={};
    ::vnitest::GroupDict::get().foreach(
        [&](::std::string_view group_name,auto const& group_body)
        #ifndef _MSC_VER
            noexcept
        #endif
        {
            static ::std::string name={};
            name=group_name;//::std::string_view to ::std::string
            if(::std::regex_match(name,group_name_regex)){
                ret.emplace_back(::vnitest::execute_group(group_name));
            }
        }
    );
    return ret;
}

::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        group_name_list
)noexcept{
    ::std::vector<::vnitest::ExecuteGroupInfo> ret={};
    for(auto const& element:group_name_list){
        if(::std::holds_alternative<::std::string_view>(element)){
            auto const& group_name=::std::get<::std::string_view>(element);
            ret.emplace_back(::vnitest::execute_group(group_name));
        }else{
            auto const& group_regex=::std::get<::std::regex>(element);
            auto ri=::vnitest::execute_group(group_regex);
            for(auto const& info:ri){
                ret.emplace_back(info);
            }
        }
    }
    return ret;
}

::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        group_name_list
)noexcept{
    return ::vnitest::execute_group(
        ::std::vector<::std::variant<::std::string_view,::std::regex>>{
            group_name_list
        }
    );
}

::std::vector<::vnitest::ExecuteGroupInfo> execute_group_all(void)noexcept{
    ::std::vector<::vnitest::ExecuteGroupInfo> ret={};
    ::vnitest::GroupDict::get().foreach(
        [&](::std::string_view group_name,auto const& group_body)
        #ifndef _MSC_VER
            noexcept
        #endif
        {
            ret.emplace_back(::vnitest::execute_group(group_name));
        }
    );
    return ret;
}

::std::string execute_case_to_json(
    ::std::string_view case_name
)noexcept{
    return ::vnitest::to_json(::vnitest::execute_case(case_name));
}

::std::string execute_case_to_json(
    ::std::regex const& case_name_regex
)noexcept{
    return ::vnitest::to_json(::vnitest::execute_case(case_name_regex));
}

::std::string execute_case_to_json(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        case_name_list
)noexcept{
    return ::vnitest::to_json(::vnitest::execute_case(case_name_list));
}

::std::string execute_case_to_json(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        case_name_list
)noexcept{
    return ::vnitest::execute_case_to_json(
        ::std::vector<::std::variant<::std::string_view,::std::regex>>{
            case_name_list
        }
    );
}

::std::string execute_case_all_to_json(void)noexcept{
    return ::vnitest::to_json(::vnitest::execute_case_all());
}

::std::string execute_group_to_json(
    ::std::string_view group_name
)noexcept{
    return ::vnitest::to_json(::vnitest::execute_group(group_name));
}

::std::string execute_group_to_json(
    ::std::regex const& group_name_regex
)noexcept{
    return
        ::vnitest::to_json(::vnitest::execute_group(group_name_regex));
}

::std::string execute_group_to_json(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        group_name_list
)noexcept{
    return
        ::vnitest::to_json(::vnitest::execute_group(group_name_list));
}

::std::string execute_group_to_json(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        group_name_list
)noexcept{
    return ::vnitest::execute_group_to_json(
        ::std::vector<::std::variant<::std::string_view,::std::regex>>{
            group_name_list
        }
    );
}

::std::string execute_group_all_to_json(void)noexcept{
    return ::vnitest::to_json(::vnitest::execute_group_all());
}

}//namespace vnitest
