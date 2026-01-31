#!/bin/bash

# Usage: ./analyze_performance.sh <binary_name>
BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary_name>"
    exit 1
fi

# Output CSV file
OUTPUT_FILE="performance_metrics.csv"
echo "Threads,Time_Elapsed,User_Time,Sys_Time,IPC,CPU_Utilization,Frequency_GHz" > $OUTPUT_FILE

# Thread counts to test (tailored for your i7-8550U)
THREAD_COUNTS=(1 2 3 4 5 6 7 8)

echo "Starting performance analysis for $BINARY..."

for T in "${THREAD_COUNTS[@]}"; do
    echo "Running with $T threads..."
    
    # Run perf and capture output to a temporary file
    # We use -o to redirect perf's own output
    export OMP_NUM_THREADS=$T
    perf stat -o perf_tmp.txt ./"$BINARY" > /dev/null

    # Extract metrics using awk/grep
    TIME=$(grep "seconds time elapsed" perf_tmp.txt | awk '{print $1}')
    USER=$(grep "seconds user" perf_tmp.txt | awk '{print $1}')
    SYS=$(grep "seconds sys" perf_tmp.txt | awk '{print $1}')
    IPC=$(grep "insn per cycle" perf_tmp.txt | awk '{print $4}')
    UTIL=$(grep "CPUs utilized" perf_tmp.txt | awk '{print $4}')
    FREQ=$(grep "GHz" perf_tmp.txt | awk '{print $4}')

    # Append to CSV
    echo "$T,$TIME,$USER,$SYS,$IPC,$UTIL,$FREQ" >> $OUTPUT_FILE
done

rm perf_tmp.txt
echo "Analysis complete. Data saved to $OUTPUT_FILE."
