# `vnitest`

`vnitest` is a simple unit testing library.

The `vnitest` library is written using the `C++20` language standard and is implemented solely using the functionalities of the `C++20` standard library.

## Dependencies
The `vnitest` library only depends on the `C++20` standard library and has no other third-party dependencies.
### Compiler Extensions
The `vnitest` library uses the following compiler extension features, listed here:

`MSVC` compiler: `_MSC_VER`, `__COUNTER__`, `__FUNCSIG__`

Other compilers: `__COUNTER__`
## Quick Start
### Including the Library
```cpp
#include <vnitest/vnitest.h>
```
### Timer
The `vnitest` library provides a simple timer class `::vnitest::Timer` to measure the actual execution time of a code segment.
#### Basic Usage Example
Below is a usage example for `::vnitest::Timer`:
```cpp
::vnitest::Timer timer;
timer.start();
//... Code segment whose execution time needs to be measured
timer.stop();
::std::cout << ::std::format(
    "duration ns    :{}\n"
    "duration us    :{}\n"
    "duration ms    :{}\n"
    "duration s     :{}\n"
    "duration m     :{}\n"
    "duration h     :{}\n"
    "duration string:{}\n"
    , timer.duration_nanoseconds()
    , timer.duration_microseconds()
    , timer.duration_milliseconds()
    , timer.duration_seconds()
    , timer.duration_minutes()
    , timer.duration_hours()
    , timer.duration_string()
);
```
Possible output:
```txt
duration ns    :15127566
duration us    :15127.566
duration ms    :15.127566
duration s     :0.015127566
duration m     :0.0002521261
duration h     :4.2021016666666665e-06
duration string:15 ms 127 us 566 ns
```
#### Accumulative Timing Functionality
```cpp
::vnitest::Timer timer;
timer.start();
//... Code segment 1 to time
timer.stop();
//...do something
timer.start();
//... Code segment 2 to time
timer.stop();
// Print timing result (total execution time of segments 1 and 2)
::std::cout << timer.duration_string();
```
#### Reset Timing Functionality
```cpp
::vnitest::Timer timer;
timer.start();
//... Code segment 1 to time
timer.stop();
//...do something
timer.start();
// Clear previous timing results (does not change timer state)
// Placing this statement before the previous `timer.start()` would have the same effect
timer.reset();
//... Code segment 2 to time
timer.stop();
// Print timing result (execution time of segment 2)
::std::cout << timer.duration_string();
```
### Test Case
The `vnitest` library provides the `VNITEST_CASE()` macro function to define a test case.

#### Definition
`VNITEST_CASE()` supports string literals / raw string literals to represent a test case name.

Similarly, `VNITEST_CASE()` also supports empty strings as test case names.
```cpp
VNITEST_CASE("hello"){
    //... Your unit test code here
}
VNITEST_CASE(R"("hello world!")"){ // Equivalent to VNITEST_CASE("\"hello world!\"")
    //... Your unit test code here
}
VNITEST_CASE(""){
    //... Your unit test code here
}
```
Inside the test case, besides regular code, you can use various expressions provided by the `vnitest` library to check the execution results of the code.
#### Undefined Behavior
###### Defining `VNITEST_CASE` in Non-Global Scope
Use `VNITEST_CASE()` in the global scope to define test cases.

