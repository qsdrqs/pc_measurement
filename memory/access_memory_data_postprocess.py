import numpy
import statistics
import re
from collections import defaultdict

def main():
    results = defaultdict(list)

    f = open("./access_latency_results.txt", "r")
    for l in f.readlines():
        temp = re.findall("\d*,\d*,\d*\.\d*", l)
        for instance in temp:
            data = instance.split(',')
            results[data[1]].append([data[0], data[2]]) # [array size, latency]
        
    

if __name__ == "__main__":
    main()
