import csv
from matplotlib import pyplot as plt

if __name__ == '__main__':
    # read results
    with open("./sequencial-random-res.csv", "r") as f:
        reader = csv.reader(f)
        results = list(reader)
        x = [int(row[0]) for row in results]
        seq = [float(row[1]) for row in results]
        rand = [float(row[2]) for row in results]
        plt.plot(x, seq, label="sequencial")
        plt.plot(x, rand, label="random")
        plt.legend(["sequencial", "random"])
        plt.title("Sequencial and random read time vs file size")
        plt.xlabel('log(File size(KB))')
        plt.ylabel('Block read time(us)')
        plt.savefig("./sequencial-random.png")