Defining `VNITEST_CASE` in other scopes, such as namespaces, functions, or classes, is undefined behavior.
###### Adding Function Modifiers to `VNITEST_CASE`
The following is an example of incorrect usage:
```cpp
inline static constexpr VNITEST_CASE("ha ha!") noexcept {
    //...
}
```
###### Defining Duplicate `VNITEST_CASE` Names
The following is an example of incorrect usage:
```cpp
VNITEST_CASE("foo"){
    //...
}
VNITEST_CASE("foo"){ // May cause compilation failure due to redefinition error
    //...
}
```
#### Executing Test Cases
###### Method 1: Execute Tests First, Then Customize Result Export Format
The `vnitest` library provides the following functions to find/execute test cases.
```cpp
namespace vnitest {

// Find and execute a test case by name
::vnitest::ExecuteCaseInfo execute_case(
    ::std::string_view case_name
) noexcept;

// Find and execute test cases whose names match a regular expression pattern
::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::regex const& case_name_regex
) noexcept;

// Find and sequentially execute a sequence of test cases specified by a list of names/regex patterns
::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::vector<::std::variant<::std::string_view, ::std::regex>> const&
        case_name_list
) noexcept;
::std::vector<::vnitest::ExecuteCaseInfo> execute_case(
    ::std::initializer_list<::std::variant<::std::string_view, ::std::regex>>
        case_name_list
) noexcept;

// Execute all test cases sequentially in the order they were defined
::std::vector<::vnitest::ExecuteCaseInfo> execute_case_all(void) noexcept;

} // namespace vnitest
```
The return type of these functions is `::vnitest::ExecuteCaseInfo` or a related sequence type.

The `ExecuteCaseInfo` class provides rich member access interfaces for customizing the test result export format.

Of course, the `vnitest` library also provides a default export interface `::vnitest::to_json()` to convert `ExecuteCaseInfo` and its sequence types to `JSON` stored in a `::std::string`.

###### Method 2: Execute Tests and Directly Export as `JSON`
To simplify the test result export process, the `vnitest` library provides corresponding derivative function interfaces for each function in Method 1.
The naming format is:

`execute_case*` `->` `execute_case*_to_json`

The return type of the `execute_case*_to_json` function interfaces is `::std::string`.

Calling the `execute_case*_to_json` function interfaces will execute the test cases and export the results as `JSON`, stored in the returned `::std::string`.
#### Runtime Checks
`VNITEST_RUNTIME_CHECK*` expressions are used to check if a condition expression holds true.

When the condition expression evaluates to `false`, relevant information is recorded. However, it does not interrupt the execution of the current test case.
###### Logical Expressions
The `vnitest` library provides the following macro functions to check the result of a logical expression.
```cpp
// check(condition)
VNITEST_RUNTIME_CHECK(condition);
// check(condition_1 && condition_2)
VNITEST_RUNTIME_CHECK_AND(condition_1, condition_2);
// check(condition_1 || condition_2)
VNITEST_RUNTIME_CHECK_OR(condition_1, condition_2);
// check(!condition)
VNITEST_RUNTIME_CHECK_NOT(condition);
```
###### Value Comparison Expressions
The `vnitest` library provides the following macro functions to check the result of a value comparison expression.
```cpp
// check(value_1 == value_2)
VNITEST_RUNTIME_CHECK_EQ(value_1, value_2);
// check(value_1 != value_2)
VNITEST_RUNTIME_CHECK_NE(value_1, value_2);
// check(value_1 > value_2)
VNITEST_RUNTIME_CHECK_GT(value_1, value_2);
// check(value_1 >= value_2)
VNITEST_RUNTIME_CHECK_GE(value_1, value_2);
// check(value_1 < value_2)
VNITEST_RUNTIME_CHECK_LT(value_1, value_2);
// check(value_1 <= value_2)
VNITEST_RUNTIME_CHECK_LE(value_1, value_2);
```
###### How to Obtain Test Results
If `VNITEST_RUNTIME_CHECK*` expressions are defined within the scope of a `VNITEST_CASE(){}` definition, the `vnitest` library provides `execute_*` functions to execute `VNITEST_CASE`/`VNITEST_GROUP` and return `ExecuteCaseInfo`/`ExecuteGroupInfo` class objects that record the execution result information.

Subsequently, use `::vnitest::to_json()` to convert the class objects into `JSON` of type `::std::string`.

Then, you can use `IO` streams / file streams to print test results to the terminal / write to files, etc.

Of course, to simplify this cumbersome test result export process, the `vnitest` library provides `execute_*_to_json` functions for one-step `JSON` export.

Similarly, `VNITEST_RUNTIME_CHECK*` expressions also support standalone use. If a check fails, the test result will display an error message at the terminal during execution, and it will not block the continued execution of the program.

