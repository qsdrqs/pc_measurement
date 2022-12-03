import os

if __name__ == '__main__':
    basepath = "./build/contention-files/"
    if not os.path.exists(basepath):
        os.mkdir(basepath)

    for i in range(0, 20):
        if not os.path.exists(basepath + "/{}".format(i)):
            f = open(basepath + "/{}".format(i), "w")
            f.write("a" * 1024 * 1024 * 50)
            f.close()
