#pragma once

#include <format>
#include <cstdint>
#include <stdfloat>
#include <string_view>
#include <stdexcept>

template <typename T>
consteval std::string_view get_type_str()
{
    throw std::invalid_argument("Unsupported type");
}

template <>
consteval std::string_view get_type_str<std::int32_t>()
{
    return "<i4"; 
}

template <>
consteval std::string_view get_type_str<std::int64_t>()
{
    return "<i8"; 
}

template <>
consteval std::string_view get_type_str<std::uint32_t>()
{
    return "<u4"; 
}

template <>
consteval std::string_view get_type_str<std::uint64_t>()
{
    return "<u8"; 
}

// c++23 fixed width floating point
template<>
consteval std::string_view get_type_str<std::float32_t>()
{
    return "<f4";
}

template<>
consteval std::string_view get_type_str<std::float64_t>()
{
    return "<f8";
}

template <typename T>
inline std::string field_to_json(std::string_view name)
{
    return std::format("{\"name\": \"{}\", \"type\": \"{}\"}", name, get_type_str<T>());
}
