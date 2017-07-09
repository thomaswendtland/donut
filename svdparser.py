#!/usr/bin/python

import xml.etree.ElementTree as et
from collections import OrderedDict
import sys

INSTANCES_KEY = "otherInstances"

# strip digits at the end of a name
# ------------------------------------------------------------------------------
def normalize(name):
    if name[0].isdigit():
        return name.title() # return if all digits
    while name[len(name)-1].isdigit() == True:
        name = name[:-1]
    return name.title()

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
        return node.text
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
