#!/usr/bin/env python

import sys
import glob

file_list = glob.glob("*.map") + glob.glob("*.Map")

for infile in file_list:
    section = None
    text_liblen = 0
    data_liblen = 0
    with open(infile, 'r') as f:
        for line in f:
            if line.startswith(".text"):
                section = 'text'
                continue
            if line.startswith(".data"):
                section = 'data'
                continue
            if line[0] != ' ':
                section = None
                continue

            if section == 'text':
                data = line.split()

                # look for lines specifying an input file
                if data[-1].startswith('/home/brghena/toolchains/'):
                    text_liblen += int(data[-2], 16)

            if section == 'data':
                data = line.split()

                # look for lines specifying an input file
                if data[-1].startswith('/home/brghena/toolchains/'):
                    data_liblen += int(data[-2], 16)


    print(str(infile) + " library length: " + str(text_liblen))
    print('\t' + " data usage: " + str(data_liblen))

