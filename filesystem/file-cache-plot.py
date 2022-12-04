import csv
from matplotlib import pyplot as plt

if __name__ == '__main__':
    # read results
    with open("./file-cache-res.csv", "r") as f:
        reader = csv.reader(f)
        results = list(reader)
        x = [(2 ** int(row[0])) for row in results]
        y = [float(row[1]) for row in results]
        plt.plot(x, y, '*')
        plt.grid()
        plt.title("Blocking reading time vs file size")
        plt.xlabel('File size(MB)')
        plt.ylabel('block reading time (us)')
        plt.savefig("./file-cache.png")
