
import svdparser
import sys

# ------------------------------------------------------------------------------

SVD_FILE_NAME = "svds/ATSAM3X8E.svd"

peripheral_to_write = "UART"

NAMESPACE = "donut"

FILE_HEADER = "\n#pragma once\n\n#include <cstdint>\n#include \"Bitfield.hpp\"\n\nusing namespace " + NAMESPACE + ";\n\nnamespace "
PERIPHERAL_TEMPLATE_STRING = "\ttemplate <std::uint32_t BaseAddress, std::uint16_t Irq, typename ... Pins>\n"
REGISTER_WIDTH_TYPE = "std::uint32_t"

# ------------------------------------------------------------------------------

def cpp_access_type_for_xmlvalue(xmlvalue):
    if xmlvalue == "read-only":
        return "AccessType::Read"
    elif xmlvalue == "write-only":
        return "AccessType::Write"
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

    print device_name

    peripherals = device_as_dict.items()[0][1]
    if peripheral_name.title() not in peripherals:
        print "Error: no such peripheral in the SVD provided"
        exit(-1)

    header_file = open("header/" + device_name + "_" + peripheral_to_write.lower() + ".hpp", "w")
    header_file.write(FILE_HEADER + device_name + " {\n")

    write_peripheral(peripherals[peripheral_name.title()], header_file)
    header_file.write("\n")
    for instance in peripherals[peripheral_name.title()]["instances"].items():
        write_instance(instance[1], header_file)

    header_file.write("\n} // end of namespace " + device_name)
    header_file.close()


# ------------------------------------------------------------------------------

if __name__ == "__main__":
    main()
