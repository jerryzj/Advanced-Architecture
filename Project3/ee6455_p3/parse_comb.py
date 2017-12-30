import glob

FILE_OUT = "summary.txt"
file_out = open(FILE_OUT, 'w')
for FILE_IN in glob.glob("*.stderr"):
	file_out.write(FILE_IN + '\n')
	file_in = open(FILE_IN)
	for line in file_in:
		if "bpred_comb.bpred_dir_rate" in line:
			file_out.write(line)
		if "sim_IPC" in line:
			file_out.write(line)
	file_out.write("\n")
	file_in.close()

file_out.close()