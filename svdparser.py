
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

# This file is used by 'makecpp_header.py' to create an 'OrderedDict' from a
# vendor-supplied SVD file for a specific MCU. The XML structure is, for practical
# reasons, not represented to a 100%

# ------------------------------------------------------------------------------

#!/usr/bin/python3

import xml.etree.ElementTree as et
from collections import OrderedDict
import re
import json

# strip non alpha-numerical characters
# ------------------------------------------------------------------------------

def sanitize(name):
    return re.sub(r'[^0-9a-zA-Z ]', '', name).lower()

# ------------------------------------------------------------------------------

def parse_individual(node):
    if node is None:
        return OrderedDict()
    individual = OrderedDict()
    for a in node.attrib:
        individual[sanitize(a)] = node.get(a).lower()
    if len(node.getchildren()) == 0:
        return sanitize(node.text)
    for element in node:
        if len(list(element.iter())) == 1:
            individual[sanitize(element.tag)] = sanitize(element.text)
        else:
            individual[sanitize(element.tag)] = parse_group(element)
    return individual

# ------------------------------------------------------------------------------

def parse_group(node):
    group = OrderedDict()
    for element in node:
        keytag = element.find("name")
        key = ""
        if keytag is None:
            key = sanitize(element.tag)
        else:
            key = keytag.text.lower()
        group[sanitize(key)] = parse_individual(element)

    return group

# ------------------------------------------------------------------------------

def parse(file):
    try:
        device_node = et.parse(file).getroot()
    except:
        print("svdparser.py: invalid file")
        return None
    peripherals_node = device_node.find("peripherals")
    device = OrderedDict()

    peripherals = parse_group(peripherals_node)
    device[sanitize(device_node.find("name").text)] = peripherals
    return device
