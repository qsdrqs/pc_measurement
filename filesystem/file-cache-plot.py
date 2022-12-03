import csv
from matplotlib import pyplot as plt

if __name__ == '__main__':
    # read results
    with open("./file-cache-res.csv", "r") as f:
        reader = csv.reader(f)
        results = list(reader)
        x = [(2 ** int(row[0])) for row in results]
        y = [float(row[1]) for row in results]
        plt.plot(x, y)
        plt.title("File Cache Size")
        plt.xlabel('Array size(MB)')
        plt.ylabel('Latency (us)')
        plt.savefig("./file-cache.png")
