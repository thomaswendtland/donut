
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

namespace donut {

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
        static_assert(Width < (uint32_t)std::numeric_limits<DataType>::digits, "DataType too small for width.");

        static constexpr volatile DataType read();
        static constexpr DataType read(const std::uint32_t reg_number);
        static constexpr void write(const DataType value);
        static constexpr void write(const DataType value, const std::uint32_t reg_number);

        //private:
        static constexpr RegType mask();

    };
}

// read
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr volatile DataType donut::Bitfield<Register, DataType, Offset, Width, Access>::read(){
    static_assert(Access != AccessType::WriteOnly, "Read not allowed on write-only fields.");
    volatile RegType& reg_value = *(reinterpret_cast<volatile RegType*>(Register::Address));
    return ((reg_value & mask())>>Offset);
}

// read with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr DataType donut::Bitfield<Register, DataType, Offset, Width, Access>::read(const std::uint32_t reg_number){
    static_assert(Access != AccessType::WriteOnly, "Read not allowed on write-only fields.");
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    volatile RegType& reg_value = *(reinterpret_cast<RegType*>(Register::Address + (reg_number*reg_size)));
    return ((reg_value & mask())>>Offset);
}

// write
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr void donut::Bitfield<Register, DataType, Offset, Width, Access>::write(const DataType value){
    static_assert(Access == AccessType::WriteOnly || Access == AccessType::ReadWrite, "Write not allowed on read-only fields.");
    using RegType = typename Register::WidthType;
    volatile RegType& reg_value = *(reinterpret_cast<volatile RegType*>(Register::Address));
    constexpr auto bitmask = mask();
    if (Access == AccessType::ReadWrite){
        reg_value &= ~bitmask;
        reg_value |= (value<<Offset) & bitmask;
    }
    else {
        reg_value = (value<<Offset) & bitmask;
    }
}

// write with offset
// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr void donut::Bitfield<Register, DataType, Offset, Width, Access>::write(const DataType value, const std::uint32_t reg_number){
    static_assert(Access == AccessType::WriteOnly || Access == AccessType::ReadWrite, "Write not allowed on read-only fields.");
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    volatile RegType& reg_value = *(reinterpret_cast<volatile RegType*>(Register::Address + (reg_number*reg_size)));
    constexpr auto bitmask = mask();
    if (Access == AccessType::ReadWrite){
        reg_value &= ~bitmask;
        reg_value |= (value<<Offset) & bitmask;
    }
    else {
        reg_value = (value<<Offset) & bitmask;
    }
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr typename Register::WidthType donut::Bitfield<Register, DataType, Offset, Width, Access>::mask(){
    RegType mask = 0x000000000 | ((1<<Width)-1);
    return mask<<Offset;
}
