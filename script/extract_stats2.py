#!/usr/bin/env python3

import sys
import os
import re

# Benchmarks to parse
benchmarks = [
    "step1",
    "step2",
    "step3",
    "step4",
    "step5",
    "step6",
    "step7",
    "step10",
    "Bubblesort",
    "Perm",
    "Puzzle",
    "Quicksort",
    "soplex"
]

phrases = [
    "condPredicted",
    "condIncorrect",
    "atLeastOneCorrectExpert",
    "allExpertsWrong",
    "allExpertsRight",
    "lowWeightExpertsWon",
    "oneCorrectExpert",
    "twoCorrectExpert",
    "threeCorrectExpert"
]

# Directory with results
directory = sys.argv[1]

for benchmark in benchmarks:
    fileName = directory + "/" + benchmark + "/stats.txt"
    statLog = open(fileName, "r")
    for line in statLog:
        for phrase in phrases:
            if phrase in line:
                words = line.split()
                print("{}\t{}\t{}".format(benchmark, phrase, words[1]))

