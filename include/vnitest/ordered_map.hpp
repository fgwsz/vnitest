#ifndef VNITEST_ORDERED_MAP_HPP
#define VNITEST_ORDERED_MAP_HPP

#include<cstddef>//::std::size_t

#include<type_traits>//::std::is_same_v ::std::decay_t
#include<list>//::std::list
#include<unordered_map>//::std::unordered_map
#include<utility>//::std::move
#include<stdexcept>//::std::runtime_error
#include<functional>//::std::function

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

#endif//VNITEST_ORDERED_MAP_HPP
