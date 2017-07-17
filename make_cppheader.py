
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
from collections import OrderedDict
import fileinput

# ------------------------------------------------------------------------------

NAMESPACE = "rye"

FILE_HEADER = "\n#pragma once\n\n#include <cstdint>\n#include \"Bitfield.hpp\"\n\nusing namespace " + NAMESPACE + ";\n\nnamespace "
PERIPHERAL_TEMPLATE_STRING = "\ttemplate <std::uint32_t BaseAddress, std::uint16_t Irq>\n"
REGISTER_WIDTH_TYPE = "std::uint32_t"

types = list()

# strip digits at the end of a name
# ------------------------------------------------------------------------------
def strip_trailing_digits(name):
    if name[0].isdigit():
        return name # return if all digits
    while name[len(name)-1].isdigit() == True:
        name = name[:-1]
    return name


#  different SVDs may contain different tags for elements, use one standard
# ------------------------------------------------------------------------------

def standardize(field):
    if "access" in field:
        if field["access"] == "read-only":
            field["access"] = "AccessType::ReadOnly"
        elif field["access"] == "write-only":
            field["access"] = "AccessType::WriteOnly"
        elif field["access"] == "read-write":
            field["access"] = "AccessType::ReadWrite"
    else:
        field["access"] = "AccessType::ReadWrite"
    if "lsb" in field:
        field["bitOffset"] = field["lsb"]
        field["bitWidth"]  = str(int(field["msb"]) - int(field["lsb"]) + 1)
    return field

# ------------------------------------------------------------------------------

def type_for_bitwidth(bitwidth):
    width_type = "std::uint32_t"
    if bitwidth <= 16:
        width_type = "std::uint16_t"
    if bitwidth <= 8:
        width_type = "std::uint8_t"
    if bitwidth == 1:
        width_type = "bool"
    return width_type

# ------------------------------------------------------------------------------

def write_field(register, field, header_file):
    field = standardize(field)
    datatype = ""
    # add a global type this field
    if "enumeratedValues" in field and len(field["enumeratedValues"]) > 1:
        global types
        datatype = field["name"].title() + "Type"
        typestring = "\tenum class " + datatype + "{\n"
        for value in  field["enumeratedValues"]:
            # todo: remove leading digits
            typestring += "\t\t" + value + ",\n"
        typestring = typestring[:-2] + "\n" # delete comma for last item
        typestring += "\t};\n"
        types.append(typestring)
    else:
        datatype = type_for_bitwidth(int(field["bitWidth"]))
    if register["name"] == field["name"]:
        field["name"] = "Value"

    template_params = [register["name"].title(), datatype, field["bitOffset"], field["bitWidth"], field["access"]]

    header_file.write("\t\t\tusing " + field["name"].title() + " = Bitfield<" + ", ".join(template_params) + ">;\n")

# ------------------------------------------------------------------------------

def write_register(register, header_file):
    header_file.write("\t\tstruct " + register["name"].title() + " {\n")
    header_file.write("\t\t\tusing WidthType = " + REGISTER_WIDTH_TYPE + ";\n")
    header_file.write("\t\t\tstatic constexpr WidthType Address = BaseAddress + " + register["addressOffset"].lower() + ";\n")
    if "fields" in register:
        for field in register["fields"].items():
            write_field(register, field[1], header_file)
    else: # write default field
        field = OrderedDict()
        field["name"] = "Value"
        field["bitOffset"] = "0"
        field["bitWidth"] = "32"
        field["access"] = "read-write"
        write_field(register, field, header_file)
    header_file.write("\t\t};\n")

# ------------------------------------------------------------------------------

def write_peripheral(peripheral, header_file):
    header_file.write(PERIPHERAL_TEMPLATE_STRING)
    header_file.write("\tstruct " + strip_trailing_digits(peripheral["name"].title()) + "Controller" +" {\n")
    for register in peripheral["registers"].items():
        write_register(register[1], header_file)
    header_file.write("\t};")

# ------------------------------------------------------------------------------

def write_instance(instance, header_file):
    interrupt = "0xFF"
    if "interrupt" in instance:
        interrupt = instance["interrupt"]["Value"]
    header_file.write("\n\tusing " + instance["name"].title() + " = " + strip_trailing_digits(instance["name"].title()) + "Controller<" + instance["baseAddress"] + ", " + interrupt + ">;")

# ------------------------------------------------------------------------------

def main():
    if len(sys.argv) != 3:
        print "Usage: makecpp_header.py SVDFILE PERIPHERAL"
        exit(-1)

    svd_filename = sys.argv[1]
    peripheral_name = sys.argv[2].title()
    device_as_dict = svdparser.run(svd_filename)
    if device_as_dict is None:
        exit(-1)

    device_name = device_as_dict.items()[0][0].lower()
    peripherals = device_as_dict.items()[0][1]

    if peripheral_name not in peripherals:
        print "Error: no such peripheral in the SVD provided"
        exit(-1)

    header_file = open("header/" + device_name + "_" + strip_trailing_digits(peripheral_name.lower()) + ".hpp", "w")
    header_file.write(FILE_HEADER + device_name + " {\n\n   // Types\n\n")

    peripheral = peripherals[peripheral_name]

    # if the peripheral selected inherits, take the parent to write header
    if "derivedFrom" in peripheral:
        peripheral_name = peripheral["derivedFrom"].title()
    write_peripheral(peripherals[peripheral_name], header_file)
    header_file.write("\n")
    # write the parent as an instance
    write_instance(peripherals[peripheral_name], header_file)

    # write all peripherals that inherit as instances
    for item in peripherals.items():
        if "derivedFrom" in item[1] and item[1]["derivedFrom"].title() == peripheral_name:
            write_instance(item[1], header_file)

    header_file.write("\n\n} // end of namespace " + device_name)

    header_file.close()

    for line in fileinput.input(header_file.name, inplace=True):
        if "Types" in line:
            line = "\n".join(types)
        print line,

    print "File successfully written to " + header_file.name

# ------------------------------------------------------------------------------

if __name__ == "__main__":
    main()
