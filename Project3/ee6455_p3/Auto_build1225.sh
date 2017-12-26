#!/bin/bash -uxe

# Set config file, "bimod" prediction 
sed -i '15s/.*/-bpred                       bimod/' default.baseline.cfg
sed -i '16s/.*/-bpred:bimod 1024/' default.baseline.cfg
# Run SPEC2000 benchmarks
# make -f SPEC2000.make all
# Copy test results
# sudo cp -r ./results ~/Results/bimod
# Clean
# rm -rf ./results
# mkdir results
