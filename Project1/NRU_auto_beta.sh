#!/bin/bash -uxe

# Set config file as 256KB 8Way
# Note that you must edit the next line to choose swap policy 
sed -i '30s/.*/-cache:dl2             ul2:512:64:8:n/' default.baseline.cfg
sed -i '33s/.*/-cache:dl2lat                    10/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all
# Copy test results
sudo cp -r ./results ~/Results/NRU_256K8W
# Clean
rm -rf ./results
mkdir results

# Set config file as 256KB 16Way
sed -i '30s/.*/-cache:dl2             ul2:256:64:16:n/' default.baseline.cfg
sed -i '33s/.*/-cache:dl2lat                    10/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all
# Copy test results
sudo cp -r ./results ~/Results/NRU_256K16W
# Clean
rm -rf ./results
mkdir results

# Set config file as 1MB 8Way
sed -i '30s/.*/-cache:dl2             ul2:2048:64:8:n/' default.baseline.cfg
sed -i '33s/.*/-cache:dl2lat                    14/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all
# Copy test results
sudo cp -r ./results ~/Results/NRU_1M8W
# Clean
rm -rf ./results
mkdir results

# Set config file as 1MB 16Way
sed -i '30s/.*/-cache:dl2             ul2:1024:64:16:n/' default.baseline.cfg
sed -i '33s/.*/-cache:dl2lat                    14/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all
# Copy test results
sudo cp -r ./results ~/Results/NRU_1M16W
# Clean
rm -rf ./results
mkdir results
