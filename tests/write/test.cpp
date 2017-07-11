
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

#include "Bitfield.hpp"
#include <iostream>
#include <cassert>

/*

    To be compiled for and run on the host system to check if writes are
    returning the correct result.

*/

using namespace donut;

static std::uint32_t TestMemoryLocation[2] = {0};

struct TestRegister {
    using WidthType = std::uint32_t;
    static std::uint64_t Address; // not as in real life, test workaround
    using TestBitfield = donut::Bitfield<TestRegister, std::uint16_t, 12, 9, AccessType::ReadWrite>;
};

uint64_t TestRegister::Address = (std::uint64_t)&TestMemoryLocation;

namespace {
    // order of values here: value in memory, value exptected
    constexpr std::uint32_t TestValues[] = {0x34, 0x1};
}

int main(int argc, char** argv){

    for(std::uint32_t i = 0;i<sizeof(TestValues)/sizeof(std::uint32_t);i++){
        TestRegister::TestBitfield::write(TestValues[i]);
        auto read_value = TestRegister::TestBitfield::read();
        printf("write_value: %i - read_value : %i\n", TestValues[i], read_value);
        assert(read_value == TestValues[i]);
    }

    return 0;
}
