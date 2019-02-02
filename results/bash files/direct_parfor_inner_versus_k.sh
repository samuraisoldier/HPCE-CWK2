#!/bin/bash
# The above shows that it is a bash file

# Create a local variable with a list of K values
#KS="1 2"
KS=$(seq 100)
# Iterate over them and print (echo) them
for K in $KS; do
    # Select the specific value of K and export to other programs
    export HPCE_DIRECT_INNER_K=${K}
    # Run the program with the chosen K, and save to dump_K.csv
    bin/time_fourier_transform hpce.hs2715.direct_fourier_transform_parfor_inner 0 1 "${HPCE_DIRECT_INNER_K}, " > results/dump_${HPCE_DIRECT_INNER_K}.csv
done