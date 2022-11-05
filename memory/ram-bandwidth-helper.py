#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Author: qsdrqs <t7zhou@ucsd.edu>

This file is used to generate loop unrolling codes

'''

if __name__ == '__main__':
    f = open('ram-bandwidth-w-helper.h', 'w')
    g = open('ram-bandwidth-rw-helper.h', 'w')
    for i in range(1024):
        f.write("memset(buf + " + str(i) + " * N, 42, N * sizeof(char));\n")
        g.write("memcpy(buf2 + " + str(i) + " * N, buf + " + str(i) + " * N, N * sizeof(char));\n")
