import sys
import os
import re

# Benchmarks to parse
benchmarks = ["step1", "step2", "step3", "step4", "step5", "step6", "step7", "step8", "step9", "Bubblesort", "Oscar", "Perm", "Puzzle", "Quicksort"] #"Trees"
phrases = ["condPredicted", "condIncorrect", "atLeastOneCorrectExpert", "allExpertsWrong", "allExpertsRight", "lowWeightExpertsWon"]

# Directory with results
directory = sys.argv[1]

for benchmark in benchmarks:
	fileName = directory + "/" + benchmark + "/stats.txt"
	statLog = open(fileName, "r")
	for line in statLog:
		for phrase in phrases:
			if phrase in line:
				words = line.split()
				print "Benchmark: {}, Key: {}, Count: {}".format(benchmark, phrase, words[1])

