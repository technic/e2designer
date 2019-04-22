#!/usr/bin/python

"""Simple python script that converts yaml to json"""

import sys
import yaml
import json

if __name__ == "__main__":
    json.dump(yaml.load(sys.stdin), sys.stdout, indent=2)
