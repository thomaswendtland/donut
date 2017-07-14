
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

/*

    Some tests reads/writes to see if the code produced is as close to the
    'handcrafted' version

*/

using namespace donut;

struct TestRegister {
    using WidthType = std::uint32_t;
    static constexpr std::uint64_t Address = 0x20003000;
    using TestBitfield = donut::Bitfield<TestRegister, std::uint16_t, 12, 9, AccessType::ReadWrite>;
};

int main(int argc, char** argv){
    // do it the manual way
    //volatile auto test = (*((volatile TestRegister::WidthType*)TestRegister::Address) & 0x1FF000) >> 12;
    //
    //volatile auto reg_value = TestRegister::TestBitfield::read();

    // do it the manual way
    // *((volatile TestRegister::WidthType*)TestRegister::Address) &= ~0x1ff000;
    // *((volatile TestRegister::WidthType*)TestRegister::Address) |= (0x5<<12)&0x1ff000;
    //
    TestRegister::TestBitfield::write(0x5);
    TestRegister::TestBitfield::write(0x7);

    return 0;
}
