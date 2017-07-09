
// Copyright 2017 Thomas Wendtland
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
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

        static constexpr DataType read();
        static constexpr DataType read(const std::uint32_t offset);
        static constexpr void write(const DataType value);
        static constexpr void write(const DataType value, const std::uint32_t offset);

        //private:
        static constexpr auto mask(const std::uint32_t offset, const std::uint32_t width);

    };
}

// -----------------------------------------------------------------------------

template<typename Register, typename DataType, std::uint32_t Offset, std::uint32_t Width, donut::AccessType Access>
constexpr auto donut::Bitfield<Register, DataType, Offset, Width, Access>::mask(const std::uint32_t offset, const std::uint32_t width){
    auto mask  = 0x000000000 | ((1<<width)-1);
    return mask<<offset;
}
