import csv
from matplotlib import pyplot as plt

if __name__ == '__main__':
    # read results
    with open("./nfs-local.csv", "r") as f:
        reader = csv.reader(f)
        results = list(reader)
        x = [int(row[0]) for row in results]
        seq_local = [float(row[1]) for row in results]
        rand_local = [float(row[2]) for row in results]
        seq_nfs = [float(row[3]) for row in results]
        rand_nfs = [float(row[4]) for row in results]
        plt.plot(x, seq_local)
        plt.plot(x, rand_local)
        plt.plot(x, seq_nfs)
        plt.plot(x, rand_nfs)
        plt.legend(["sequencial local", "random local", "sequencial nfs", "random nfs"])
        plt.title("Read time vs file size in local and nfs")
        plt.xlabel('log(File size(KB))')
        plt.ylabel('Block read time(us)')
        plt.savefig("./nfs.png")
