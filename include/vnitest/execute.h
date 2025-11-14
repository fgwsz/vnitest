#ifndef VNITEST_EXECUTE_H
#define VNITEST_EXECUTE_H

#include<string_view>//::std::string_view
#include<vector>//::std::vector
#include<regex>//::std::regex
#include<variant>//::std::variant
#include<initializer_list>//::std::initializer_list

#include<vnitest/execute_case_info.h>
#include<vnitest/execute_group_info.h>

namespace vnitest{

::vnitest::ExecuteCaseInfo execute_case(
    ::std::string_view case_name
)noexcept;

::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::regex const& case_name_regex
)noexcept;

::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        case_name_list
)noexcept;

::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        case_name_list
)noexcept;

::std::vector<::vnitest::ExecuteCaseInfo> execute_case_all(void)noexcept;

::vnitest::ExecuteGroupInfo execute_group(
    ::std::string_view group_name
)noexcept;

::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::regex const& group_name_regex
)noexcept;

::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        group_name_list
)noexcept;

::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        group_name_list
)noexcept;

::std::vector<::vnitest::ExecuteGroupInfo> execute_group_all(void)noexcept;

::std::string execute_case_to_json(
    ::std::string_view case_name
)noexcept;

::std::string execute_case_to_json(
    ::std::regex const& case_name_regex
)noexcept;

::std::string execute_case_to_json(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        case_name_list
)noexcept;

::std::string execute_case_to_json(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        case_name_list
)noexcept;

::std::string execute_case_all_to_json(void)noexcept;

::std::string execute_group_to_json(
    ::std::string_view group_name
)noexcept;

::std::string execute_group_to_json(
    ::std::regex const& group_name_regex
)noexcept;

::std::string execute_group_to_json(
    ::std::vector<::std::variant<::std::string_view,::std::regex>> const&
        group_name_list
)noexcept;

::std::string execute_group_to_json(
    ::std::initializer_list<::std::variant<::std::string_view,::std::regex>>
        group_name_list
)noexcept;

::std::string execute_group_all_to_json(void)noexcept;

}//namespace vnitest

#endif//VNITEST_EXECUTE_H
