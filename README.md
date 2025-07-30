# MBP-10 Order Book Reconstruction

MBP-10 Order Book Reconstruction - README
==========================================

Author: Radhika Sharda  
GitHub: https://github.com/Radhika-SHARDA/mbp10-orderbook-reconstruction

Overview
--------
This project reconstructs the MBP-10 (Market By Price, top 10 levels) order book from MBO (Market By Order) data provided in a CSV file format. It emits a snapshot only when the top 10 levels of the order book change.

Key Features
------------
1. Handles all supported MBO actions: Add (A), Modify (M), Cancel (C), Trade (T), Fill (F), and Reset (R).
2. Ensures strict snapshot emission logic: one line per snapshot, only when the top 10 levels change.
3. Correctly models implied trade cancellations using the T → F → C pattern as described in the prompt.
4. Ignores trades with unspecified side (`side == 'N'`) as per requirements.
5. Optimized snapshot formatting using fixed-precision CSV output.

Special Instructions and Rules Followed
---------------------------------------
✔️ The first row (`action == R`) is ignored — we assume an initially empty order book.

✔️ [T]rade, [F]ill, and [C]ancel actions are treated as a single logical update:
   - The `T` action does not immediately affect the order book.
   - The `C` action that follows it *does* affect the book, on the **opposite side** of the trade.

✔️ If a `T` action has side = 'N', it is **ignored completely**.

✔️ The snapshot is emitted only if there is a change in the top 10 bid or ask levels (including price or quantity).

Build Instructions
------------------
This project is built using `g++` with C++11 standard.

```bash
g++ -std=c++11 -O2 src/main.cpp -o reconstruction_shard.exe