###### Custom Messages and Custom Type Information
```cpp
VNITEST_CASE("mytest"){
    VNITEST_RUNTIME_CHECK(1+1 != 2) << int{1} << '+' << int{1} << "!=2?";
    VNITEST_RUNTIME_CHECK_AND(true, false) << ::std::format("check {} && {}", true, false);
    VNITEST_RUNTIME_CHECK_OR(false, false) << "this is msg";
    VNITEST_RUNTIME_CHECK_NOT(true) << "";
    VNITEST_RUNTIME_CHECK_EQ(1, 2) << "1 == 2?";
    VNITEST_RUNTIME_CHECK_NE(1, 1) << "1 != 1?";
    VNITEST_RUNTIME_CHECK_GT(1, 2) << "1 > 2?";
    VNITEST_RUNTIME_CHECK_GE(1, 2) << "1 >= 2?";
    VNITEST_RUNTIME_CHECK_LT(2, 1) << "2 < 1?";
    VNITEST_RUNTIME_CHECK_LE(2, 1) << "2 <= 1?";
}
```
The custom type information functionality of the `VNITEST_RUNTIME_CHECK*` macros depends on `::vnitest::OStringStream`, whose implementation in turn depends on `::std::ostringstream`.

However, `::std::ostringstream` does not support conversion of all types to strings. Therefore, the `vnitest` library provides the following solution to support custom type information:

To enable `::vnitest::OStringStream` to support information for custom types, you can define custom `operator<<` overload functions for `::vnitest::OStringStream`.

Here is a concrete example:
```cpp
struct Person {
    ::std::string_view name;
    short age;
};
::vnitest::OStringStream&
operator<<(::vnitest::OStringStream& oss, Person const& p) noexcept {
    return oss << ::std::format("Person{{name:{},age:{}}}", p.name, p.age);
}
VNITEST_CASE("test.PersonCmp.RuntimeCheckFailed"){
    Person p1{"Tom", 20};
    Person p2{"Jerry", 10};
    Person p3{"Tom", 20};
    VNITEST_RUNTIME_CHECK(p1 != p3); // Fails
    VNITEST_RUNTIME_CHECK(p1 == p2); // Fails
    VNITEST_RUNTIME_CHECK_NOT(p1 == p3); // Fails (p1 == p3 is true, NOT true is false)
    VNITEST_RUNTIME_CHECK_NOT(p1 != p2); // Fails (p1 != p2 is true, NOT true is false)
    VNITEST_RUNTIME_CHECK_EQ(p1, p2); // Fails
    VNITEST_RUNTIME_CHECK_NE(p1, p3); // Fails
}
```
Possible test export result:
```txt
{
    "case name": "test.PersonCmp.RuntimeCheckFailed",
    "duration ns": 16853,
    "duration string": "16 us 853 ns",
    "runtime check total": 6,
    "runtime check passed": 0,
    "runtime check failed": 6,
    "runtime check passed rate": 0,
    "runtime assert total": 0,
    "runtime assert passed": 0,
    "runtime assert failed": 0,
    "runtime assert passed rate": 1,
    "is passed": false,
    "is failed": true,
    "is skipped": false,
    "is undefined": false,
    "runtime check failed errors": [
        {
            "file": "vnitest/tests/sample7.h",
            "line": "43",
            "info": "(true) == (p1!=p3)"
        },
        {
            "file": "vnitest/tests/sample7.h",
            "line": "44",
            "info": "(true) == (p1==p2)"
        },
        {
            "file": "vnitest/tests/sample7.h",
            "line": "45",
            "info": "(false) == (p1==p3)"
        },
        {
            "file": "vnitest/tests/sample7.h",
            "line": "46",
            "info": "(false) == (p1!=p2)"
        },
        {
            "file": "vnitest/tests/sample7.h",
            "line": "47",
            "info": "(Person{name:Tom,age:20}) == (Person{name:Jerry,age:10})"
        },
        {
            "file": "vnitest/tests/sample7.h",
            "line": "48",
            "info": "(Person{name:Tom,age:20}) != (Person{name:Tom,age:20})"
        }
    ]
}
```
###### Standalone Use (Outside `VNITEST_CASE(){}` Scope)
`VNITEST_RUNTIME_CHECK*` expressions support use outside the `VNITEST_CASE(){}` definition scope, within other code blocks.

