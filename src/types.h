#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <memory>

// Type aliases similar to cppfront

namespace st {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using null = std::nullptr_t;

using std::shared_ptr;
using std::string;
using std::string_view;
using std::unique_ptr;

}  // namespace st