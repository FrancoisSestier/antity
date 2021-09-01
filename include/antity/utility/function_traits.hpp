#pragma once
#include <concepts>
#include <functional>

template <typename... Args>
struct type_list {};

namespace details {
    template <typename Class, typename Ret, typename... Args>
    type_list<Args...> args_helper(Ret (Class::*)(Args...));

    template <typename Class, typename Ret, typename... Args>
    type_list<Args...> args_helper(Ret (Class::*)(Args...) const);

    template <typename Class, typename Ret, typename... Args>
    Ret ret_helper(Ret (Class::*)(Args...));

    
    template <typename Class, typename Ret, typename... Args>
    Ret ret_helper(Ret (Class::*)(Args...) const);

}  // namespace details

template <typename F>
struct function_traits {};

template <typename F>
struct functor_traits {
    using args_type = decltype(details::args_helper(&F::operator()));
    using return_type = decltype(details::ret_helper(&F::operator())); 
};