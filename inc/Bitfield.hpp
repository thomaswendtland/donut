
// Copyright 2018 Thomas Wendtland
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with \the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

/*

    TODO: - add offset check for register offset ()
          - atomimicity policies (bitbanding etc.)

*/

namespace rye {

    enum class Access {
        ReadOnly,
        WriteOnly,
        ReadWrite,
        ReadAndClear
    };

    template<typename Register, typename Dt, std::uint32_t Offset, std::uint32_t Width, Access Access>
    struct Bitfield {
        using RegType = typename Register::WidthType;
        using DataType = Dt;
        static_assert(!std::is_same<DataType, void>::value, "Bitfield error: DataType must not be 'void'");
        static_assert(Width <= std::numeric_limits<Dt>::digits, "Width for field exceeds type's number of bits");
        static_assert((Offset+Width) <= std::numeric_limits<typename Register::WidthType>::digits, "Bitfield error: Invalid offset/width for Bitfield.");

        static constexpr volatile DataType read();
        static constexpr DataType read(const std::uint32_t reg_number);

        template<typename T>
        static constexpr void write(const T value);
        template<typename T>
        static constexpr void write(const T value, const std::uint32_t reg_number);
        static constexpr void set();
        static constexpr void clear();

        static constexpr RegType mask();

    private:
        template<typename T>
        static constexpr DataType read_impl(const T addr);

        template<typename T, typename U>
        static constexpr void write_impl(const T value, const U addr);
    };
}

// read
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr volatile DataType rye::Bitfield<Register, DataType, Offset, Width, Access>::read(){
    return read_impl(Register::Address);
}

// read with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr DataType rye::Bitfield<Register, DataType, Offset, Width, Access>::read(const std::uint32_t reg_number){
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    return read_impl(Register::Address + (reg_number*reg_size));
}

// write
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
template<typename T>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::write(const T value){
    write_impl(value, Register::Address);
}


// write with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
template<typename T>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::write(const T value, const std::uint32_t reg_number){
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    write_impl(value, Register::Address + (reg_number*reg_size));
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::set(){
    static_assert(Width == 1, "Set only allowed on fields of size 1");
    write(true);
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::clear(){
    static_assert(Width == 1, "Clear only allowed on fields of size 1");
    write(false);
}

// read implementation
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
template<typename T>
constexpr DataType rye::Bitfield<Register, DataType, Offset, Width, Access>::read_impl(const T addr){
    static_assert(Access != Access::WriteOnly, "Read not allowed on write-only fields.");
    using RegType = typename Register::WidthType;
    volatile RegType& reg_value = *(reinterpret_cast<RegType*>(addr));
    return static_cast<volatile DataType>(((reg_value & mask())>>Offset));
}

// write implementation
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
template<typename T, typename U>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::write_impl(const T value, const U addr){
    static_assert(Access == Access::WriteOnly || Access == Access::ReadWrite, "Write not allowed on read-only fields.");
    static_assert(std::is_same<T, DataType>::value, "Wrong dataype for field");
    using RegType = typename Register::WidthType;
    volatile RegType* reg = reinterpret_cast<volatile RegType*>(addr);
    auto reg_value = *reg;
    constexpr auto bitmask = mask();
    if (Access == Access::ReadWrite){
        reg_value &= ~bitmask;
        reg_value |= (value<<Offset) & bitmask;
    }
    else {
        reg_value = (value<<Offset) & bitmask;
    }
    *reg = reg_value;
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr typename Register::WidthType rye::Bitfield<Register, DataType, Offset, Width, Access>::mask(){
    std::uint64_t mask = std::uint64_t{0x000000000} | ((std::uint64_t{1}<<Width)-1); // std::uint64_t to surpress warning when shifting 32 bits
    return mask<<Offset;
}
