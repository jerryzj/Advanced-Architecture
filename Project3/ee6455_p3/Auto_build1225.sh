#!/bin/bash -uxe

# 1. Run in bimod mode
# Set config file, "bimod" prediction 
sed -i '16s/.*/-bpred                       bimod/' default.baseline.cfg
sed -i '17s/.*/-bpred:bimod 4096/' default.baseline.cfg
# Set branch delay
sed -i '35s/.*/-fetch:mplat 3/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_bimod.py ./results
cd results
sudo python parse_bimod.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/bimod_3
# Clean
rm -rf ./results
mkdir results

# Set branch delay
sed -i '35s/.*/-fetch:mplat 9/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_bimod.py ./results
cd results
sudo python parse_bimod.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/bimod_9
# Clean
rm -rf ./results
mkdir results

# 2. PAg
# Set config file
sed -i '16s/.*/-bpred                       2lev/' default.baseline.cfg
sed -i '17s/.*/-bpred:2lev 1024 4096 12 0 /' default.baseline.cfg
sed -i '18s/.*/-bpred:btb 4 4096/' default.baseline.cfg
# Set branch delay
sed -i '35s/.*/-fetch:mplat 3/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_2lev.py ./results
cd results
sudo python parse_2lev.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/PAg_3
# Clean
rm -rf ./results
mkdir results

# Set branch delay
sed -i '35s/.*/-fetch:mplat 9/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_2lev.py ./results
cd results
sudo python parse_2lev.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/PAg_9
# Clean
rm -rf ./results
mkdir results

# 3. GAg
# Set config file, "bimod" prediction 
sed -i '16s/.*/-bpred                       2lev/' default.baseline.cfg
sed -i '17s/.*/-bpred:2lev 1 4096 12 0 /' default.baseline.cfg
sed -i '18s/.*/-bpred:btb 4 4096/' default.baseline.cfg
# Set branch delay
sed -i '35s/.*/-fetch:mplat 3/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_2lev.py ./results
cd results
sudo python parse_2lev.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/GAg_3
# Clean
rm -rf ./results
mkdir results

# Set branch delay
sed -i '35s/.*/-fetch:mplat 9/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_2lev.py ./results
cd results
sudo python parse_2lev.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/GAg_9
# Clean
rm -rf ./results
mkdir results

# 4. Run in Alpha21264 mode
# Set config file, "bimod" prediction 
sed -i '16s/.*/-bpred                      Comb/' default.baseline.cfg
sed -i '17s/.*/#-bpred:2lev 1 4096 12 0 /' default.baseline.cfg
sed -i '18s/.*/#-bpred:btb 4 4096/' default.baseline.cfg
# Set branch delay
sed -i '35s/.*/-fetch:mplat 3/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_comb.py ./results
cd results
sudo python parse_comb.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/comb_3
# Clean
rm -rf ./results
mkdir results
#test
# Set branch delay
sed -i '35s/.*/-fetch:mplat 9/' default.baseline.cfg
# Run SPEC2000 benchmarks
make -f SPEC2000.make all -j8
# Copy parse script and run
sudo cp parse_comb.py ./results
cd results
sudo python parse_comb.py
cd ..
# Copy test results
sudo cp -r ./results ~/Results/comb_9
# Clean
rm -rf ./results
mkdir results
