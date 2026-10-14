#pragma once

#include <string>
#include <string_view>
#include <type_traits>


namespace test {


template<typename NumT>
auto toStr(NumT v) -> decltype(std::to_string(v))
{
    return std::to_string(v);
}


inline std::string toStr(std::string_view str)
{
    return '"' + std::string{str} + '"';
}


template<typename Iter>
std::string toStr(Iter begin, Iter end)
{
    std::string result{"{"};

    for (auto iter = begin; iter != end; ++iter) {
        if (iter != begin)
            result += ", ";

        result += toStr(*iter);
    }

    result += '}';

    return result;
}


template<
    typename T,
    std::enable_if_t<
        !std::is_convertible_v<T, std::string_view>, int> = 0>
auto toStr(const T& v) -> decltype(toStr(v.begin(), v.end()))
{
    return toStr(v.begin(), v.end());
}


}
