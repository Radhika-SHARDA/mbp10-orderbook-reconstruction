import csv


def load_csv(path):
    with open(path, 'r', newline='') as f:
        return list(csv.reader(f))


def normalize_row(row):
    return [cell.strip() for cell in row]


def trim_trailing_empty(row):
    while row and row[-1] == '':
        row.pop()
    return row


def compare_files(file1, file2):
    rows1 = load_csv(file1)
    rows2 = load_csv(file2)

    if len(rows1) != len(rows2):
        print(f"❌ Line count mismatch: {len(rows1)} vs {len(rows2)}")
        return

    mismatch_count = 0

    for i, (r1, r2) in enumerate(zip(rows1, rows2), start=1):
        nr1 = trim_trailing_empty(normalize_row(r1))
        nr2 = trim_trailing_empty(normalize_row(r2))

        if nr1 != nr2:
            mismatch_count += 1
            print(f"\n🔸 Mismatch at line {i}:")

            print(f"  ➤ output.csv: {nr1}")
            print(f"  ➤ mbp.csv:   {nr2}")

            if mismatch_count >= 10:
                print("...stopping after 10 mismatches")
                break

    if mismatch_count == 0:
        print("✅ output.csv matches mbp.csv exactly!")


if __name__ == "__main__":
    compare_files("output.csv", "data/mbp.csv")