The check result is evaluated when the expression is executed. If the check fails, an error message will be displayed on the terminal, but it will not block the continued execution of the current program.

Here is a concrete example:
```cpp
void my_test_func(void) noexcept {
    VNITEST_RUNTIME_CHECK(1+1 != 2) << "this is msg"; // Check fails
}
int main(void) {
    my_test_func();
    return 0;
}
```
Possible output:
```txt
{
    "file": "vnitest/tests/sample10.h",
    "line": "8",
    "info": "(true) == (1+1!=2)",
    "msg": "this is msg"
}
```
#### Runtime Asserts
`VNITEST_RUNTIME_ASSERT*` expressions are used to check if a condition expression holds true.

When the condition expression evaluates to `false`, relevant information is recorded and the execution of the current test case is interrupted.
###### Logical Expressions
The `vnitest` library provides the following macro functions to check the result of a logical expression.
```cpp
// assert(condition)
VNITEST_RUNTIME_ASSERT(condition);
// assert(condition_1 && condition_2)
VNITEST_RUNTIME_ASSERT_AND(condition_1, condition_2);
// assert(condition_1 || condition_2)
VNITEST_RUNTIME_ASSERT_OR(condition_1, condition_2);
// assert(!condition)
VNITEST_RUNTIME_ASSERT_NOT(condition);
```
###### Value Comparison Expressions
The `vnitest` library provides the following macro functions to check the result of a value comparison expression.
```cpp
// assert(value_1 == value_2)
VNITEST_RUNTIME_ASSERT_EQ(value_1, value_2);
// assert(value_1 != value_2)
VNITEST_RUNTIME_ASSERT_NE(value_1, value_2);
// assert(value_1 > value_2)
VNITEST_RUNTIME_ASSERT_GT(value_1, value_2);
// assert(value_1 >= value_2)
VNITEST_RUNTIME_ASSERT_GE(value_1, value_2);
// assert(value_1 < value_2)
VNITEST_RUNTIME_ASSERT_LT(value_1, value_2);
// assert(value_1 <= value_2)
VNITEST_RUNTIME_ASSERT_LE(value_1, value_2);
```
###### How to Obtain Test Results
Similar to `VNITEST_RUNTIME_CHECK*`, if `VNITEST_RUNTIME_ASSERT*` expressions are defined within the scope of a `VNITEST_CASE(){}` definition, use the `vnitest` library's provided `execute_*` / `execute_*_to_json` functions as previously described to export test results.

Similarly, `VNITEST_RUNTIME_ASSERT*` expressions also support standalone use. If an assertion fails, the test result will, upon execution, throw a `::vnitest::RuntimeAssertFailedException` exception. If this exception is not caught, it will display an assertion failure error message on the terminal and halt the execution of the current program.

###### Custom Messages and Custom Type Information
Similar to `VNITEST_RUNTIME_CHECK*`, `VNITEST_RUNTIME_ASSERT*` also supports custom messages and custom type information functionality, which will not be elaborated here.
```cpp
VNITEST_CASE("mytest"){
    VNITEST_RUNTIME_ASSERT(1+1 != 2) << int{1} << '+' << int{1} << "!=2?"; // Fails, aborts case
    VNITEST_RUNTIME_ASSERT_AND(true, false) << ::std::format("assert {} && {}", true, false); // Would fail if reached
    // ... other asserts would not be reached due to first assert failure
}
```
###### Standalone Use (Outside `VNITEST_CASE(){}` Scope)
`VNITEST_RUNTIME_ASSERT*` expressions support use outside the `VNITEST_CASE(){}` definition scope, within other code blocks. The test result is evaluated when the expression is executed.

