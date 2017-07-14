
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

using namespace rye;

struct TestRegister {
    using WidthType = std::uint32_t;
    using TestBitfield0 = rye::Bitfield<TestRegister, uint8_t, 4, 4, AccessType::ReadOnly>;
    using TestBitfield1 = rye::Bitfield<TestRegister, uint8_t, 18, 3, AccessType::ReadOnly>;
    using TestBitfield2 = rye::Bitfield<TestRegister, uint8_t, 7, 5, AccessType::ReadOnly>;
    using TestBitfield3 = rye::Bitfield<TestRegister, uint8_t, 12, 9, AccessType::ReadOnly>;
    using TestBitfield4 = rye::Bitfield<TestRegister, uint8_t, 0, 32, AccessType::ReadOnly>;
};

namespace {
    // order of values here: offset, width, expected mask
    constexpr std::uint32_t TestValues[] = { 0xF0, 0x1C0000, 0xF80, 0x1FF000, 0xFFFFFFFF};
}

int main(int argc, char** argv){

    std::uint32_t mask = 0;
    mask = TestRegister::TestBitfield0::mask();
    printf("mask: 0x%x - expected: 0x%x\n", mask, TestValues[0]);
    assert(mask == TestValues[0]);

    mask = TestRegister::TestBitfield1::mask();
    printf("mask: 0x%x - expected: 0x%x\n", mask, TestValues[1]);
    assert(mask == TestValues[1]);

    mask = TestRegister::TestBitfield2::mask();
    printf("mask: 0x%x - expected: 0x%x\n", mask, TestValues[2]);
    assert(mask == TestValues[2]);

    mask = TestRegister::TestBitfield3::mask();
    printf("mask: 0x%x - expected: 0x%x\n", mask, TestValues[3]);
    assert(mask == TestValues[3]);

    mask = TestRegister::TestBitfield4::mask();
    printf("mask: 0x%x - expected: 0x%x\n", mask, TestValues[4]);
    assert(mask == TestValues[4]);
    return 0;
}
