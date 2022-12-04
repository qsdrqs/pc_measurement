import os

if __name__ == '__main__':
    basepath = "./build/file-read/"
    if not os.path.exists(basepath):
        os.mkdir(basepath)

    for i in range(0, 11):
        if not os.path.exists(basepath + "/{}".format(i)):
            f = open(basepath + "/{}".format(i), "w")
            f.write("a" * (2 ** i) * 1024)
            f.close()