If the assertion fails, it throws a `::vnitest::RuntimeAssertFailedException` exception. If this exception is not caught, it will display an assertion failure error message on the terminal and halt the execution of the current program.
```cpp
void my_test_func(void) noexcept {
    VNITEST_RUNTIME_ASSERT(1+1 != 2) << "this is msg"; // Fails, throws
}
int main(void) {
    my_test_func(); // Exception thrown here, uncaught -> terminate
    return 0;
}
```
Possible output:
```txt
terminate called after throwing an instance of 'vnitest::RuntimeAssertFailedException'
  what():  [FILE] vnitest/tests/sample10.h [LINE] 12 [INFO] (true) == (1+1!=2) [MSG] this is msg
Aborted
```
#### Skip
In addition to the above two expressions, `vnitest` also provides the `VNITEST_SKIP()` macro function to exit the current test case early under certain circumstances.

Note: `VNITEST_SKIP()` does not support standalone use outside the `VNITEST_CASE(){}` definition scope.

When `VNITEST_SKIP()` is executed, relevant information is recorded. The macro function parameters support empty input `()` or `("")`, or specifying a custom reason string.

The reason string supports the following two formats:
1. String literal
2. A value that can be assigned to `::std::string`

Here is a concrete usage example:
```cpp
VNITEST_CASE("test_file"){
    ::std::ofstream ofs("input.txt");
    if (!ofs.is_open()) {
        ::std::string_view reason = {"File open failed"};
        VNITEST_SKIP(reason);
    }
    //... File testing code (skipped if file didn't open)
}
```
###### Custom Messages and Custom Type Information
Similar to `VNITEST_RUNTIME_CHECK*` / `VNITEST_RUNTIME_ASSERT*`, `VNITEST_SKIP` also supports custom messages and custom type information functionality, which will not be elaborated here.
```cpp
VNITEST_SKIP("this is reason")
    << "this is skip msg"
    << int{100}
    << ::std::format("fmt{}", double{0.0});
```
### Test Group
The `vnitest` library provides `VNITEST_GROUP` to combine zero or more `VNITEST_CASE`s.

Using `VNITEST_GROUP` offers the following advantages over using single or multiple individual `VNITEST_CASE`s:
1. Specify the execution order of `VNITEST_CASE` members.
2. Use regular expressions to match `VNITEST_CASE` names that meet criteria as members.
3. Allow duplicate `VNITEST_CASE` members.
#### Definition
`VNITEST_GROUP()` supports string literals / raw string literals to represent a test group name.

Similarly, `VNITEST_GROUP()` also supports empty strings as test group names.
```cpp
VNITEST_GROUP("hello"){
    //... Statements to add test case members to this group go here
}
VNITEST_GROUP(R"("hello world!")"){ // Equivalent to VNITEST_GROUP("\"hello world!\"")
    //... Statements to add test case members to this group go here
}
VNITEST_GROUP(""){
    //... Statements to add test case members to this group go here
}
```
###### Adding Test Case Members
Inside the test group, use the `vnitest` library's provided `VNITEST_GROUP_ELEMENT_CASE*` expressions to add test case members.

Note: The input parameters for `VNITEST_GROUP_ELEMENT_CASE*()` can only use string literals / raw string literals.
```cpp
// Add a test case to the group by its name
VNITEST_GROUP_ELEMENT_CASE(case_name_literal);
// Find and add test cases matching the pattern via regular expression
VNITEST_GROUP_ELEMENT_CASE_REGEX(case_name_pattern_literal);
// Add a test case to the group by its name, and repeat it integer_literal times
VNITEST_GROUP_ELEMENT_CASE_REPEAT(integer_literal, case_name_literal);
// Find and add test cases matching the pattern via regular expression, and repeat them integer_literal times
VNITEST_GROUP_ELEMENT_CASE_REGEX_REPEAT(integer_literal, case_name_pattern_literal);
```
Here is a concrete usage example:
```cpp
VNITEST_GROUP("test Person"){
    VNITEST_GROUP_ELEMENT_CASE("test Person name");
    VNITEST_GROUP_ELEMENT_CASE("test Person age");
    VNITEST_GROUP_ELEMENT_CASE_REGEX(R"(^test Person .*)");
    VNITEST_GROUP_ELEMENT_CASE_REPEAT(2, "test Person name");
    VNITEST_GROUP_ELEMENT_CASE_REGEX_REPEAT(3, R"(^test Person .*)");
}
```
#### Undefined Behavior
###### Defining `VNITEST_GROUP` in Non-Global Scope
Use `VNITEST_GROUP()` in the global scope to define test groups.

