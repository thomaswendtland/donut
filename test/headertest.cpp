
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

//#include "stm32f072x_crc.hpp"
#include "Bitfield.hpp"
#include <iostream>
#include <cassert>

using namespace donut;

struct TestRegister {
    using WidthType = std::uint32_t;
    using TestBitfield = donut::Bitfield<TestRegister, uint8_t, 4, 4, AccessType::Read>;
};

namespace {
    constexpr auto ExpectedMask = 0xF0; // 4 bit maks shifted to the left by 4
    constexpr std::uint32_t TestValues[6] = { 4, 4, 0xF0, 18, 3, 0x1C0000};
}

// for now simply including a header to see if the syntax is correct
int main(int argc, char** argv){

    std::uint32_t mask = 0;
    for (auto i = 0;i<sizeof(TestValues);i+=3){
        mask = TestRegister::TestBitfield::mask(TestValues[i], TestValues[i+1]);
        printf("mask: 0x%x - expected: 0x%x\n", mask, TestValues[i+2]);
        assert(mask == TestValues[i+2]);
    }
    printf("0x%x\n", mask);
    return 0;
}
