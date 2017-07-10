
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

    To be compiled for the host system to check if the bitmasks computed are
    matching the expected results.

*/

using namespace donut;

struct TestRegister {
    using WidthType = std::uint32_t;
    using TestBitfield = donut::Bitfield<TestRegister, uint8_t, 4, 4, AccessType::Read>;
};

namespace {
    // order of values here: offset, width, expected mask
    constexpr std::uint32_t TestValues[] = { 4, 4, 0xF0, 18, 3, 0x1C0000, 7, 5, 0xF80, 12, 9, 0x1FF000};
}

int main(int argc, char** argv){

    std::uint32_t mask = 0;
    for (std::uint32_t i = 0;i<sizeof(TestValues)/sizeof(uint32_t);i+=3){
        mask = TestRegister::TestBitfield::mask();
        printf("mask: 0x%x - expected: 0x%x\n", mask, TestValues[i+2]);
        assert(mask == TestValues[i+2]);
    }
    return 0;
}
