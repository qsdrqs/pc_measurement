import numpy
import statistics
import re
from collections import defaultdict
import matplotlib.pyplot as plt

def main():
    results = defaultdict(list)

    f = open("./access_latency_results.txt", "r")
    for l in f.readlines():
        temp = re.findall("\d*,\d*,\d*\.\d*", l)
        for instance in temp:
            data = instance.split(',')
            results[data[1]].append([float(data[0]), float(data[2])]) # [array size, latency]
    
    for r in results:
        x = []
        y = []
        for d in results[r]:
            x.append(d[0])
            y.append(d[1] * 0.27)
        plt.plot(x,y, label=r)
    plt.legend(loc='upper left')
    plt.title("Access Latency")
    plt.xlabel('log2(Array size)')
    plt.ylabel('Latency (ns)')
    plt.savefig("./access_latency.png")
    pass

    

if __name__ == "__main__":
    main()
