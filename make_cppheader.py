
# Copyright 2017 Thomas Wendtland
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# ------------------------------------------------------------------------------

 # This file takes the 'OrderedDict' svdparser.py created and writes out C++
 # header files according a certain layout. Parsed dictionariers can of course
 # be used as one chooses to.

# ------------------------------------------------------------------------------

import svdparser
import sys

# ------------------------------------------------------------------------------

NAMESPACE = "donut"

FILE_HEADER = "\n#pragma once\n\n#include <cstdint>\n#include \"Bitfield.hpp\"\n\nusing namespace " + NAMESPACE + ";\n\nnamespace "
PERIPHERAL_TEMPLATE_STRING = "\ttemplate <std::uint32_t BaseAddress, std::uint16_t Irq, typename ... Pins>\n"
REGISTER_WIDTH_TYPE = "std::uint32_t"

# ------------------------------------------------------------------------------

def cpp_access_type_for_xmlvalue(xmlvalue):
    if xmlvalue == "read-only":
        return "AccessType::ReadOnly"
    elif xmlvalue == "write-only":
        return "AccessType::WriteOnly"
    elif xmlvalue == "read-write":
        return "AccessType::ReadWrite"

# ------------------------------------------------------------------------------

def type_for_bitwidth(bitwidth):
    width_type = "std::uint32_t"
    if bitwidth <= 16:
        width_type = "std::uint16_t"
    if bitwidth <= 8:
        width_type = "std::uint8_t"
    return width_type

# ------------------------------------------------------------------------------

def write_field(register, field, header_file):
    datatype = type_for_bitwidth(int(field["bitWidth"]))
    if register["name"] == field["name"]:
        field["name"] = "Value"

    access = register["access"]
    if "access" in field:
        access = field["access"]
    template_params = [register["name"].title(), datatype, field["bitOffset"], field["bitWidth"], cpp_access_type_for_xmlvalue(access)]
    header_file.write("\t\t\tusing " + field["name"].title() + " = Bitfield<" + (", ".join(template_params)) + ">;\n")

# ------------------------------------------------------------------------------

def write_register(register, header_file):
    header_file.write("\t\tstruct " + register["name"].title() + " {\n")
    header_file.write("\t\t\tusing WidthType = " + REGISTER_WIDTH_TYPE + ";\n")
    header_file.write("\t\t\tstatic constexpr WidthType Address = BaseAddress + " + register["addressOffset"] + ";\n")
    for field in register["fields"].items():
        write_field(register, field[1], header_file)
    header_file.write("\t\t};\n")

# ------------------------------------------------------------------------------

def write_peripheral(peripheral, header_file):
    header_file.write(PERIPHERAL_TEMPLATE_STRING)
    header_file.write("\tstruct " + svdparser.normalize(peripheral["name"]) + "Controller" +" {\n")
    for register in peripheral["registers"].items():
        write_register(register[1], header_file)
    header_file.write("\t};")

# ------------------------------------------------------------------------------

def write_instance(instance, header_file):
    interrupt = "0xFF"
    if "interrupt" in instance:
        interrupt = instance["interrupt"]["value"]
    header_file.write("\tusing " + instance["name"] + " = " + svdparser.normalize(instance["name"]) + "Controller<" + instance["baseAddress"] + ", " + interrupt + ">;\n")

# ------------------------------------------------------------------------------

def main():
    if len(sys.argv) != 3:
        print "Usage: makecpp_header.py SVDFILE PERIPHERAL"
        exit(-1)

    svd_filename = sys.argv[1]
    peripheral_name = sys.argv[2]
    device_as_dict = svdparser.run(svd_filename)
    if device_as_dict is None:
        exit(-1)
    device_name = device_as_dict.items()[0][0].lower()

    peripherals = device_as_dict.items()[0][1]
    if peripheral_name.title() not in peripherals:
        print "Error: no such peripheral in the SVD provided"
        exit(-1)

    header_file = open("header/" + device_name + "_" + peripheral_name.lower() + ".hpp", "w")
    header_file.write(FILE_HEADER + device_name + " {\n")

    write_peripheral(peripherals[peripheral_name.title()], header_file)
    header_file.write("\n")
    for instance in peripherals[peripheral_name.title()]["instances"].items():
        write_instance(instance[1], header_file)

    header_file.write("\n} // end of namespace " + device_name)
    header_file.close()

    print "File successfully written to " + header_file.name

# ------------------------------------------------------------------------------

if __name__ == "__main__":
    main()
