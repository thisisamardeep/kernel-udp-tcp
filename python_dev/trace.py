input_file = 'amar1.log'
output_file = 'amarfirst.txt'
removed_file = 'amarsecond.txt'

with open(input_file, 'r') as infile:
    lines = infile.readlines()

with open(output_file, 'w') as outfile, open(removed_file, 'w') as remfile:
    for line in lines:
        if '315025' in line:
            remfile.write(line)
        else:
            outfile.write(line)