Defining `VNITEST_GROUP` in other scopes, such as namespaces, functions, or classes, is undefined behavior.
###### Adding Function Modifiers to `VNITEST_GROUP`
The following is an example of incorrect usage:
```cpp
inline static constexpr VNITEST_GROUP("ha ha!") noexcept {
    //...
}
```
###### Defining Duplicate `VNITEST_GROUP` Names
The following is an example of incorrect usage:
```cpp
VNITEST_GROUP("foo"){
    //...
}
VNITEST_GROUP("foo"){ // May cause compilation failure due to redefinition error
    //...
}
```
###### Writing Irrelevant Statements (Non-Member-Adding Statements) Inside `VNITEST_GROUP(){}`
The following is an example of incorrect usage:
```cpp
VNITEST_GROUP("foo"){
    ::std::cout << "hello world\n"; // Not allowed
}
```
#### Executing Test Groups
###### Method 1: Execute Tests First, Then Customize Result Export Format
The `vnitest` library provides the following functions to find/execute test groups.
```cpp
namespace vnitest {

// Find and execute a test group by name
::vnitest::ExecuteGroupInfo execute_group(
    ::std::string_view group_name
) noexcept;

// Find and execute test groups whose names match a regular expression pattern
::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::regex const& group_name_regex
) noexcept;

// Find and sequentially execute a sequence of test groups specified by a list of names/regex patterns
::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::vector<::std::variant<::std::string_view, ::std::regex>> const&
        group_name_list
) noexcept;
::std::vector<::vnitest::ExecuteGroupInfo> execute_group(
    ::std::initializer_list<::std::variant<::std::string_view, ::std::regex>>
        group_name_list
) noexcept;

// Execute all test groups sequentially in the order they were defined
::std::vector<::vnitest::ExecuteGroupInfo> execute_group_all(void) noexcept;

} // namespace vnitest
```
The return type of these functions is `::vnitest::ExecuteGroupInfo` or a related sequence type.

The `ExecuteGroupInfo` class provides rich member access interfaces for customizing the test result export format.

Of course, the `vnitest` library also provides a default export interface `::vnitest::to_json()` to convert `ExecuteGroupInfo` and its sequence types to `JSON` stored in a `::std::string`.

###### Method 2: Execute Tests and Directly Export as `JSON`
To simplify the test result export process, the `vnitest` library provides corresponding derivative function interfaces for each function in Method 1.
The naming format is:

`execute_group*` `->` `execute_group*_to_json`

The return type of the `execute_group*_to_json` function interfaces is `::std::string`.

Calling the `execute_group*_to_json` function interfaces will execute the test groups and export the results as `JSON`, stored in the returned `::std::string`.
### Type Information
#### Get Compile-Time Type Name String
`vnitest` provides `::vnitest::type_name_of` to get the name string of a type.

The return value is of type `constexpr ::std::string_view`.
```cpp
::std::cout
    << ::vnitest::type_name_of<int const volatile &>()
    << ::std::endl;
// Output: "int const volatile &"
```
#### Get Variable's Compile-Time Type Name String
Similarly, `vnitest` provides type name export for variables.

The `vnitest` library provides `VNITEST_TYPE_NAME_OF_VARIABLE` to get the real type name string of a variable.

The return value is of type `constexpr ::std::string_view`.
```cpp
int const volatile num;
int const volatile& ref = num;
::std::cout
    << VNITEST_TYPE_NAME_OF_VARIABLE(ref)
    << ::std::endl;
// Output: "int const volatile &"
```
### Compile-Time Checks
To support testing template metaprogramming code segments, the `vnitest` library provides an alternative effective compile-time checking method besides `static_assert`.

The `vnitest` library provides `VNITEST_STATIC_CHECK*` expressions for performing compile-time static checks.

Unlike `static_assert`, `VNITEST_STATIC_CHECK*(false)` does not cause compilation to fail. Instead, it displays a warning message at compile time and then continues compilation.

