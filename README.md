# MBP-10 Order Book Reconstruction

This project reconstructs the MBP-10 top-of-book data from raw MBO order flow.

## Files
- `src/main.cpp`: Main C++11 reconstruction logic.
- `data/mbo.csv`: Input MBO data.
- `data/mbp.csv`: Expected output for validation.
- `compare_output.py`: Python script to compare actual and expected output.
- `Makefile`: Build instructions.

## Usage

```bash
g++ -std=c++11 -O2 src/main.cpp -o reconstruction_shard.exe
./reconstruction_shard.exe data/mbo.csv > output.csv
python compare_output.py
# mbp10-orderbook-reconstruction
