#!/bin/bash

# Usage: ./analyze_performance.sh <binary_name>
BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary_name>"
    exit 1
fi

OUTPUT_FILE="performance_metrics.csv"
echo "Threads,Time_Elapsed,User_Time,Sys_Time,IPC,CPU_Utilization,Frequency_GHz" > $OUTPUT_FILE

THREAD_COUNTS=(1 2 3 4 5 6 7 8)

echo "Starting performance analysis for $BINARY..."

for T in "${THREAD_COUNTS[@]}"; do
    echo "---------------------------------------"
    echo "Running with $T threads..."
    
    export OMP_NUM_THREADS=$T
    
    PROG_OUTPUT=$(./"$BINARY")
    echo "Program Output: $PROG_OUTPUT"
    
    perf stat -o perf_tmp.txt ./"$BINARY" > /dev/null

    TIME=$(grep "seconds time elapsed" perf_tmp.txt | awk '{print $1}')
    USER=$(grep "seconds user" perf_tmp.txt | awk '{print $1}')
    SYS=$(grep "seconds sys" perf_tmp.txt | awk '{print $1}')
    IPC=$(grep "insn per cycle" perf_tmp.txt | awk '{print $4}')
    UTIL=$(grep "CPUs utilized" perf_tmp.txt | awk '{print $4}')
    FREQ=$(grep "GHz" perf_tmp.txt | awk '{print $4}')

    echo "$T,$TIME,$USER,$SYS,$IPC,$UTIL,$FREQ" >> $OUTPUT_FILE
done

rm perf_tmp.txt
echo "---------------------------------------"
echo "Analysis complete. Data saved to $OUTPUT_FILE."
