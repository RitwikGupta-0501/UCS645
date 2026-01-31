# Parallel & Distributed Computing - Assignment 1

This branch contains the implementation and performance analysis for **Assignment 1** of the UCS645 course, focused on benchmarking OpenMP kernels.

## Author

**Ritwik Gupta**

## System Environment

Benchmarks were conducted on the following hardware:

* **Processor:** Intel Core i7-8550U (4 Physical Cores, 8 Logical Cores)
* **Memory:** 8 GiB
* **OS:** EndeavourOS (Wayland)
* **Kernel:** 6.18.6-zen1-1-zen

## Contents

### Source Code

* **`q1.c`**: Implementation of a **DAXPY loop** (Double precision ).
* **`q2_1d.c` / `q2_2d.c**`: Parallel **Matrix Multiplication** () comparing 1D row-partitioning vs. 2D loop-collapse strategies.
* **`q3.c`**: Numerical integration to approximate the value of **** using the `reduction` clause.

### Automation & Analysis

* **`analyze.sh`**: A BASH script used to automate the execution of OpenMP binaries across 1 to 8 threads while extracting key hardware metrics using the Linux `perf` utility.

### Results

* **`*_metrics.csv`**: CSV files containing raw performance data (Time, IPC, CPU Utilization, etc.) for each task.
* **`Report.pdf`**: A comprehensive analysis of speedup, negative scaling, and microarchitectural bottlenecks observed during testing.

## Usage

To run the performance analysis for a compiled binary:

```bash
./analyze.sh <binary_name>

```

The script will generate a `performance_metrics.csv` file with the extracted data.
