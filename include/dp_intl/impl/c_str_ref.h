#pragma once

#include <cstddef>


namespace dp::intl {


class CStrRef {
public:
    CStrRef() = default;

    CStrRef(const char* data, std::size_t size)
        : data{data}
        , size{size}
    {
    }

    bool empty() const
    {
        return size == 0;
    }

    operator std::string_view() const
    {
        return {data, size};
    }

    operator const char*() const
    {
        return data;
    }
private:
    const char* data{""};
    std::size_t size{};
};


}
