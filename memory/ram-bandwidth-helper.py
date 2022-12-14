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
        f.write("memset(buf + {} * N, 42, N * sizeof(char));\n".format(i))
        g.write("memcpy(buf2 + {} * N, buf + {} * N, N * sizeof(char));\n".format(i, i))
