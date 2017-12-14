#!/usr/bin/env awk -f
#
# Get the branch prediction accuracy from a stats.txt file
#
# Usage: accuracy.awk FILE
#
# Andrew Pensinger

/branchPred\.condPredicted/ {
    correct = $2
}

/branchPred\.condIncorrect/ {
    incorrect = $2
}

END {
    accuracy = correct / (correct + incorrect) * 100
    print accuracy
}
