input_file = 'output_fixed.csv'  # or 'output.csv' if normalized
output_file = 'output_dedup.csv'

seen = set()
with open(input_file, 'r', newline='') as fin, open(output_file, 'w', newline='') as fout:
    for line in fin:
        if line not in seen:
            fout.write(line)
            seen.add(line)

print(f"Removed duplicates. Unique lines saved to {output_file}")
