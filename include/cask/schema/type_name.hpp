#pragma once

#include <cstdint>
#include <string>

namespace cask {

template<typename T> struct type_name;

template<> struct type_name<float> { static constexpr const char* value = "float32"; };
template<> struct type_name<double> { static constexpr const char* value = "float64"; };
template<> struct type_name<int32_t> { static constexpr const char* value = "int32"; };
template<> struct type_name<uint32_t> { static constexpr const char* value = "uint32"; };
template<> struct type_name<int64_t> { static constexpr const char* value = "int64"; };
template<> struct type_name<uint64_t> { static constexpr const char* value = "uint64"; };
template<> struct type_name<bool> { static constexpr const char* value = "bool"; };
template<> struct type_name<std::string> { static constexpr const char* value = "string"; };

}