Here is a concrete example:
```cpp
// In a header/source file
VNITEST_STATIC_CHECK(100 == 200); // Condition is false
```
Here is the warning information displayed by the `GCC` compiler during compilation when testing this statement:

```txt
vnitest/tests/sample6.h: In lambda function:
vnitest/include/vnitest/static_check.hpp:87:25: warning:
‘constexpr bool vnitest::detail::StaticCheck<File__, Line__, Info__>::operator()
(vnitest::detail::Bool<false>) const [
    with File__ = vnitest::detail::File<
        vnitest::StaticString<vnitest::StaticCStr<char, 41>{
            "vnitest/tests/sample6.h"
        }>
    >;
    Line__ = vnitest::detail::Line<34>;
    Info__ = vnitest::detail::Info<
        vnitest::StaticString<vnitest::StaticCStr<char, 21>{
            "(true) == (100==200)"
        }>
    >
]’
is deprecated: ::vnitest::static_check(false) [-Wdeprecated-declarations]
   87 |             static_check(result) \
      |             ~~~~~~~~~~~~^~~~~~~~
vnitest/tests/sample6.h:34:1: note: in expansion of macro ‘VNITEST_STATIC_CHECK’
   34 | VNITEST_STATIC_CHECK(100==200);
      | ^~~~~~~~~~~~~~~~~~~~
vnitest/include/vnitest/static_check.hpp:24:20: note: declared here
   24 |     constexpr bool operator()(::vnitest::detail::Bool<false>) const noexcept {
      |                    ^~~~~~~~
```
#### Scope of `VNITEST_STATIC_CHECK*` Statements
`VNITEST_STATIC_CHECK*` statements support definition and use in global/namespace scope, inside functions, and inside classes.

Here are specific usage examples:
```cpp
// Use in global scope
VNITEST_STATIC_CHECK("global scope", false);

void sample5_func(void) noexcept {
    // Use inside global function
    VNITEST_STATIC_CHECK("global function scope", false);
    []() {
        // Use inside lambda inside global function
        VNITEST_STATIC_CHECK("lambda scope in global function", false);
    }();
    struct LocalClass {
// Cannot use inside local class inside global function (local type in non-constexpr context)
//        VNITEST_STATIC_CHECK("local class scope in global function", false);
    };
}

auto lambda_func = []() {
    // Use inside global lambda
    VNITEST_STATIC_CHECK("global lambda scope", false);
};

class Sample5_Class {
    // Use inside global class
    VNITEST_STATIC_CHECK("global class scope", false);
    struct LocalClass {
        // Use inside local class inside global class
        VNITEST_STATIC_CHECK("local class scope in global class", false);
    };
    void member_func(void) noexcept {
        // Use inside member function of global class
        VNITEST_STATIC_CHECK("global class member function scope", false);
        []() {
            // Use inside lambda inside member function of global class
            VNITEST_STATIC_CHECK("lambda scope in global class member function", false);
        }();
        struct LocalClass {
// Cannot use inside local class inside member function (local type in non-constexpr context)
//            VNITEST_STATIC_CHECK("local class scope in global class member function", false);
        };
    }
};

namespace tests {
// Use in namespace scope
VNITEST_STATIC_CHECK("namespace scope", false);

void sample5_func(void) noexcept {
    // Use inside function in namespace
    VNITEST_STATIC_CHECK("namespace function scope", false);
    []() {
        // Use inside lambda inside function in namespace
        VNITEST_STATIC_CHECK("lambda scope in namespace function", false);
    }();
    struct LocalClass {
// Cannot use inside local class inside function (local type in non-constexpr context)
//        VNITEST_STATIC_CHECK("local class scope in namespace function", false);
    };
}

auto lambda_func = []() {
    // Use inside lambda in namespace
    VNITEST_STATIC_CHECK("namespace lambda scope", false);
};

class Sample5_Class {
    // Use inside class in namespace
    VNITEST_STATIC_CHECK("namespace class scope", false);
    struct LocalClass {
        // Use inside local class inside class in namespace
        VNITEST_STATIC_CHECK("local class scope in namespace class", false);
    };
    void member_func(void) noexcept {
        // Use inside member function of class in namespace
        VNITEST_STATIC_CHECK("namespace class member function scope", false);
        []() {
            // Use inside lambda inside member function of class in namespace
            VNITEST_STATIC_CHECK("lambda scope in namespace class member function", false);
        }();
        struct LocalClass {
// Cannot use inside local class inside member function (local type in non-constexpr context)
//            VNITEST_STATIC_CHECK("local class scope in namespace class member function", false);
        };
    }
};

} // namespace tests
```
#### Special Usage Technique for Single-Parameter `VNITEST_STATIC_CHECK*()`
The parameter list of `VNITEST_STATIC_CHECK()`(in principle) can only take one `constexpr bool` compile-time expression, but the usage in the previous section input two parameters.

