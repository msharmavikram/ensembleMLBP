import sys
import os
import re

# Benchmarks to parse
benchmarks = ["step1", "step2", "step3", "step4", "step5", "step6", "step7", "Bubblesort", "Perm", "Puzzle", "Quicksort", "soplex"]
phrases = ["condPredicted", "condIncorrect", "atLeastOneCorrectExpert", "oneCorrectExpert", "twoCorrectExpert" "allExpertsWrong", "allExpertsRight", "lowWeightExpertsWon"]

predicted = 0
mispredicted = 0
atleastonecorrect = 0
allwrong = 0
allright = 0
lowweightwon = 0
majoritycorrect = 0
onecorrectexpert = 0
twocorrectexpert = 0

# Directory with results
directory = sys.argv[1]

for benchmark in benchmarks:
	fileName = directory + "/" + benchmark + "/stats.txt"
	statLog = open(fileName, "r")
	for line in statLog:
		words = line.split()
		if "condPredicted" in line:
			predicted = int(words[1])
		elif "condIncorrect" in line:
			mispredicted = int(words[1])
		elif "atLeastOneCorrectExpert" in line:
			atleastonecorrect = int(words[1])
		elif "oneCorrectExpert" in line:
			onecorrectexpert = int(words[1])
		elif "twoCorrectExpert" in line:
			twocorrectexpert = int(words[1])
		elif "allExpertsWrong" in line:
			allwrong = int(words[1])
		elif "allExpertsRight" in line:
			allright = int(words[1])
		elif "lowWeightExpertsWon" in line:
			lowweightwon = int(words[1])
	print "Benchmark: {}".format(benchmark)
	print "Accuracy: {}".format(100*float(mispredicted)/(mispredicted+predicted))
	print "AtLeastOneCorrect: {}".format(100*float(atleastonecorrect)/(mispredicted))
	print "MajorityCorrect: {}".format(100*float(twocorrectexpert)/(mispredicted))
	print "AllExpertsWrong: {}".format(100*float(allwrong)/mispredicted)
	print "UniformVote: {}".format(100*float(allwrong+allright)/(predicted+mispredicted))
	print "LowWeightsWon: {}".format(100*float(lowweightwon)/(predicted+mispredicted))

# for benchmark in benchmarks:
# 	fileName = directory + "/" + benchmark + "/stats.txt"
# 	statLog = open(fileName, "r")
# 	for line in statLog:
# 		for phrase in phrases:
# 			if phrase in line:
# 				words = line.split()
# 				print "Benchmark: {}, Key: {}, Count: {}".format(benchmark, phrase, words[1])

