rm -rf result*.log
find . -name stats.txt | xargs grep "branchPred.condIncorrect" | sed 's/:/ /g' | sed 's/\// /g' | awk '{ printf("%-15s %25s %20s\n",$2,$4,$5)}' >>result.log
find . -name stats.txt | xargs grep "atLeastOneCorrectExpert" | sed 's/:/ /g' | sed 's/\// /g' | awk '{ printf("%-15s %25s %10s\n",$2,$4,$5)}'  >>result.log
sort -r result.log >> result_sorted.log
