#!/usr/bin/python

import xml.etree.ElementTree as et
from collections import OrderedDict
import sys


GROUP_KEY = "groupName"
INSTANCES_KEY = "otherInstances"

# ------------------------------------------------------------------------------

def parse_individual(node, relevant_tags):
    individual = OrderedDict()
    for element in node:
        if len(list(element.iter())) == 1:
            individual[element.tag] = element.text
        elif element.tag == relevant_tags[0]:
            individual[element.tag] = parse_group(element, relevant_tags[1:])

    return individual

# ------------------------------------------------------------------------------

def parse_group(node, relevant_tags):
    group = OrderedDict()
    for element in node:
        # the if/else clause is just for 'peripheral' nodes
        if element.get("derivedFrom") is None:
            group[element.find(relevant_tags[0]).text] = parse_individual(element, relevant_tags[1:])
        else:
            # this is an instance that dervies
            if INSTANCES_KEY not in group[element.find(GROUP_KEY).text]:
                group[element.find(GROUP_KEY).text][INSTANCES_KEY] = OrderedDict()
            else:
                # add instance
                instance = OrderedDict()
                for kv in element:
                    instance[kv.tag] = kv.text
                group[element.find(GROUP_KEY).text][INSTANCES_KEY].update({element.find("name").text : instance})
    return group

# ------------------------------------------------------------------------------

def run(file):
    try:
        device = et.parse(file).getroot()
    except:
        print "SVD parser: invalid file"
        return None
    peripherals_node = device.find("peripherals")
    peripherals = parse_group(peripherals_node, ["groupName", "registers", "name", "fields", "name"])
    return peripherals
