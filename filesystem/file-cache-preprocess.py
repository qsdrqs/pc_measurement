import os
from math import log

# NOTE: this script will use up a large amount of disk space, do not run
#       this script unless you really want to run the test

if __name__ == '__main__':
    basepath = os.environ['HOME'] + "/tmp-files"
    if not os.path.exists(basepath):
        os.mkdir(basepath)

    for i in range(0, int(log(32 * 1024) / log(2))):
        if not os.path.exists(basepath + "/{}".format(i)):
            f = open(basepath + "/{}".format(i), "w")
            f.write("a" * (2 ** i) * 1024 * 1024)
            f.close()
