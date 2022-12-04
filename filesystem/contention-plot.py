import csv
from matplotlib import pyplot as plt

if __name__ == '__main__':
    # read results
    with open("./contention-res.csv", "r") as f:
        reader = csv.reader(f)
        results = list(reader)
        x = [int(row[0]) for row in results]
        y = [float(row[1]) for row in results]
        plt.plot(x, y, '*-')
        x_list = range(1, 21)
        plt.xticks(x_list)
        plt.title("Blocking reading time vs process number")
        plt.xlabel('proces number')
        plt.ylabel('block reading time (us)')
        plt.savefig("./contention.png")
