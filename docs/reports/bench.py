import os

binaryPath = "C:/Users/micce/source/repos/voxomap_lib/x64/Release/benchmark.exe"

def launchBenchmark(nb_iteration, min, max):
	for i in range(0, nb_iteration):
		for e in range(min, max + 1):
			cmd = binaryPath + " " + str(e)
			os.system(cmd)

launchBenchmark(10, 0, 21)