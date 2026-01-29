#ifndef VNITEST_H
#define VNITEST_H
//=============================================================================
//#include<std>
//=============================================================================
//C std
#include<cstddef>//::std::size_t
#include<cmath>//::std::isnan ::std::isinf ::std::isnormal
//C++ 98/03
#include<ios>//std::boolalpha ::std::hex ::std::uppercase
#include<iomanip>//::std::setw ::std::setfill
#include<iostream>//::std::cerr ::std::endl
#include<sstream>//::std::ostringstream
#include<limits>//::std::numeric_limits
#include<utility>//::std::forward ::std::move
#include<iterator>//::std::begin ::std::end
#include<algorithm>//::std::copy
#include<string>//::std::string ::std::to_string
#include<list>//::std::list
#include<vector>//::std::vector
#include<functional>//::std::function
#include<exception>//::std::exception
#include<stdexcept>//::std::runtime_error
//C++ 11
#include<type_traits>//::std::remove_cvref_t ::std::is_same_v ::std::decay_t
#include<initializer_list>//::std::initializer_list
#include<array>//::std::array
#include<unordered_map>//::std::unordered_map
#include<tuple>//::std::tuple
#include<regex>//::std::regex ::std::regex_match
#include<chrono>//::std::chrono
//C++ 17
#include<optional>//::std::optional ::std::nullopt
#include<variant>//::std::variant
#include<string_view>//::std::string_view
#include<charconv>//::std::to_chars
//C++ 20
#include<source_location>//::std::source_location
#include<format>//::std::format
//=============================================================================
//#include<vnitest/timer.h>
//=============================================================================
namespace vnitest{

class Timer{
public:
    Timer(Timer const&)noexcept=default;
    Timer(Timer&&)noexcept=default;
    Timer& operator=(Timer const&)noexcept=default;
    Timer& operator=(Timer&&)noexcept=default;
    ~Timer(void)noexcept=default;

    Timer(void)noexcept;
    //开始计时/结束暂停继续计时
    void start(void)noexcept;
    //暂停计时/停止计时
    void stop(void)noexcept;
    //重置累计时间但不改变当前计时器状态
    void reset(void)noexcept;
    //获取当前累计时间
    double duration_nanoseconds(void)const noexcept;
    double duration_microseconds(void)const noexcept;
    double duration_milliseconds(void)const noexcept;
    double duration_seconds(void)const noexcept;
    double duration_minutes(void)const noexcept;
    double duration_hours(void)const noexcept;
    //累计时间打印相关
    static ::std::string nanoseconds_to_string(double ns)noexcept;
    ::std::string duration_string(void)const noexcept;
    //计时器状态
    enum class State{
        STOPPED,//已停止
        RUNNING //运行中
    };
    //获取当前状态
    State get_state(void)const noexcept;
    bool is_stopped(void)const noexcept;
    bool is_running(void)const noexcept;
private:
    ::std::chrono::steady_clock::time_point start_time_;
    double total_duration_;
    State state_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/macro.h>
//=============================================================================
#define VNITEST_STRINGIFY(...) #__VA_ARGS__

#define VNITEST_TO_STRING(...) VNITEST_STRINGIFY(__VA_ARGS__)

#define VNITEST_CONCAT_IMPL(lhs__,...) lhs__##__VA_ARGS__

#define VNITEST_CONCAT(lhs__,...) VNITEST_CONCAT_IMPL(lhs__,__VA_ARGS__)

#ifdef __COUNTER__
#   define VNITEST_UNIQUE_VAR_NAME(...) \
        VNITEST_CONCAT(__VA_ARGS__,__COUNTER__)
#else
#   define VNITEST_UNIQUE_VAR_NAME(...) \
        VNITEST_CONCAT(__VA_ARGS__,__LINE__)
#endif
//=============================================================================
//#include<vnitest/ostring_stream.hpp>
//=============================================================================
namespace vnitest{

class OStringStream{
public:
    OStringStream(void)noexcept
        :data_({}){
        this->data_<<::std::boolalpha;
    }
    OStringStream& operator=(OStringStream const&)noexcept=default;
    OStringStream& operator=(OStringStream &&)noexcept=default;
    ~OStringStream(void)noexcept=default;

