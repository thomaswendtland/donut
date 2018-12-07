
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

#!/usr/bin/python3

import svdparser
import sys
import json
import writer

# ------------------------------------------------------------------------------

def list_peripherals(l):
    for (k, v) in l.items():
        print("Available peripherals:")
        for(a, b) in v.items():
            if "description" in b:
                print(a + " - " + b["description"])
            else:
                print(a)

# ------------------------------------------------------------------------------

def write_header(device_as_dict, command):
    if len(command) != 2:
        print("generator.py: write command needs a peripheral")
        return
    #print(device_as_dict["nrf52840"])
    peripheral_name = command[1]
    if peripheral_name not in device_as_dict["nrf52840"]:
        print("generator.py - No such peripheral")
        return
    peripheral = device_as_dict["nrf52840"][peripheral_name]
    writer.write(peripheral)

# ------------------------------------------------------------------------------

def print_help():
    print("Commands:")
    print(" l :             list peripherals")
    print(" h :             print this menu")
    print(" w $peripheral : write peripheral header file")

# ------------------------------------------------------------------------------

def dump_json(l):
    print("dumping JSON")
    filename = input("Please provide a filename for the JSON")
    #json.dumps

# ------------------------------------------------------------------------------

def main():
    if len(sys.argv) < 2:
        print("generator - please provide a valid SVD file")
        exit(-1)
    svd_file = sys.argv[1]
    device = svdparser.parse(svd_file)
    #print(device)
    if device is None:
        print("generator - Error parsing the SVD file")
        exit(-1)
    #l = json.loads(device_as_json)
    while(1):
        command_str = input("Enter command (h for help):")
        command_parts = command_str.split(" ")
        if command_parts[0] == "l":
            list_peripherals(device)
        elif command_parts[0] == "w":
            write_header(device, command_parts)
        elif command_parts[0] == "h":
            print_help()
        elif command_parts[0] == "d":
            dump_json(device)
        elif command_parts[0] == "p":
            print(json.dumps(list(device.values())[0]["ficr"], indent=4, sort_keys=True))
        elif command_parts[0] == "q":
            exit(-1)
        else:
            print("Invalid command")

# ------------------------------------------------------------------------------

if __name__ == "__main__":
    main()
