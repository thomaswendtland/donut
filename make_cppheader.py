
# Copyright 2018 Thomas Wendtland
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

#!/usr/bin/python

import svdparser
import sys
import os
from collections import OrderedDict
import fileinput

# ------------------------------------------------------------------------------

NAMESPACE = "rye"
INNER_NAMESPACE = "hardware"

TAB = "    "

FILE_HEADER = "\n#pragma once\n\n#include <cstdint>\n#include \"Bitfield.hpp\"\n\nusing namespace " + NAMESPACE + ";\n\nnamespace "
PERIPHERAL_TEMPLATE_STRING = TAB + "template <std::uint32_t BaseAddress, std::uint16_t Irq>\n"
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

# leading digits not allowed
# ------------------------------------------------------------------------------

def move_trailing_leading_digits(name):
    for c in name:
        if c.isdigit():
            name = name.replace(c, "")
            name += c
    return name

#  different SVDs may contain different tags for elements, use one standard
# ------------------------------------------------------------------------------

def standardize(field):
    if "access" in field:
        if field["access"] == "read-only":
            field["access"] = "Access::ReadOnly"
        elif field["access"] == "write-only":
            field["access"] = "Access::WriteOnly"
        elif field["access"] == "read-write":
            field["access"] = "Access::ReadWrite"
    else:
        field["access"] = "Access::ReadWrite"
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

def check_and_add_fieldtype(field):
    global types
    datatype = field["name"].title() + "Type"
    exists = False
    for t in types:
        # type of that name already exists
        if datatype in t:
            exists = True
    if exists is False:
        typestring = TAB + "enum class " + datatype + " {\n"
        for key in field["enumeratedValues"]:
            # todo: remove leading digits
            # not all fields have correct values, so check if string only
            elem = field["enumeratedValues"][key]
            if isinstance(elem, basestring) == False:
                typestring += TAB + TAB + move_trailing_leading_digits(elem["name"]) + " = " + elem["value"] + ",\n"
        typestring = typestring[:-2] + "\n" # delete comma for last item
        typestring += TAB + "};\n"
        types.append(typestring)
    return datatype

# ------------------------------------------------------------------------------

def write_field(register, field, header_file):
    field = standardize(field)
    datatype = ""
    # add a global type this field
    if "enumeratedValues" in field and len(field["enumeratedValues"]) > 2:
        datatype = check_and_add_fieldtype(field)
    else:
        datatype = type_for_bitwidth(int(field["bitWidth"]))
    if register["name"] == field["name"]:
        field["name"] = "Value"

    template_params = [register["name"].title(), datatype, field["bitOffset"], field["bitWidth"], field["access"]]

    header_file.write(TAB + TAB + TAB + "using " + field["name"].title() + " = Bitfield<" + ", ".join(template_params) + ">;\n")

# ------------------------------------------------------------------------------

def write_register(register, header_file):
    header_file.write(TAB + TAB + "struct " + register["name"].title() + " {\n")
    header_file.write(TAB + TAB + TAB + "using WidthType = " + REGISTER_WIDTH_TYPE + ";\n")
    header_file.write(TAB + TAB + TAB + "static constexpr std::uint32_t Address = BaseAddress + " + register["addressOffset"].lower() + ";\n")
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
    header_file.write(TAB + TAB + "};\n")

# ------------------------------------------------------------------------------

def write_peripheral(peripheral, header_file):
    header_file.write(PERIPHERAL_TEMPLATE_STRING)
    header_file.write(TAB + "struct Controller {\n")
    header_file.write(TAB + TAB + "static constexpr std::uint32_t Interrupt = Irq;\n")
    for register in peripheral["registers"].items():
        write_register(register[1], header_file)
    header_file.write(TAB + "};")

# ------------------------------------------------------------------------------

def write_instance(instance, header_file):
    interrupt = "0xFF"
    if "interrupt" in instance:
        interrupt = instance["interrupt"]["Value"]
    header_file.write("\n" + TAB + "using " + instance["name"].title() + " = " + INNER_NAMESPACE + "::" + strip_trailing_digits(instance["name"].title().lower()) + "::Controller<" + instance["baseAddress"] + ", " + interrupt + ">;")

# ------------------------------------------------------------------------------

def main():
    generate_types = False
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

    fileURI = "header/" + device_name + "_" + strip_trailing_digits(peripheral_name.lower()) + ".hpp"
    if not os.path.exists(os.path.dirname(fileURI)):
        os.makedirs(os.path.dirname(fileURI))
    header_file = open(fileURI, "w")
    header_file.write(FILE_HEADER + device_name + " {\nnamespace " + INNER_NAMESPACE + " {\nnamespace " + strip_trailing_digits(peripheral_name).lower() + " {\n   // Types\n\n")

    peripheral = peripherals[peripheral_name]

    # if the peripheral selected inherits, take the parent to write header
    if "derivedFrom" in peripheral:
        peripheral_name = peripheral["derivedFrom"].title()
    write_peripheral(peripherals[peripheral_name], header_file)
    header_file.write("\n\n}\n}")
    # write the parent as an instance
    write_instance(peripherals[peripheral_name], header_file)

    # write all peripherals that inherit as instances
    for item in peripherals.items():
        if "derivedFrom" in item[1] and item[1]["derivedFrom"].title() == peripheral_name:
            write_instance(item[1], header_file)

    header_file.write("\n} // end of namespace " + device_name + "\n")

    header_file.close()

    for line in fileinput.input(header_file.name, inplace=True):
        if "Types" in line:
            line = "\n".join(types)
        print line,

    print "File successfully written to " + header_file.name

# ------------------------------------------------------------------------------

if __name__ == "__main__":
    main()
