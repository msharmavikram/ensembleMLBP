#!/bin/bash

if [ "$1" = "libquantum" ]; then
	./build/ARM/gem5.fast configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ./libquantum/libquantum_base.arm -o '33 5'
else if [ "$1" = "soplex" ]; then
	./build/ARM/gem5.fast configs/example/se.py --caches --l2cache --cpu-type=O3_ARM_v7a_3 -c ./soplex/soplex_base.arm -o '-m10000 ./soplex/test.mps'
fi
fi
