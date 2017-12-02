rm -rf result*.log
find . -name stats.txt | xargs grep "branchPred.condIncorrect" | sed 's/:/ /g' | sed 's/\// /g' | awk '{ printf("%-15s %20s\n",$2,$5)}' >>result_in.log
find . -name stats.txt | xargs grep "atLeastOneCorrectExpert" | sed 's/:/ /g' | sed 's/\// /g' | awk '{ printf("%10s\n",$5)}'  >>result_on.log
paste result_in.log result_on.log >> result_comb.log
cat result_comb.log | awk '{print ($3)*100/$2}' > result_per.log
#cat result_comb.log | awk '{print ($2 - $3)*100/$2}' > result_per.log
paste result_comb.log result_per.log >> result_unsorted.log
sort -k 4 -rn result_unsorted.log > result.log
sed -i '1s/^/Algorithm               condIncorrect   Atleast1Cor    %ImpPoss\n/' result.log
sed -i '2s/^/-------------------------------------------------------------------\n/' result.log
rm -rf result_*
#find . -name stats.txt | xargs grep "branchPred.condIncorrect" | sed 's/:/ /g' | sed 's/\// /g' | awk '{ printf("%-15s %25s %20s\n",$2,$4,$5)}' >>resultin.log
#find . -name stats.txt | xargs grep "atLeastOneCorrectExpert" | sed 's/:/ /g' | sed 's/\// /g' | awk '{ printf("%-15s %25s %10s\n",$2,$4,$5)}'  >>resulton.log
#sort -r result.log >> result_sorted.log
