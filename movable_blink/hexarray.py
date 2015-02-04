#!/usr/bin/env python

import sys

# handle input arguments
if len(sys.argv) != 3:
    print("ERROR: Expected input 'hexarray <infile.bin> <outfile.h>'")
    sys.exit(1)
infile_name = sys.argv[1]
outfile_name = sys.argv[2]

with open(outfile_name, 'w') as outfile:
    # start the array
    outfile.write('unsigned char code[] = {')

    # read bytes from bin file
    with open(infile_name, 'r') as infile:
        bin_blob = infile.read()

    # rewrite as hex strings
    for char in bin_blob:
        #print(char)
        outfile.write('0x{:02X}, '.format(ord(char)))

    # finish file
    outfile.write('};\n')