    template<typename Type__>requires requires{
        ::std::ostringstream{}<<Type__{};
    }
    OStringStream& operator<<(Type__&& arg)noexcept{
        this->data_<<::std::forward<Type__>(arg);
        return *this;
    }
    ::std::string str(void)const noexcept{
        return this->data_.str();
    }
private:
    ::std::ostringstream data_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/condition_info.h>
//=============================================================================
namespace vnitest{

struct ConditionInfo{
    ::std::string_view file,line;
    ::std::string info;
    bool condition;
};

}//namespace vnitest

#define VNITEST_MAKE_CONDITION_INFO(...) \
    ::vnitest::ConditionInfo{ \
        __FILE__ \
        ,VNITEST_TO_STRING(__LINE__) \
        ,VNITEST_TO_STRING(__VA_ARGS__) \
        ,static_cast<bool>(__VA_ARGS__) \
    } \
//
#define VNITEST_MAKE_CONDITION_INFO_BY_UNARY_OPERATOR(operator__,...) \
    [&](void)->::vnitest::ConditionInfo{ \
        auto const& rhs_ref=(__VA_ARGS__); \
        ::vnitest::OStringStream oss={}; \
        return ::vnitest::ConditionInfo{ \
            __FILE__ \
            ,VNITEST_TO_STRING(__LINE__) \
            ,( \
                oss \
                    <<VNITEST_TO_STRING(operator__) " " \
                    <<"("<<rhs_ref<<")" \
            ).str() \
            ,static_cast<bool>(operator__ rhs_ref) \
        }; \
    }() \
//
#define VNITEST_MAKE_CONDITION_INFO_BY_BINARY_OPERATOR( \
    operator__,lhs__,... \
) \
    [&](void)->::vnitest::ConditionInfo{ \
        auto const& lhs_ref=(lhs__); \
        auto const& rhs_ref=(__VA_ARGS__); \
        ::vnitest::OStringStream oss={}; \
        return ::vnitest::ConditionInfo{ \
            __FILE__ \
            ,VNITEST_TO_STRING(__LINE__) \
            ,( \
                oss \
                    <<"("<<lhs_ref<<")" \
                    <<" " VNITEST_TO_STRING(operator__) " " \
                    <<"("<<rhs_ref<<")" \
            ).str() \
            ,static_cast<bool>(lhs_ref operator__ rhs_ref) \
        }; \
    }() \
//
//=============================================================================
//#include<vnitest/runtime_check_stream.hpp>
//=============================================================================
namespace vnitest{

class RuntimeCheckStream final{
public:
    template<typename Type__>
    RuntimeCheckStream& operator<<(Type__&& arg)noexcept{
        ::vnitest::OStringStream oss={};
        ::std::string str=(oss<<::std::forward<Type__>(arg)).str();
        if(this->message_.has_value()){
            this->message_.value()+=str;
        }else{
            this->message_=str;
        }
        return *this;
    }
    RuntimeCheckStream(::vnitest::ConditionInfo const& ci)noexcept;
    ~RuntimeCheckStream(void)noexcept;
private:
    ::vnitest::ConditionInfo info_;
    ::std::optional<::std::string> message_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/runtime_check.h>
//=============================================================================
#define VNITEST_RUNTIME_CHECK(...) \
    ::vnitest::RuntimeCheckStream( \
        VNITEST_MAKE_CONDITION_INFO((true) == (__VA_ARGS__)) \
    ) \
//
#define VNITEST_RUNTIME_CHECK_UNARY_OPERATOR(operator__,...) \
    ::vnitest::RuntimeCheckStream( \
        VNITEST_MAKE_CONDITION_INFO_BY_UNARY_OPERATOR( \
            operator__,__VA_ARGS__ \
        ) \
    ) \
//
#define VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(operator__,lhs__,...) \
    ::vnitest::RuntimeCheckStream( \
        VNITEST_MAKE_CONDITION_INFO_BY_BINARY_OPERATOR( \
            operator__,lhs__,__VA_ARGS__ \
        ) \
    ) \
//
#define VNITEST_RUNTIME_CHECK_EQ(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(==,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_NE(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(!=,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_GT(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(>,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_GE(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(>=,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_LT(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(<,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_LE(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(<=,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_AND(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(&&,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_OR(lhs__,...) \
    VNITEST_RUNTIME_CHECK_BINARY_OPERATOR(||,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_CHECK_NOT(...) \
    ::vnitest::RuntimeCheckStream( \
        VNITEST_MAKE_CONDITION_INFO((false) == (__VA_ARGS__)) \
    ) \
//
//=============================================================================
//#include<vnitest/runtime_assert_stream.hpp>
//=============================================================================
namespace vnitest{

class RuntimeAssertStream final{
public:
    template<typename Type__>
    RuntimeAssertStream& operator<<(Type__&& arg)noexcept{
        ::vnitest::OStringStream oss={};
        ::std::string str=(oss<<::std::forward<Type__>(arg)).str();
        if(this->message_.has_value()){
            this->message_.value()+=str;
        }else{
            this->message_=str;
        }
        return *this;
    }
    RuntimeAssertStream(::vnitest::ConditionInfo const& ci)noexcept;
    ~RuntimeAssertStream(void)noexcept(false);
private:
    ::vnitest::ConditionInfo info_;
    ::std::optional<::std::string> message_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/runtime_assert.h>
//=============================================================================
#define VNITEST_RUNTIME_ASSERT(...) \
    ::vnitest::RuntimeAssertStream( \
        VNITEST_MAKE_CONDITION_INFO((true) == (__VA_ARGS__)) \
    ) \
//
#define VNITEST_RUNTIME_ASSERT_UNARY_OPERATOR(operator__,...) \
    ::vnitest::RuntimeAssertStream( \
        VNITEST_MAKE_CONDITION_INFO_BY_UNARY_OPERATOR( \
            operator__,__VA_ARGS__ \
        ) \
    ) \
//
#define VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(operator__,lhs__,...) \
    ::vnitest::RuntimeAssertStream( \
        VNITEST_MAKE_CONDITION_INFO_BY_BINARY_OPERATOR( \
            operator__,lhs__,__VA_ARGS__ \
        ) \
    ) \
//
#define VNITEST_RUNTIME_ASSERT_EQ(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(==,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_NE(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(!=,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_GT(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(>,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_GE(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(>=,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_LT(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(<,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_LE(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(<=,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_AND(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(&&,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_OR(lhs__,...) \
    VNITEST_RUNTIME_ASSERT_BINARY_OPERATOR(||,lhs__,__VA_ARGS__) \
//
#define VNITEST_RUNTIME_ASSERT_NOT(...) \
    ::vnitest::RuntimeAssertStream( \
        VNITEST_MAKE_CONDITION_INFO((false) == (__VA_ARGS__)) \
    ) \
//
//=============================================================================
//#include<vnitest/static_string.hpp>
//=============================================================================
namespace vnitest{

template<typename CharType__,::std::size_t length__>
struct StaticCStr{
    CharType__ value[length__]{};
    consteval StaticCStr(CharType__ const(&c_str)[length__]){
        ::std::copy(
            ::std::begin(c_str),::std::end(c_str),::std::begin(value)
        );
    }
    using char_type=CharType__;
    static constexpr auto length=length__;
};

template<StaticCStr static_c_str__>
struct StaticString{
    using char_type=
        typename ::std::remove_cvref_t<decltype(static_c_str__)>::char_type;
    static constexpr auto size=static_c_str__.length-1;
    static constexpr decltype(auto) value=(static_c_str__.value);
};

}//namespace vnitest

//...: c_str_literal
#define VNITEST_MAKE_STATIC_STRING(...) \
    ::vnitest::StaticString<__VA_ARGS__> \
//
//=============================================================================
//#include<vnitest/ordered_map.hpp>
//=============================================================================
namespace vnitest{

template<typename KeyType__,typename ValueType__>
requires
    ::std::is_same_v<::std::decay_t<KeyType__>,KeyType__>&&
    ::std::is_same_v<::std::decay_t<ValueType__>,ValueType__>
class OrderedMap final{
    struct Pair{
        KeyType__ const key_;
        ValueType__ value_;
    };
    using pair_t=Pair;
    using list_iterator=typename ::std::list<pair_t>::iterator;
    ::std::unordered_map<KeyType__,list_iterator> map_;
    ::std::list<pair_t> list_;
public:
    constexpr OrderedMap(void)noexcept
        :map_({})
        ,list_({})
    {}
    constexpr OrderedMap(OrderedMap const& rhs)noexcept
        :OrderedMap(){
        *this=rhs;
    }
    constexpr OrderedMap(OrderedMap&& rhs)noexcept
        :OrderedMap(){
        *this=::std::move(rhs);
    }
    constexpr OrderedMap& operator=(OrderedMap const& rhs)noexcept{
        if(this==&rhs){
            return *this;
        }
        this->clear();
        rhs.foreach(
            [this](KeyType__ const& key,ValueType__ const& value){
                this->insert(key,value);
            }
        );
        return *this;
    }
    constexpr OrderedMap& operator=(OrderedMap&& rhs)noexcept{
        if(this==&rhs){
            return *this;
        }
        this->list_=::std::move(rhs.list_);
        this->map_=::std::move(rhs.map_);
        return *this;
    }
    constexpr OrderedMap& swap(OrderedMap& rhs)noexcept{
        this->list_.swap(rhs.list_);
        this->map_.swap(rhs.map_);
        return *this;
    }
    constexpr bool contains(KeyType__ const& key)const noexcept{
        return this->map_.count(key)!=0;
    }
    constexpr OrderedMap& insert(
        KeyType__ const& key,
        ValueType__ const& value
    ){
        if(this->contains(key)){
            throw ::std::runtime_error(
                "OrderedMap::insert(): "
                "This key already exists."
            );
        }
        this->map_[key]=this->list_.insert(
            this->list_.cend(),
            pair_t{key,value}
        );
        return *this;
    }
    constexpr OrderedMap& erase(KeyType__ const& key){
        if(!this->contains(key)){
            throw ::std::runtime_error(
                "OrderedMap::erase(): "
                "Can't find this key."
            );
        }
        this->list_.erase(this->map_[key]);
        this->map_.erase(key);
        return *this;
    }
    constexpr OrderedMap& clear(void)noexcept{
        this->list_.clear();
        this->map_.clear();
        return *this;
    }
    constexpr ValueType__ const& get(KeyType__ const& key)const{
        if(!this->contains(key)){
            throw ::std::runtime_error(
                "OrderedMap::get(): "
                "Can't find this key."
            );
        }
        return this->map_.at(key)->value_;
    }
    constexpr OrderedMap& update(
        KeyType__ const& key,
        ValueType__ const& value
    ){
        if(!this->contains(key)){
            throw ::std::runtime_error(
                "OrderedMap::update() :"
                "Can't find this key."
            );
        }
        this->map_[key]->value_=value;
        return *this;
    }
    constexpr OrderedMap& set(
        KeyType__ const& key,
        ValueType__ const& value
    )noexcept{
        if(!this->contains(key)){
            this->insert(key,value);
        }else{
            this->update(key,value);
        }
        return *this;
    }
    constexpr ::std::size_t size(void)const noexcept{
        return this->map_.size();
    }
    constexpr bool empty(void)const noexcept{
        return this->size()==0;
    }
    constexpr OrderedMap& foreach(
        ::std::function<void(KeyType__ const&,ValueType__&)>const& func
    ){
        for(auto& pair:this->list_){
            func(pair.key_,pair.value_);
        }
        return *this;
    }
    constexpr OrderedMap const& foreach(
        ::std::function<void(KeyType__ const&,ValueType__ const&)>const& func
    )const{
        for(auto const& pair:this->list_){
            func(pair.key_,pair.value_);
        }
        return *this;
    }
#ifndef _MSC_VER
    constexpr OrderedMap& foreach(
        ::std::function<void(KeyType__ const&,ValueType__&)noexcept>const&
            func
    )noexcept{
        for(auto& pair:this->list_){
            func(pair.key_,pair.value_);
        }
        return *this;
    }
    constexpr OrderedMap const& foreach(
        ::std::function<void(KeyType__ const&,ValueType__ const&)noexcept>
            const& func
    )const noexcept{
        for(auto const& pair:this->list_){
            func(pair.key_,pair.value_);
        }
        return *this;
    }
#endif
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/case_dict.h>
//=============================================================================
namespace vnitest{

class CaseDict{
public:
    using dict_type=::vnitest::OrderedMap<::std::string_view,void(*)(void)>;
    static dict_type& get(void)noexcept{
        static dict_type case_dict={};
        return case_dict;
    }
    static bool regist(
        ::std::string_view case_name
        ,void(*case_body)(void)
    )noexcept{
        if(::vnitest::CaseDict::get().contains(case_name)){
            return false;
        }
        ::vnitest::CaseDict::get().insert(case_name,case_body);
        return true;
    }
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/case.hpp>
//=============================================================================
namespace vnitest::detail{

template<typename StaticString__>
struct Case;

}//namespace vnitest::detail

//...: c_str_literal
#define VNITEST_CASE(...) \
template<> \
struct vnitest::detail::Case<VNITEST_MAKE_STATIC_STRING(__VA_ARGS__)>{ \
    static void body(void); \
    inline static bool regist_flag=::vnitest::CaseDict::regist( \
        ::std::string_view{__VA_ARGS__} \
        ,&::vnitest::detail::Case<VNITEST_MAKE_STATIC_STRING(__VA_ARGS__)> \
            ::body \
    ); \
}; \
void ::vnitest::detail::Case<VNITEST_MAKE_STATIC_STRING(__VA_ARGS__)> \
    ::body(void) \
//
//=============================================================================
//#include<vnitest/source_info.h>
//=============================================================================
namespace vnitest{

struct SourceInfo{
    ::std::string_view file,line;
    ::std::string info;
};
}//namespace vnitest

#define VNITEST_MAKE_SOURCE_INFO(...) \
    ::vnitest::SourceInfo{ \
        __FILE__ \
        ,VNITEST_TO_STRING(__LINE__) \
        ,::std::string{__VA_ARGS__} \
    } \
//
//=============================================================================
//#include<vnitest/skip_stream.hpp>
//=============================================================================
namespace vnitest{

class SkipStream final{
public:
    template<typename Type__>
    SkipStream& operator<<(Type__&& arg)noexcept{
        ::vnitest::OStringStream oss={};
        ::std::string str=(oss<<::std::forward<Type__>(arg)).str();
        if(this->message_.has_value()){
            this->message_.value()+=str;
        }else{
            this->message_=str;
        }
        return *this;
    }
    SkipStream(::vnitest::SourceInfo const& si)noexcept;
    ~SkipStream(void)noexcept(false);
private:
    ::vnitest::SourceInfo info_;
    ::std::optional<::std::string> message_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/skip.h>
//=============================================================================
//...: string variable
#define VNITEST_SKIP(...) \
    ::vnitest::SkipStream(VNITEST_MAKE_SOURCE_INFO(__VA_ARGS__)) \
//
//=============================================================================
//#include<vnitest/regex.h>
//=============================================================================
namespace vnitest{

struct RegEx{
    ::std::string_view pattern;
    ::std::regex object;
};

}//namespace vnitest

//...: c_str_literal
#define VNITEST_MAKE_REGEX(...) \
    ::vnitest::RegEx{ \
        ::std::string_view{__VA_ARGS__} \
        ,::std::regex{__VA_ARGS__} \
    }\
//
//=============================================================================
//#include<vnitest/group_dict.h>
//=============================================================================
namespace vnitest{

class GroupDict{
public:
    using value_type=
        ::std::vector<::std::variant<::std::string_view,::vnitest::RegEx>>*;
    using dict_type=::vnitest::OrderedMap<::std::string_view,value_type>;
    static dict_type& get(void)noexcept{
        static dict_type group_dict={};
        return group_dict;
    }
    static bool regist(
        ::std::string_view group_name
        ,value_type group_body
    )noexcept{
        if(::vnitest::GroupDict::get().contains(group_name)){
            return false;
        }
        ::vnitest::GroupDict::get().insert(group_name,group_body);
        return true;
    }
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/group.hpp>
//=============================================================================
namespace vnitest::detail{

template<typename StaticString__>
struct Group;

}//namespace vnitest::detail

#define VNITEST_GROUP(...) \
    template<> \
    struct vnitest::detail::Group<VNITEST_MAKE_STATIC_STRING(__VA_ARGS__)>{ \
        using self_type=::vnitest::detail::Group< \
            VNITEST_MAKE_STATIC_STRING(__VA_ARGS__) \
        >; \
        using body_type=::std::vector< \
            ::std::variant<::std::string_view,::vnitest::RegEx> \
        >; \
        inline static body_type body_={}; \
        static void init(body_type& body); \
        inline static bool regist_flag=[](void){ \
            self_type::init(self_type::body_); \
            return ::vnitest::GroupDict::regist( \
                ::std::string_view{__VA_ARGS__} \
                ,&self_type::body_ \
            ); \
        }(); \
    }; \
    void ::vnitest::detail::Group< \
        VNITEST_MAKE_STATIC_STRING(__VA_ARGS__) \
    >::init( \
        typename ::vnitest::detail::Group< \
            VNITEST_MAKE_STATIC_STRING(__VA_ARGS__) \
        >::body_type& body \
    ) \
//

#define VNITEST_GROUP_ELEMENT_CASE(...) do{ \
    body.emplace_back(::std::string_view{__VA_ARGS__}); \
}while(0) \
//

#define VNITEST_GROUP_ELEMENT_CASE_REGEX(...) do{ \
    body.emplace_back(VNITEST_MAKE_REGEX(__VA_ARGS__)); \
}while(0) \
//

//number__: unsigned integer literal
#define VNITEST_GROUP_ELEMENT_CASE_REPEAT(number__,...) do{ \
    for(::std::size_t count=0;count<number__;++count){ \
        VNITEST_GROUP_ELEMENT_CASE(__VA_ARGS__); \
    } \
}while(0) \
//

//number__: unsigned integer literal
#define VNITEST_GROUP_ELEMENT_CASE_REGEX_REPEAT(number__,...) do{ \
    for(::std::size_t count=0;count<number__;++count){ \
        VNITEST_GROUP_ELEMENT_CASE_REGEX(__VA_ARGS__); \
    } \
}while(0) \
//
//=============================================================================
//#include<vnitest/runtime_check_failed_error.h>
//=============================================================================
namespace vnitest{

struct RuntimeCheckFailedError{
    ::std::string_view file,line;
    ::std::string info;
    ::std::optional<::std::string> msg;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/runtime_assert_failed_exception.h>
//=============================================================================
namespace vnitest{

class RuntimeAssertFailedException
    :public ::std::exception{
public:
    RuntimeAssertFailedException(RuntimeAssertFailedException const&)noexcept
        =default;
    RuntimeAssertFailedException(RuntimeAssertFailedException &&)noexcept
        =default;
    RuntimeAssertFailedException&
        operator=(RuntimeAssertFailedException const&)noexcept=default;
    RuntimeAssertFailedException&
        operator=(RuntimeAssertFailedException &&)noexcept=default;
    virtual ~RuntimeAssertFailedException(void)noexcept=default;

    RuntimeAssertFailedException(
        ::std::string_view file
        ,::std::string_view line
        ,::std::string const& info
        ,::std::optional<::std::string> const& msg={}
    )noexcept;
    virtual char const* what()const noexcept override;
    ::std::string_view file(void)const noexcept;
    ::std::string_view line(void)const noexcept;
    ::std::string const& info(void)const noexcept;
    bool has_msg(void)const noexcept;
    ::std::string const& msg(void)const noexcept;
    void set_msg(::std::optional<::std::string> const& msg)noexcept;
private:
    ::std::string_view file_,line_;
    ::std::string info_;
    ::std::optional<::std::string> msg_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/skip_exception.h>
//=============================================================================
namespace vnitest{

class SkipException
    :public ::std::exception{
public:
    SkipException(SkipException const&)noexcept=default;
    SkipException(SkipException &&)noexcept=default;
    SkipException& operator=(SkipException const&)noexcept=default;
    SkipException& operator=(SkipException &&)noexcept=default;
    virtual ~SkipException(void)noexcept=default;

    SkipException(
        ::std::string_view file
        ,::std::string_view line
        ,::std::string const& info
        ,::std::optional<::std::string> const& msg={}
    )noexcept;
    virtual char const* what()const noexcept override;
    ::std::string_view file(void)const noexcept;
    ::std::string_view line(void)const noexcept;
    ::std::string const& info(void)const noexcept;
    bool has_msg(void)const noexcept;
    ::std::string const& msg(void)const noexcept;
    void set_msg(::std::optional<::std::string> const& msg)noexcept;
private:
    ::std::string_view file_,line_;
    ::std::string info_;
    ::std::optional<::std::string> msg_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/execute_case_info.h>
//=============================================================================
namespace vnitest{

class ExecuteCaseInfo{
public:
    ExecuteCaseInfo(ExecuteCaseInfo const&)=default;
    ExecuteCaseInfo(ExecuteCaseInfo &&)=default;
    ExecuteCaseInfo& operator=(ExecuteCaseInfo const&)=default;
    ExecuteCaseInfo& operator=(ExecuteCaseInfo &&)=default;
    ~ExecuteCaseInfo(void)=default;

    explicit ExecuteCaseInfo(::std::string_view case_name="")noexcept;
    //case name
    ExecuteCaseInfo& set_case_name(::std::string_view case_name)noexcept;
    ::std::string_view get_case_name(void)const noexcept;
    //group name
    bool has_group_name(void)const noexcept;
    ExecuteCaseInfo& set_group_name(::std::string_view group_name)noexcept;
    ::std::string_view get_group_name(void)const noexcept;
    //execute time info
    ExecuteCaseInfo& execute_start(void)noexcept;
    ExecuteCaseInfo& execute_stop(void)noexcept;
    double get_duration_ns(void)const noexcept;
    ::std::string get_duration_string(void)const noexcept;
    //runtime check
    ExecuteCaseInfo& runtime_check_total_increment(void)noexcept;
    ExecuteCaseInfo& runtime_check_passed_increment(void)noexcept;
    ExecuteCaseInfo& runtime_check_failed_increment(void)noexcept;
    ::std::size_t get_runtime_check_total(void)const noexcept;
    ::std::size_t get_runtime_check_passed(void)const noexcept;
    ::std::size_t get_runtime_check_failed(void)const noexcept;
    double get_runtime_check_passed_rate(void)const noexcept;
    //runtime check failed errors
    bool has_runtime_check_failed_errors(void)const noexcept;
    ExecuteCaseInfo& runtime_check_failed_errors_push_back(
        ::vnitest::RuntimeCheckFailedError const& error
    )noexcept;
    ExecuteCaseInfo const& runtime_check_failed_errors_foreach(
        ::std::function<void(::vnitest::RuntimeCheckFailedError const&)>const&
            func
    )const noexcept;
    ::std::vector<::vnitest::RuntimeCheckFailedError> const&
        get_runtime_check_errors(void)const noexcept;
    //runtime assert
    ExecuteCaseInfo& runtime_assert_total_increment(void)noexcept;
    ExecuteCaseInfo& runtime_assert_passed_increment(void)noexcept;
    ExecuteCaseInfo& runtime_assert_failed_increment(void)noexcept;
    ::std::size_t get_runtime_assert_total(void)const noexcept;
    ::std::size_t get_runtime_assert_passed(void)const noexcept;
    ::std::size_t get_runtime_assert_failed(void)const noexcept;
    double get_runtime_assert_passed_rate(void)const noexcept;
    //runtime assert failed exception
    ExecuteCaseInfo& set_runtime_assert_failed_exception(
        ::vnitest::RuntimeAssertFailedException const& exception
    )noexcept;
    bool has_runtime_assert_failed_exception(void)const noexcept;
    ::vnitest::RuntimeAssertFailedException const&
    get_runtime_assert_failed_exception(void)const noexcept;
    //runtime exception
    bool has_runtime_exception(void)const noexcept;
    ExecuteCaseInfo& set_runtime_exception(
        ::std::string const& exception_what
    )noexcept;
    ::std::string const& get_runtime_exception(void)const noexcept;
    //state
    enum class State{ Undefined,Passed,Failed,Skipped };
    ExecuteCaseInfo::State get_state(void)const noexcept;
    bool is_passed(void)const noexcept;
    bool is_failed(void)const noexcept;
    bool is_skipped(void)const noexcept;
    bool is_undefined(void)const noexcept;
    //skip
    ExecuteCaseInfo& set_skip(::vnitest::SkipException const& se)noexcept;
    bool has_skip(void)const noexcept;
    ::vnitest::SkipException const& get_skip(void)const noexcept;
    //current
    static void set_current(ExecuteCaseInfo& info)noexcept;
    static ExecuteCaseInfo& get_current(void)noexcept;
    static bool has_current(void)noexcept;
    static void reset_current(void)noexcept;
private:
    ::std::string_view case_name_;
    ::std::optional<::std::string_view> group_name_;
    ::vnitest::Timer timer_;
    ::std::size_t runtime_check_total_;
    ::std::size_t runtime_check_passed_;
    ::std::size_t runtime_check_failed_;
    ::std::vector<::vnitest::RuntimeCheckFailedError>
        runtime_check_failed_errors_;
    ::std::size_t runtime_assert_total_;
    ::std::size_t runtime_assert_passed_;
    ::std::size_t runtime_assert_failed_;
    ::std::optional<::vnitest::RuntimeAssertFailedException>
        runtime_assert_failed_exception_;
    ::std::optional<::std::string> runtime_exception_;
    ::std::optional<::vnitest::SkipException> skip_exception_;
    static ExecuteCaseInfo* current_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/execute_group_info.h>
//=============================================================================
namespace vnitest{

class ExecuteGroupInfo{
public:
    ExecuteGroupInfo(ExecuteGroupInfo const&)=default;
    ExecuteGroupInfo(ExecuteGroupInfo &&)=default;
    ExecuteGroupInfo& operator=(ExecuteGroupInfo const&)=default;
    ExecuteGroupInfo& operator=(ExecuteGroupInfo &&)=default;
    ~ExecuteGroupInfo(void)=default;

    explicit ExecuteGroupInfo(::std::string_view name="")noexcept;
    //group name
    ExecuteGroupInfo& set_name(::std::string_view name)noexcept;
    ::std::string_view get_name(void)const noexcept;
    //execute time info
    ExecuteGroupInfo& execute_start(void)noexcept;
    ExecuteGroupInfo& execute_stop(void)noexcept;
    double get_duration_ns(void)const noexcept;
    ::std::string get_duration_string(void)const noexcept;
    //case
    ExecuteGroupInfo& case_total_increment(void)noexcept;
    ExecuteGroupInfo& case_passed_increment(void)noexcept;
    ExecuteGroupInfo& case_failed_increment(void)noexcept;
    ExecuteGroupInfo& case_skipped_increment(void)noexcept;
    ExecuteGroupInfo& case_undefined_increment(void)noexcept;
    ::std::size_t get_case_total(void)const noexcept;
    ::std::size_t get_case_passed(void)const noexcept;
    ::std::size_t get_case_failed(void)const noexcept;
    ::std::size_t get_case_skipped(void)const noexcept;
    ::std::size_t get_case_undefined(void)const noexcept;
    double get_case_passed_rate(void)const noexcept;
    //state
    enum class State{ Undefined,Passed,Failed,Skipped };
    ExecuteGroupInfo::State get_state(void)const noexcept;
    bool is_passed(void)const noexcept;
    bool is_failed(void)const noexcept;
    bool is_skipped(void)const noexcept;
    bool is_undefined(void)const noexcept;
    //data
    using case_regex_info_type=::std::tuple<
        ::std::string_view
        ,::std::vector<::vnitest::ExecuteCaseInfo>
    >;
    using element_type=::std::variant<
        ::vnitest::ExecuteCaseInfo
        ,case_regex_info_type
    >;
    ExecuteGroupInfo& data_push_back(
        ::std::string_view case_name
        ,::vnitest::ExecuteCaseInfo const& case_info
    )noexcept;
    ExecuteGroupInfo& data_push_back(
        ::std::string_view case_regex_pattern
        ,::std::vector<::vnitest::ExecuteCaseInfo> const& case_regex_info
    )noexcept;
    ::std::vector<element_type>const& get_data(void)const noexcept;
private:
    ::std::string_view name_;
    ::vnitest::Timer timer_;
    ::std::size_t case_total_;
    ::std::size_t case_passed_;
    ::std::size_t case_failed_;
    ::std::size_t case_skipped_;
    ::std::size_t case_undefined_;
    ::std::vector<element_type> data_;
};

}//namespace vnitest
//=============================================================================
//#include<vnitest/execute.h>
//=============================================================================
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
//=============================================================================
//#include<vnitest/to_json.h>
//=============================================================================
namespace vnitest{

inline static constexpr ::std::size_t default_tab_width=4;

::std::string to_json(
    bool condition
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::std::string_view sv
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::std::string const& str
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::std::size_t size
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    double number
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::vnitest::RuntimeCheckFailedError const& error
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::std::vector<::vnitest::RuntimeCheckFailedError>const& errors
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::vnitest::RuntimeAssertFailedException const& exception
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::vnitest::SkipException const& exception
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::vnitest::ExecuteCaseInfo const& ei
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::std::vector<::vnitest::ExecuteCaseInfo> const& ei_vec
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::vnitest::ExecuteGroupInfo const& ei
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;
::std::string to_json(
    ::std::vector<::vnitest::ExecuteGroupInfo> const& ei_vec
    ,::std::size_t tab_width=::vnitest::default_tab_width
    ,::std::size_t current_tab_number=0
)noexcept;

}//namespace vnitest
//=============================================================================
//#include<vnitest/type_name_of.hpp>
//=============================================================================
namespace vnitest{

//get type name of <Type__>
//return constexpr ::std::string_view
template<typename Type__>
consteval auto type_name_of(void)noexcept{
#ifndef _MSC_VER
    ::std::string_view ret=::std::source_location::current().function_name();
    auto front_index=ret.find_first_of('=')+1;
    auto back_index=ret.find_last_of(']')-1;
#else
    ::std::string_view ret={__FUNCSIG__};
    auto front_index=ret.find_first_of('<')+1;
    auto back_index=ret.find_last_of('>')-1;
#endif
    front_index=ret.find_first_not_of(' ',front_index);
    back_index=ret.find_last_not_of(' ',back_index);
    return ret.substr(front_index,back_index-front_index+1);
}

}//namespace vnitest

#define VNITEST_TYPE_NAME_OF_VARIABLE(...) \
    ::vnitest::type_name_of<decltype(__VA_ARGS__)>() \
//
//=============================================================================
//#include<vnitest/static_check.hpp>
//=============================================================================
namespace vnitest::detail{

template<bool value__>
struct Bool{
    constexpr Bool(void)noexcept{}
    constexpr Bool(Bool const&)noexcept{}
};

template<typename File__,typename Line__,typename Info__>
struct StaticCheck{
    constexpr StaticCheck(void)noexcept{}
    constexpr StaticCheck(StaticCheck const&)noexcept{}
    [[deprecated("::vnitest::static_check(false)")]]
    constexpr bool operator()(::vnitest::detail::Bool<false>)const noexcept{
        return false;
    }
    constexpr bool operator()(::vnitest::detail::Bool<true>)const noexcept{
        return true;
    }
};

template<typename File__>
struct File{};

template<::std::size_t line__>
struct Line{};

template<typename File__>
struct Info{};

template<auto lhs__,auto rhs__>
struct ValueEqual{};

template<auto lhs__,auto rhs__>
struct ValueNotEqual{};

template<auto lhs__,auto rhs__>
struct ValueGreaterThan{};

template<auto lhs__,auto rhs__>
struct ValueGreaterEqual{};

template<auto lhs__,auto rhs__>
struct ValueLessThan{};

template<auto lhs__,auto rhs__>
struct ValueLessEqual{};

template<bool lhs__,bool rhs__>
struct ValueAnd{};

template<bool lhs__,bool rhs__>
struct ValueOr{};

template<typename LHS__,typename RHS__>
struct TypeEqual{};

template<typename LHS__,typename RHS__>
struct TypeNotEqual{};

}//namespace vnitest::detail

#define VNITEST_STATIC_CHECK(...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::Info<VNITEST_MAKE_STATIC_STRING( \
                "(true) == (" VNITEST_TO_STRING(__VA_ARGS__) ")" \
            )> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>(__VA_ARGS__)> \
            result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_NOT(...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::Info<VNITEST_MAKE_STATIC_STRING( \
                "(false) == (" VNITEST_TO_STRING(__VA_ARGS__) ")" \
            )> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<!(static_cast<bool>(__VA_ARGS__))> \
            result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_EQ(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr auto lhs=lhs__; \
        constexpr auto rhs=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueEqual<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) == (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_NE(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr auto lhs=lhs__; \
        constexpr auto rhs=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueNotEqual<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) != (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_GT(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr auto lhs=lhs__; \
        constexpr auto rhs=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueGreaterThan<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) > (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_GE(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr auto lhs=lhs__; \
        constexpr auto rhs=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueGreaterEqual<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) >= (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_LT(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr auto lhs=lhs__; \
        constexpr auto rhs=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueLessThan<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) < (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_LE(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr auto lhs=lhs__; \
        constexpr auto rhs=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueLessEqual<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) <= (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_AND(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr bool lhs=static_cast<bool>(lhs__); \
        constexpr bool rhs=static_cast<bool>(__VA_ARGS__); \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueAnd<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) && (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_VALUE_OR(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        constexpr bool lhs=static_cast<bool>(lhs__); \
        constexpr bool rhs=static_cast<bool>(__VA_ARGS__); \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::ValueOr<lhs,rhs> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            (lhs) || (rhs) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_TYPE_EQ(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        using lhs_t=lhs__; \
        using rhs_t=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::TypeEqual<lhs_t,rhs_t> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            ::std::is_same_v<lhs_t,rhs_t> \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//

#define VNITEST_STATIC_CHECK_TYPE_NE(lhs__,...) \
    static constexpr bool VNITEST_UNIQUE_VAR_NAME( \
        vnitest_static_check_expr_ \
    )=[](void)noexcept->bool{ \
        using lhs_t=lhs__; \
        using rhs_t=__VA_ARGS__; \
        constexpr ::vnitest::detail::StaticCheck< \
            ::vnitest::detail::File<VNITEST_MAKE_STATIC_STRING(__FILE__)> \
            ,::vnitest::detail::Line<__LINE__> \
            ,::vnitest::detail::TypeNotEqual<lhs_t,rhs_t> \
        > static_check={}; \
        constexpr ::vnitest::detail::Bool<static_cast<bool>( \
            !(::std::is_same_v<lhs_t,rhs_t>) \
        )> result={}; \
        return \
            static_check(result) \
        ; \
    }() \
//
//============================================================================
//#include<vnitest/timer.cpp>
//============================================================================
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
//=============================================================================
//#include<vnitest/runtime_check_stream.cpp>
//=============================================================================
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
//=============================================================================
//#include<vnitest/runtime_assert_stream.cpp>
//=============================================================================
namespace vnitest{

RuntimeAssertStream::RuntimeAssertStream(::vnitest::ConditionInfo const& ci)
noexcept
    :info_(ci)
    ,message_({}){
}
RuntimeAssertStream::~RuntimeAssertStream(void)noexcept(false){
    ::std::optional<::vnitest::RuntimeAssertFailedException> exception={};
    if(!this->info_.condition){
        exception=::vnitest::RuntimeAssertFailedException(
            this->info_.file
            ,this->info_.line
            ,this->info_.info
        );
    }
    if(::vnitest::ExecuteCaseInfo::has_current()){//in VNITEST_CASE
        ::vnitest::ExecuteCaseInfo::get_current()
            .runtime_assert_total_increment();
    }
    if(!exception.has_value()){
        if(::vnitest::ExecuteCaseInfo::has_current()){//in VNITEST_CASE
            ::vnitest::ExecuteCaseInfo::get_current()
                .runtime_assert_passed_increment();
        }
    }else{
        if(::vnitest::ExecuteCaseInfo::has_current()){//in VNITEST_CASE
            ::vnitest::ExecuteCaseInfo::get_current()
                .runtime_assert_failed_increment();
        }
        if(this->message_.has_value()){
            exception.value().set_msg(this->message_.value());
        }
        throw exception.value();
    }
}

}//namespace vnitest
//=============================================================================
//#include<vnitest/skip_stream.cpp>
//=============================================================================
namespace vnitest{

SkipStream::SkipStream(::vnitest::SourceInfo const& si)noexcept
    :info_(si),message_({})
{}
SkipStream::~SkipStream(void)noexcept(false){
    auto se=::vnitest::SkipException(
        this->info_.file
        ,this->info_.line
        ,this->info_.info
    );
    if(this->message_.has_value()){
        se.set_msg(this->message_.value());
    }
    throw se;
}

}//namespace vnitest
//=============================================================================
//#include<vnitest/runtime_assert_failed_exception.cpp>
//=============================================================================
namespace vnitest{

RuntimeAssertFailedException::RuntimeAssertFailedException(
    ::std::string_view file
    ,::std::string_view line
    ,::std::string const& info
    ,::std::optional<::std::string> const& msg
)noexcept
:file_(file),line_(line),info_(info),msg_(msg)
{}
char const* RuntimeAssertFailedException::what(void)const noexcept{
    static ::std::string ret;
    ret=::std::format(
        "[FILE] {} [LINE] {} [INFO] {}"
        ,this->file_
        ,this->line_
        ,this->info_
    );
    if(this->msg_.has_value()){
        ret+=::std::format(" [MSG] {}",this->msg_.value());
    }
    return ret.c_str();
}
::std::string_view RuntimeAssertFailedException::file(void)const noexcept{
    return this->file_;
}
::std::string_view RuntimeAssertFailedException::line(void)const noexcept{
    return this->line_;
}
::std::string const& RuntimeAssertFailedException::info(void)const noexcept{
    return this->info_;
}
bool RuntimeAssertFailedException::has_msg(void)const noexcept{
    return this->msg_.has_value();
}
::std::string const& RuntimeAssertFailedException::msg(void)const noexcept{
    return this->msg_.value();
}
void RuntimeAssertFailedException::set_msg(
    ::std::optional<::std::string> const& msg
)noexcept{
    this->msg_=msg;
}

}//namespace vnitest
//=============================================================================
//#include<vnitest/skip_exception.cpp>
//=============================================================================
namespace vnitest{

SkipException::SkipException(
    ::std::string_view file
    ,::std::string_view line
    ,::std::string const& info
    ,::std::optional<::std::string> const& msg
)noexcept
:file_(file),line_(line),info_(info),msg_(msg)
{}
char const* SkipException::what(void)const noexcept{
    static ::std::string ret;
    ret=::std::format(
        "[FILE] {} [LINE] {} [INFO] {}"
        ,this->file_
        ,this->line_
        ,this->info_
    );
    if(this->msg_.has_value()){
        ret+=::std::format(" [MSG] {}",this->msg_.value());
    }
    return ret.c_str();
}
::std::string_view SkipException::file(void)const noexcept{
    return this->file_;
}
::std::string_view SkipException::line(void)const noexcept{
    return this->line_;
}
::std::string const& SkipException::info(void)const noexcept{
    return this->info_;
}
bool SkipException::has_msg(void)const noexcept{
    return this->msg_.has_value();
}
::std::string const& SkipException::msg(void)const noexcept{
    return this->msg_.value();
}
void SkipException::set_msg(
    ::std::optional<::std::string> const& msg
)noexcept{
    this->msg_=msg;
}

}//namespace vnitest
//=============================================================================
//#include<vnitest/execute_case_info.cpp>
//=============================================================================
namespace vnitest{

ExecuteCaseInfo* ExecuteCaseInfo::current_=nullptr;

ExecuteCaseInfo::ExecuteCaseInfo(::std::string_view case_name)noexcept
    :case_name_(case_name)
    ,group_name_({})
    ,timer_({})
    ,runtime_check_total_(0)
    ,runtime_check_passed_(0)
    ,runtime_check_failed_(0)
    ,runtime_check_failed_errors_({})
    ,runtime_assert_total_(0)
    ,runtime_assert_passed_(0)
    ,runtime_assert_failed_(0)
    ,runtime_assert_failed_exception_({})
    ,runtime_exception_({})
    ,skip_exception_({})
{}
//case name
ExecuteCaseInfo&
ExecuteCaseInfo::set_case_name(::std::string_view case_name)noexcept{
    this->case_name_=case_name;
    return *this;
}
::std::string_view ExecuteCaseInfo::get_case_name(void)const noexcept{
    return this->case_name_;
}
//group name
bool ExecuteCaseInfo::has_group_name(void)const noexcept{
    return this->group_name_.has_value();
}
ExecuteCaseInfo&
ExecuteCaseInfo::set_group_name(::std::string_view group_name)noexcept{
    this->group_name_=group_name;
    return *this;
}
::std::string_view ExecuteCaseInfo::get_group_name(void)const noexcept{
    return this->group_name_.value();
}
//execute time info
ExecuteCaseInfo& ExecuteCaseInfo::execute_start(void)noexcept{
    this->timer_.start();
    return *this;
}
ExecuteCaseInfo& ExecuteCaseInfo::execute_stop(void)noexcept{
    this->timer_.stop();
    return *this;
}
double ExecuteCaseInfo::get_duration_ns(void)const noexcept{
    return this->timer_.duration_nanoseconds();
}
::std::string ExecuteCaseInfo::get_duration_string(void)const noexcept{
    return this->timer_.duration_string();
}
//runtime check
bool ExecuteCaseInfo::has_runtime_check_failed_errors(void)const noexcept{
    return !(this->runtime_check_failed_errors_.empty());
}
ExecuteCaseInfo& ExecuteCaseInfo::runtime_check_total_increment(void)noexcept{
    ++(this->runtime_check_total_);
    return *this;
}
ExecuteCaseInfo&
ExecuteCaseInfo::runtime_check_passed_increment(void)noexcept{
    ++(this->runtime_check_passed_);
    return *this;
}
ExecuteCaseInfo&
ExecuteCaseInfo::runtime_check_failed_increment(void)noexcept{
    ++(this->runtime_check_failed_);
    return *this;
}
::std::size_t ExecuteCaseInfo::get_runtime_check_total(void)const noexcept{
    return this->runtime_check_total_;
}
::std::size_t ExecuteCaseInfo::get_runtime_check_passed(void)const noexcept{
    return this->runtime_check_passed_;
}
::std::size_t ExecuteCaseInfo::get_runtime_check_failed(void)const noexcept{
    return this->runtime_check_failed_;
}
double ExecuteCaseInfo::get_runtime_check_passed_rate(void)const noexcept{
    return this->runtime_check_total_>0
        ?static_cast<double>(this->runtime_check_passed_)
            /this->runtime_check_total_
        :1.0;
}
//runtime check failed errors
ExecuteCaseInfo& ExecuteCaseInfo::runtime_check_failed_errors_push_back(
    ::vnitest::RuntimeCheckFailedError const& error
)noexcept{
    this->runtime_check_failed_errors_.emplace_back(error);
    return *this;
}
ExecuteCaseInfo const& ExecuteCaseInfo::runtime_check_failed_errors_foreach(
    ::std::function<void(::vnitest::RuntimeCheckFailedError const&)>const&
        func
)const noexcept{
    for(auto const& error:this->runtime_check_failed_errors_){
        func(error);
    }
    return *this;
}
::std::vector<::vnitest::RuntimeCheckFailedError> const&
    ExecuteCaseInfo::get_runtime_check_errors(void)const noexcept{
    return this->runtime_check_failed_errors_;
}
//runtime assert
ExecuteCaseInfo&
ExecuteCaseInfo::runtime_assert_total_increment(void)noexcept{
    ++(this->runtime_assert_total_);
    return *this;
}
ExecuteCaseInfo&
ExecuteCaseInfo::runtime_assert_passed_increment(void)noexcept{
    ++(this->runtime_assert_passed_);
    return *this;
}
ExecuteCaseInfo&
ExecuteCaseInfo::runtime_assert_failed_increment(void)noexcept{
    ++(this->runtime_assert_failed_);
    return *this;
}
::std::size_t ExecuteCaseInfo::get_runtime_assert_total(void)const noexcept{
    return this->runtime_assert_total_;
}
::std::size_t ExecuteCaseInfo::get_runtime_assert_passed(void)const noexcept{
    return this->runtime_assert_passed_;
}
::std::size_t ExecuteCaseInfo::get_runtime_assert_failed(void)const noexcept{
    return this->runtime_assert_failed_;
}
double ExecuteCaseInfo::get_runtime_assert_passed_rate(void)const noexcept{
    return this->runtime_assert_total_>0
        ?static_cast<double>(this->runtime_assert_passed_)
            /this->runtime_assert_total_
        :1.0;
}
//runtime assert failed exception
ExecuteCaseInfo& ExecuteCaseInfo::set_runtime_assert_failed_exception(
    ::vnitest::RuntimeAssertFailedException const& exception
)noexcept{
    this->runtime_assert_failed_exception_=exception;
    return *this;
}
bool ExecuteCaseInfo::has_runtime_assert_failed_exception(void)const noexcept{
    return this->runtime_assert_failed_exception_.has_value();
}
::vnitest::RuntimeAssertFailedException const&
ExecuteCaseInfo::get_runtime_assert_failed_exception(void)const noexcept{
    return this->runtime_assert_failed_exception_.value();
}
//runtime exception
bool ExecuteCaseInfo::has_runtime_exception(void)const noexcept{
    return this->runtime_exception_.has_value();
}
ExecuteCaseInfo& ExecuteCaseInfo::set_runtime_exception(
    ::std::string const& exception_what
)noexcept{
    this->runtime_exception_=exception_what;
    return *this;
}
::std::string const&
ExecuteCaseInfo::get_runtime_exception(void)const noexcept{
    return this->runtime_exception_.value();
}
//state
ExecuteCaseInfo::State ExecuteCaseInfo::get_state(void)const noexcept{
    if(this->is_undefined()){
        return ExecuteCaseInfo::State::Undefined;
    }
    if(this->is_skipped()){
        return ExecuteCaseInfo::State::Skipped;
    }
    if(this->is_failed()){
        return ExecuteCaseInfo::State::Failed;
    }
    return ExecuteCaseInfo::State::Passed;
}
bool ExecuteCaseInfo::is_passed(void)const noexcept{
    return (!this->is_undefined())
        &&(!this->is_failed())
        &&(!this->is_skipped());
}
bool ExecuteCaseInfo::is_failed(void)const noexcept{
    if(this->is_undefined()){
        return false;
    }
    if(this->is_skipped()){
        return false;
    }
    return this->has_runtime_check_failed_errors()
        || this->has_runtime_assert_failed_exception()
        || this->has_runtime_exception();
}
bool ExecuteCaseInfo::is_skipped(void)const noexcept{
    return (!this->is_undefined())&&(this->has_skip());
}
bool ExecuteCaseInfo::is_undefined(void)const noexcept{
    return !(::vnitest::CaseDict::get().contains(this->case_name_));
}
//skip
ExecuteCaseInfo&
ExecuteCaseInfo::set_skip(::vnitest::SkipException const& se)noexcept{
    this->skip_exception_=se;
    return *this;
}
bool ExecuteCaseInfo::has_skip(void)const noexcept{
    return this->skip_exception_.has_value();
}
::vnitest::SkipException const& ExecuteCaseInfo::get_skip(void)const noexcept{
    return this->skip_exception_.value();
}
//current
void ExecuteCaseInfo::set_current(ExecuteCaseInfo& info)noexcept{
    ExecuteCaseInfo::current_=&info;
}
ExecuteCaseInfo& ExecuteCaseInfo::get_current(void)noexcept{
    return *(ExecuteCaseInfo::current_);
}
bool ExecuteCaseInfo::has_current(void)noexcept{
    return ExecuteCaseInfo::current_!=nullptr;
}
void ExecuteCaseInfo::reset_current(void)noexcept{
    ExecuteCaseInfo::current_=nullptr;
}

}//namespace vnitest
//=============================================================================
//#include<vnitest/execute_group_info.cpp>
//=============================================================================
namespace vnitest{

ExecuteGroupInfo::ExecuteGroupInfo(::std::string_view name)noexcept
    :name_(name)
    ,timer_({})
    ,case_total_(0)
    ,case_passed_(0)
    ,case_failed_(0)
    ,case_skipped_(0)
    ,case_undefined_(0)
    ,data_({})
{}
//group name
ExecuteGroupInfo& ExecuteGroupInfo::set_name(::std::string_view name)noexcept{
    this->name_=name;
    return *this;
}
::std::string_view ExecuteGroupInfo::get_name(void)const noexcept{
    return this->name_;
}
//execute time info
ExecuteGroupInfo& ExecuteGroupInfo::execute_start(void)noexcept{
    this->timer_.start();
    return *this;
}
ExecuteGroupInfo& ExecuteGroupInfo::execute_stop(void)noexcept{
    this->timer_.stop();
    return *this;
}
double ExecuteGroupInfo::get_duration_ns(void)const noexcept{
    return this->timer_.duration_nanoseconds();
}
::std::string ExecuteGroupInfo::get_duration_string(void)const noexcept{
    return this->timer_.duration_string();
}
//case
ExecuteGroupInfo& ExecuteGroupInfo::case_total_increment(void)noexcept{
    ++(this->case_total_);
    return *this;
}
ExecuteGroupInfo& ExecuteGroupInfo::case_passed_increment(void)noexcept{
    ++(this->case_passed_);
    return *this;
}
ExecuteGroupInfo& ExecuteGroupInfo::case_failed_increment(void)noexcept{
    ++(this->case_failed_);
    return *this;
}
ExecuteGroupInfo& ExecuteGroupInfo::case_skipped_increment(void)noexcept{
    ++(this->case_skipped_);
    return *this;
}
ExecuteGroupInfo& ExecuteGroupInfo::case_undefined_increment(void)noexcept{
    ++(this->case_undefined_);
    return *this;
}
::std::size_t ExecuteGroupInfo::get_case_total(void)const noexcept{
    return this->case_total_;
}
::std::size_t ExecuteGroupInfo::get_case_passed(void)const noexcept{
    return this->case_passed_;
}
::std::size_t ExecuteGroupInfo::get_case_failed(void)const noexcept{
    return this->case_failed_;
}
::std::size_t ExecuteGroupInfo::get_case_skipped(void)const noexcept{
    return this->case_skipped_;
}
::std::size_t ExecuteGroupInfo::get_case_undefined(void)const noexcept{
    return this->case_undefined_;
}
double ExecuteGroupInfo::get_case_passed_rate(void)const noexcept{
    if(this->case_total_==0){
        return 1.0;
    }else{
        return static_cast<double>(this->case_passed_)/this->case_total_;
    }
}
//state
ExecuteGroupInfo::State ExecuteGroupInfo::get_state(void)const noexcept{
    if(this->is_passed()){
        return ExecuteGroupInfo::State::Passed;
    }else if(this->is_failed()){
        return ExecuteGroupInfo::State::Failed;
    }else if(this->is_skipped()){
        return ExecuteGroupInfo::State::Skipped;
    }
    return ExecuteGroupInfo::State::Undefined;
}
bool ExecuteGroupInfo::is_passed(void)const noexcept{
    return (!this->is_undefined())
        &&(this->case_total_==this->case_passed_);
}
bool ExecuteGroupInfo::is_failed(void)const noexcept{
    return (!this->is_undefined())
        &&(this->case_total_!=0)
        &&(this->case_failed_>0);
}
bool ExecuteGroupInfo::is_skipped(void)const noexcept{
    return (!this->is_undefined())
        &&(this->case_total_!=0)
        &&(this->case_skipped_>0);
}
bool ExecuteGroupInfo::is_undefined(void)const noexcept{
    return !(::vnitest::GroupDict::get().contains(this->name_));
}
ExecuteGroupInfo& ExecuteGroupInfo::data_push_back(
    ::std::string_view case_name
    ,::vnitest::ExecuteCaseInfo const& case_info
)noexcept{
    this->data_.emplace_back(case_info);
    return *this;
}
ExecuteGroupInfo& ExecuteGroupInfo::data_push_back(
    ::std::string_view case_regex_pattern
    ,::std::vector<::vnitest::ExecuteCaseInfo> const& case_regex_info
)noexcept{
    this->data_.emplace_back(
        typename ExecuteGroupInfo::case_regex_info_type{
            case_regex_pattern
            ,case_regex_info
        }
    );
    return *this;
}
::std::vector<typename ExecuteGroupInfo::element_type>const&
ExecuteGroupInfo::get_data(void)const noexcept{
    return this->data_;
}

}//namespace vnitest
//=============================================================================
//#include<vnitest/execute.cpp>
//=============================================================================
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
//=============================================================================
//#include<vnitest/to_json.cpp>
//=============================================================================
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

#endif//VNITEST_H
