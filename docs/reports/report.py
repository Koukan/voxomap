import csv
import matplotlib.pyplot as plt
import pandas as pd
import math

g_color = ['#27b', '#f81', '#3a3', '#d22', '#96c', '#955', '#e7c', '#888', '#cc2', '#1cd', '#1c7']

def shrinkName(name):
    name = name.replace("Container", "C")
    name = name.replace("Super", "Su")
    name = name.replace("Sparse", "S")
    name = name.replace("Array", "A")
    name = name.replace("Sided", "Si")
    name = name.replace("test::voxel", "")
    name = name.replace(",>", ">")
    return name

def createChart(title, index, values):
    data = pd.DataFrame(values, index=index)
    data.plot(kind="bar", rot=0)
    plt.title(title + " benchmark")
    plt.ylabel("million voxels per second")
    plt.legend(bbox_to_anchor=(1.05, 1.0), loc='upper left', fontsize='small')
    plt.grid(axis = 'y')
    plt.savefig(title + ".png", bbox_inches="tight")

def createMultipleChart(title, values, **kwargs):
    order = ["add", "remove", "update", "read", "iterator", "memory"]
    count = 0
    for oi in range(0, len(order)):
        if order[oi] in values:
            count += 1

    nbCol = count
    nbRow = 1
    if count > 4:
        nbCol = 3
        nbRow = math.ceil(count / 3)

    fig, ax = plt.subplots(ncols=nbCol, nrows=nbRow, figsize=(nbCol * 4, nbRow * 3))
    data = []
    labels = []

    ai = 0
    for oi in range(0, len(order)):
        if order[oi] in values:
            subValues = {}
            for key,value in values[order[oi]].items():
                if order[oi] != "memory":
                    subValues[key] = [value / 1000000]
                else:
                    subValues[key] = [value]
                if ai == 0:
                    labels.append(key)

            data.append(pd.DataFrame(subValues))
            axi = ax[math.floor(ai / 3)][ai % 3]
            data[ai].plot(ax=axi, kind="bar", rot=0, legend=None, width=2, color=g_color)
            axi.grid(axis = 'y')
            axi.set_xticks([])
            if order[oi] == "memory":
                axi.set_title(order[oi] + " (MB)")
            else:
                axi.set_title(order[oi])
            ai += 1
    
    for i in range(0, nbRow):
        ax[i][0].set_ylabel("Million voxels / second")
    fig.legend(loc='upper center', fontsize='small', bbox_to_anchor=(0.5, 0.09), labels=labels)
    if "nb_voxel" in kwargs:
        nbVoxel = int(round(kwargs["nb_voxel"] / 100000))
        nbVoxel = nbVoxel * 100
        if nbVoxel >= 1000:
            nbVoxel = str(int(nbVoxel / 1000)) + "M"
        else:
            nbVoxel = str(nbVoxel) + "K"
        fig.suptitle(title.capitalize() + " benchmark (" + nbVoxel + " voxels)", fontsize=14)
    else:
        fig.suptitle(title.capitalize() + " benchmark", fontsize=14)
    plt.savefig(title.replace(" ", "_").lower() + ".png", bbox_inches="tight")

def readBenchmarkFile(path, title_sup, filter):
    benchmark = {}

    with open(path, newline='\n') as csvfile:
        csvFile = csv.reader(csvfile, delimiter=';', quotechar='|')
        for row in csvFile:
            if len(row) != 0:
                if not filter(row[1]):
                    continue

                nb_voxel = 0
                # search nb_voxel
                for value in row:
                    if value.find("nb_voxel") != -1:
                        nb_voxel = value.split("=")[1]

                benchData = row[0] + ";" + nb_voxel
                className = row[1]

                if benchData not in benchmark:
                    benchmark[benchData] = {}
                for i in range(2, len(row)):
                    values = row[i].split("=")
                    if values[0] not in benchmark[benchData]:
                        benchmark[benchData][values[0]] = {}
                    if className not in benchmark[benchData][values[0]]:
                        benchmark[benchData][values[0]][className] = []
                    benchmark[benchData][values[0]][className].append(int(values[1]))

        for benchName,benchValues in benchmark.items():
            for className,classValues in benchValues.items():
                for name,values in classValues.items():
                    values.sort()
                    benchmark[benchName][className][name] = values[math.floor(len(values) / 2)]

        for benchName,benchValues in benchmark.items():
            benchData = benchName.split(";")
            createMultipleChart(title_sup + benchData[0], benchValues, nb_voxel=int(benchData[1]))


readBenchmarkFile("benchmark_report.csv", "", lambda name : name.find("SidedContainer") == -1)
readBenchmarkFile("benchmark_report.csv", "Sided ", lambda name : name.find("SidedContainer") != -1)