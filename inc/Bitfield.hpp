
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
#include <iostream>

/*

    TODO: - add offset check for register offset ()

*/

namespace donut {

    enum class AccessType {
        Read,
        Write,
        ReadWrite,
        ReadAndClear
    };

    template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, AccessType Access>
    struct Bitfield {
        static_assert(!std::is_same<DataType, void>::value, "DataType must not be 'void'");
        static_assert((Offset+Width) < std::numeric_limits<typename Register::WidthType>::digits, "Invalid offset/width for Bitfield.");
        static_assert(Width < (uint32_t)std::numeric_limits<DataType>::digits, "DataType too small for width.");

        static constexpr DataType read();
        static constexpr DataType read(const std::uint32_t offset);
        static constexpr void write(const DataType value);
        static constexpr void write(const DataType value, const std::uint32_t offset);

        //private:
        static constexpr typename Register::WidthType mask();

    };
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr DataType donut::Bitfield<Register, DataType, Offset, Width, Access>::read(){
    static_assert(Access != AccessType::Write, "Read not allowed on write-only fields.");
    using RegType = typename Register::WidthType;
    volatile RegType& reg_value = *(reinterpret_cast<RegType*>(Register::Address));
    return ((reg_value & mask())>>Offset);
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr DataType donut::Bitfield<Register, DataType, Offset, Width, Access>::read(const std::uint32_t reg_offset){
    static_assert(Access != AccessType::Write, "Read not allowed on write-only fields.");
    using RegType = typename Register::WidthType;
    constexpr std::size_t reg_size = sizeof(RegType);
    volatile RegType& reg_value = *(reinterpret_cast<RegType*>(Register::Address + (reg_offset*reg_size)));
    return ((reg_value & mask())>>Offset);
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr typename Register::WidthType donut::Bitfield<Register, DataType, Offset, Width, Access>::mask(){
    auto mask = 0x000000000 | ((1<<Width)-1);
    return mask<<Offset;
}