Actually, a comma operator trick is used here:

The comma operator `,` can connect two different expressions, but the output result is the result of the last expression.

`VNITEST_STATIC_CHECK()` exports the entire content inside `()` as a string for the hint information, thus including all content before the comma operator.

Therefore, this trick is used here to make `VNITEST_STATIC_CHECK()` achieve a usage effect similar to `static_assert(expr, msg)`,just reversed the order of the `msg` input parameter.

Similarly, `VNITEST_STATIC_CHECK_NOT()`, which can only take one input parameter, also supports the above usage.

However, other `VNITEST_STATIC_CHECK*()` that support two input parameters cannot use this trick (as they already expect two specific expressions).
#### Compile-Time Value Logical Expressions
Besides `VNITEST_STATIC_CHECK`, the `vnitest` library provides other `VNITEST_STATIC_CHECK*` macros for checking compile-time logical expressions.
```cpp
// check(condition)
VNITEST_STATIC_CHECK(condition);
// check(!condition)
VNITEST_STATIC_CHECK_NOT(condition);
// check(condition_1 && condition_2)
VNITEST_STATIC_CHECK_VALUE_AND(condition_1, condition_2);
// check(condition_1 || condition_2)
VNITEST_STATIC_CHECK_VALUE_OR(condition_1, condition_2);
```
#### Compile-Time Value Comparison Expressions
Similarly, the `vnitest` library provides other `VNITEST_STATIC_CHECK*` macros for checking the relational comparison of two compile-time values.
```cpp
// check(value_1 == value_2)
VNITEST_STATIC_CHECK_VALUE_EQ(value_1, value_2);
// check(value_1 != value_2)
VNITEST_STATIC_CHECK_VALUE_NE(value_1, value_2);
// check(value_1 > value_2)
VNITEST_STATIC_CHECK_VALUE_GT(value_1, value_2);
// check(value_1 >= value_2)
VNITEST_STATIC_CHECK_VALUE_GE(value_1, value_2);
// check(value_1 < value_2)
VNITEST_STATIC_CHECK_VALUE_LT(value_1, value_2);
// check(value_1 <= value_2)
VNITEST_STATIC_CHECK_VALUE_LE(value_1, value_2);
```
#### Compile-Time Type Comparison Expressions
To facilitate testing template metaprogramming code, the `vnitest` library provides other `VNITEST_STATIC_CHECK*` macros for checking the similarity or difference of two types (types include possible `cv ref` qualifiers).
```cpp
// check(true == ::std::is_same_v<Type1, Type2>)
VNITEST_STATIC_CHECK_TYPE_EQ(Type1, Type2);
// check(false == ::std::is_same_v<Type1, Type2>)
VNITEST_STATIC_CHECK_TYPE_NE(Type1, Type2);
```
A concrete usage example:
```cpp
VNITEST_STATIC_CHECK_TYPE_EQ(int, double); // false -> warning
VNITEST_STATIC_CHECK_TYPE_EQ(int, int const); // false -> warning
VNITEST_STATIC_CHECK_TYPE_EQ(int, int volatile); // false -> warning
VNITEST_STATIC_CHECK_TYPE_EQ(int, int&); // false -> warning
VNITEST_STATIC_CHECK_TYPE_EQ(int, int&&); // false -> warning
VNITEST_STATIC_CHECK_TYPE_NE(int, int); // false (is_same_v is true, NE checks for false == true -> false) -> warning
```
