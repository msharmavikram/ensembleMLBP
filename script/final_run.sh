#!/bin/sh

build/ARM/gem5.fast -d m5out/step1 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step1 -o '10'  &
build/ARM/gem5.fast -d m5out/step2 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step2 -o '9'   &
build/ARM/gem5.fast -d m5out/step3 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step3 -o '5 2' &
build/ARM/gem5.fast -d m5out/step4 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step4 -o '5 2' &
build/ARM/gem5.fast -d m5out/step5 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step5 -o '6'
build/ARM/gem5.fast -d m5out/step6 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step6 -o '4'
build/ARM/gem5.fast -d m5out/step7 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step7 -o '9'
# build/ARM/gem5.fast -d m5out/step8 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step8 -o '4'   >m5out/step8/run.log
# build/ARM/gem5.fast -d m5out/step9 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step9 -o '8'   >m5out/step9/run.log
build/ARM/gem5.fast -d m5out/step10 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/microbench/Step10
build/ARM/gem5.fast -d m5out/Bubblesort ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Bubblesort &
# build/ARM/gem5.fast -d m5out/FloatMM    ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/FloatMM
# build/ARM/gem5.fast -d m5out/IntMM      ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/IntMM
# build/ARM/gem5.fast -d m5out/Oscar      ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Oscar &
build/ARM/gem5.fast -d m5out/Perm       ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Perm &
build/ARM/gem5.fast -d m5out/Puzzle     ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Puzzle
# build/ARM/gem5.fast -d m5out/Queens     ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Queens
build/ARM/gem5.fast -d m5out/Quicksort  ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Quicksort
# build/ARM/gem5.fast -d m5out/RealMM     ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/RealMM
# build/ARM/gem5.fast -d m5out/Towers     ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Towers
# build/ARM/gem5.fast -d m5out/Treesort   ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/se-benchmarks/Treesort
build/ARM/gem5.fast -d m5out/soplex ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ../benchmark/soplex/soplex_base.arm -o '-m10000 ../benchmark/soplex/test.mps'
# echo "Running Parsec Blackscholes Simple"
# build/ARM/gem5.fast -d m5out/blackscholes ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c /home/vikram/coursework/ece511/ensembleMLBP/benchmark/blackscholes/src/blackscholes -o '1 /home/vikram/coursework/ece511/ensembleMLBP/benchmark/blackscholes/inputs/in_4.txt out.log'
# echo "Running Parsec Blackscholes Complex"
# build/ARM/gem5.fast -d m5out/blackscholes_16 ./configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c /home/vikram/coursework/ece511/ensembleMLBP/benchmark/blackscholes/src/blackscholes -o '1 /home/vikram/coursework/ece511/ensembleMLBP/benchmark/blackscholes/inputs/in_16.txt out.log'
