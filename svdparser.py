
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
# reasons, not represented to a 100%. (TODO: should it be?)

# ------------------------------------------------------------------------------

#!/usr/bin/python

import xml.etree.ElementTree as et
from collections import OrderedDict
import sys

# strip digits at the end of a name
# ------------------------------------------------------------------------------
def normalize(name):
    if name[0].isdigit():
        return name # return if all digits
    while name[len(name)-1].isdigit() == True:
        name = name[:-1]
    return name

# ------------------------------------------------------------------------------

def parse_instance(node):
    instance = OrderedDict()
    for kv in node:
        instance[kv.tag] = kv.text
    return instance

# ------------------------------------------------------------------------------

def parse_individual(node):
    if node is None:
        return OrderedDict()
    individual = OrderedDict()
    # no child nodes for some reason
    if len(node.getchildren()) == 0:
        return node.text.title()
    for element in node:
        if len(list(element.iter())) == 1:
            individual[element.tag] = element.text
        else:
            individual[element.tag] = parse_group(element)

    return individual

# ------------------------------------------------------------------------------

def parse_group(node):
    group = OrderedDict()
    for element in node:
        # this is only relevant for peripheral nodes
        if element.get("derivedFrom") is None:
            keytag = element.find("name")
            key = ""
            if keytag is None:
                key = element.tag
            else:
                key = normalize(keytag.text)
            group[key] = parse_individual(element)
            if element.tag == "peripheral":
                group[key]["instances"] = OrderedDict()
                group[key]["instances"].update({element.find("name").text : parse_individual(element)})
        else:
            parent = normalize(element.get("derivedFrom"))
            group[parent]["instances"].update({element.find("name").text : parse_individual(element)})

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
    device[device_node.find("name").text] = peripherals
    #print device
    return device
