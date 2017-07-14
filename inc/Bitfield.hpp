
// Copyright 2017 Thomas Wendtland
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

    enum class AccessType {
        ReadOnly,
        WriteOnly,
        ReadWrite,
        ReadAndClear
    };

    // @note: weak symbol to overwrite possible?
    void atomic_write();


    template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, AccessType Access>
    struct Bitfield {
        using RegType = typename Register::WidthType;
        static_assert(!std::is_same<DataType, void>::value, "DataType must not be 'void'");
        static_assert((Offset+Width) < std::numeric_limits<typename Register::WidthType>::digits, "Invalid offset/width for Bitfield.");

        static constexpr volatile DataType read();
        static constexpr DataType read(const std::uint32_t reg_number);

        static constexpr void write(const DataType value);
        static constexpr void write(const DataType value, const std::uint32_t reg_number);
        static constexpr void set();
        static constexpr void clear();

        //private:
        static constexpr RegType mask();
    };
}

// read
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::AccessType Access>
constexpr volatile DataType rye::Bitfield<Register, DataType, Offset, Width, Access>::read(){
    static_assert(Access != AccessType::WriteOnly, "Read not allowed on write-only fields.");
    volatile RegType& reg_value = *(reinterpret_cast<volatile RegType*>(Register::Address));
    return ((reg_value & mask())>>Offset);
}

// read with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::AccessType Access>
constexpr DataType rye::Bitfield<Register, DataType, Offset, Width, Access>::read(const std::uint32_t reg_number){
    static_assert(Access != AccessType::WriteOnly, "Read not allowed on write-only fields.");
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    volatile RegType& reg_value = *(reinterpret_cast<RegType*>(Register::Address + (reg_number*reg_size)));
    return ((reg_value & mask())>>Offset);
}

// write
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::AccessType Access>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::write(const DataType value){
    static_assert(Access == AccessType::WriteOnly || Access == AccessType::ReadWrite, "Write not allowed on read-only fields.");
    using RegType = typename Register::WidthType;
    volatile RegType* reg = (reinterpret_cast<volatile RegType*>(Register::Address));
    auto reg_value = *reg;
    constexpr auto bitmask = mask();
    if (Access == AccessType::ReadWrite){
        reg_value &= ~bitmask;
        reg_value |= (value<<Offset) & bitmask;
    }
    else {
        reg_value = (value<<Offset) & bitmask;
    }
    *reg = reg_value;
}

// write with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::AccessType Access>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::write(const DataType value, const std::uint32_t reg_number){
    static_assert(Access == AccessType::WriteOnly || Access == AccessType::ReadWrite, "Write not allowed on read-only fields.");
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    volatile RegType& reg = *(reinterpret_cast<volatile RegType*>(Register::Address + (reg_number*reg_size)));
    auto reg_value = *reg;
    constexpr auto bitmask = mask();
    if (Access == AccessType::ReadWrite){
        reg_value &= ~bitmask;
        reg_value |= (value<<Offset) & bitmask;
    }
    else {
        reg_value = (value<<Offset) & bitmask;
    }
    *reg = value;
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::AccessType Access>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::set(){
    static_assert(Width == 1, "Set only allowed on fields of size 1");
    write(true);
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::AccessType Access>
constexpr void rye::Bitfield<Register, DataType, Offset, Width, Access>::clear(){
    static_assert(Width == 1, "Clear only allowed on fields of size 1");
    write(false);
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, rye::AccessType Access>
constexpr typename Register::WidthType rye::Bitfield<Register, DataType, Offset, Width, Access>::mask(){
    RegType mask = 0x000000000 | ((1<<Width)-1);
    return mask<<Offset;
}
