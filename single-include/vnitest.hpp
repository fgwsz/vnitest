//=============================================================================
//#include<std>
//=============================================================================
//C std
#include<cstddef>//::std::size_t
//C++ 11
#include<ios>//std::boolalpha
#include<utility>//::std::forward ::std::move
#include<iterator>//::std::begin ::std::end
#include<algorithm>//::std::copy
#include<type_traits>//::std::remove_cvref_t
                     //::std::is_same_v ::std::decay_t
#include<initializer_list>//::std::initializer_list
#include<string>//::std::string
#include<list>//::std::list
#include<vector>//::std::vector
#include<unordered_map>//::std::unordered_map
#include<tuple>//::std::tuple
#include<functional>//::std::function
#include<exception>//::std::exception
#include<stdexcept>//::std::runtime_error
#include<sstream>//::std::ostringstream
#include<regex>//::std::regex
//C++ 17
#include<optional>//::std::optional
#include<variant>//::std::variant
#include<string_view>//::std::string_view
//maybe C++20
#include<chrono>//::std::chrono
//C++ 20
#include<source_location>//::std::source_location
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
        rhs->foreach(
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
