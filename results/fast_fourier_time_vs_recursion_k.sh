#!/bin/bash
# The above shows that it is a bash file

# Create a local variable with a list of K values

KS="2 4 8 16 32 64"
# Iterate over them and print (echo) them
for K in $KS; do
	echo $K;
    # Select the specific value of K and export to other programs
    export HPCE_FFT_RECURSION_K=${K}
    # Run the program with the chosen K, and save to dump_K.csv
    bin/time_fourier_transform hpce.hs2715.direct_fourier_transform_parfor_inner > results/dump_${HPCE_FFT_RECURSION_K}.csv
done

cat results/dump_*.csv > results/all.csv