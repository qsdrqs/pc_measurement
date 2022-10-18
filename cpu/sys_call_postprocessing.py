import statistics


def main():
    f = open("./sys_call_results.txt", "r")
    line = f.read()
    data = line.split(', ')
    data = data[:-1]

    data = [float(i) for i in data]
    data = [i-47 for i in data] # 47 is the measurement overhead

    mean = statistics.mean(data)
    sd = statistics.stdev(data)

    print(f"mean: {mean}\nsd: {sd}")



if __name__ == "__main__":
    main()