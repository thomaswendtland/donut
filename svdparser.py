
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

# This file is used by 'makecpp_header.py' to create an 'OrderedDict' from a
# vendor-supplied SVD file for a specific MCU. The XML structure is, for practical
# reasons, not represented to a 100%

# ------------------------------------------------------------------------------

#!/usr/bin/python

import xml.etree.ElementTree as et
from collections import OrderedDict
import re

# strip non alpha-numerical characters
# ------------------------------------------------------------------------------

def sanitize(name):
    return re.sub(r'\W+', '', name)

# ------------------------------------------------------------------------------

def parse_individual(node):
    if node is None:
        return OrderedDict()
    individual = OrderedDict()
    for a in node.attrib:
        individual[a] = node.get(a)
    if len(node.getchildren()) == 0:
        return sanitize(node.text.title())
    for element in node:
        if len(list(element.iter())) == 1:
            individual[element.tag] = sanitize(element.text.title())
        else:
            individual[element.tag] = parse_group(element)
    return individual

# ------------------------------------------------------------------------------

def parse_group(node):
    group = OrderedDict()
    for element in node:
        keytag = element.find("name")
        key = ""
        if keytag is None:
            key = sanitize(element.tag.title())
        else:
            key = keytag.text.title()
        group[key] = parse_individual(element)

    return group

# ------------------------------------------------------------------------------

def run(file):
    try:
        device_node = et.parse(file).getroot()
    except:
        print "SVD parser: invalid file"
        return None
    peripherals_node = device_node.find("peripherals")
    device = OrderedDict()

    peripherals = parse_group(peripherals_node)
    device[device_node.find("name").text.title()] = peripherals
    #print device
    return device
