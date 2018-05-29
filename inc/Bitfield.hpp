
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

    // @note: weak symbol to overwrite possible?
    void atomic_write();


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
    };
}

// read
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr volatile DataType rye::Bitfield<Register, DataType, Offset, Width, Access>::read(){
    static_assert(Access != Access::WriteOnly, "Read not allowed on write-only fields.");
    volatile RegType& reg_value = *(reinterpret_cast<volatile RegType*>(Register::Address));
    return static_cast<volatile DataType>((reg_value & mask())>>Offset);
}

// read with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr DataType rye::Bitfield<Register, DataType, Offset, Width, Access>::read(const std::uint32_t reg_number){
    static_assert(Access != Access::WriteOnly, "Read not allowed on write-only fields.");
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    volatile RegType& reg_value = *(reinterpret_cast<RegType*>(Register::Address + (reg_number*Register::Width)));
    return static_cast<volatile DataType>(((reg_value & mask())>>Offset));
}

// write
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
template<typename T>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::write(const T value){
    static_assert(Access == Access::WriteOnly || Access == Access::ReadWrite, "Write not allowed on read-only fields.");
    static_assert(std::is_same<T, DataType>::value, "Wrong dataype for field");
    using RegType = typename Register::WidthType;
    volatile RegType* reg = (reinterpret_cast<volatile RegType*>(Register::Address));
    auto reg_value = *reg;
    constexpr auto bitmask = mask();
    const std::uint32_t valueAsUint = static_cast<std::uint32_t>(value);
    if (Access == Access::ReadWrite){
        reg_value &= ~bitmask;
        reg_value |= (valueAsUint<<Offset) & bitmask;
    }
    else {
        reg_value = (valueAsUint<<Offset) & bitmask;
    }
    *reg = reg_value;
}

// write with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
template<typename T>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::write(const T value, const std::uint32_t reg_number){
    static_assert(Access == Access::WriteOnly || Access == Access::ReadWrite, "Write not allowed on read-only fields.");
    static_assert(std::is_same<T, DataType>::value, "Wrong dataype for field");
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    volatile RegType* reg = (reinterpret_cast<volatile RegType*>(Register::Address + (reg_number*Register::Width)));
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

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::Access Access>
constexpr typename Register::WidthType rye::Bitfield<Register, DataType, Offset, Width, Access>::mask(){
    std::uint64_t mask = std::uint64_t{0x000000000} | ((std::uint64_t{1}<<Width)-1); // std::uint64_t to surpress warning when shifting 32 bits
    return mask<<Offset;
}
