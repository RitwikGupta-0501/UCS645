#!/bin/bash

# Usage: ./analyze_performance.sh <binary_name>
BINARY=$1

if [ -z "$BINARY" ]; then
  echo "Usage: $0 <binary_name>"
  exit 1
fi

OUTPUT_FILE="performance_metrics.csv"
# Header for CSV
echo "Threads,Real_Time_s,User_Time_s,Sys_Time_s,IPC,Context_Switches,Cache_Misses" >$OUTPUT_FILE

# Detected physical cores (optional, for reference)
CORES=$(nproc)
echo "Detected $CORES logical cores."

# Array of thread counts to test
THREAD_COUNTS=(1 2 4 8) # Matches your assignment requirements

echo "Starting performance analysis for $BINARY..."

for T in "${THREAD_COUNTS[@]}"; do
  echo "---------------------------------------"
  echo "Running with OMP_NUM_THREADS=$T ..."

  export OMP_NUM_THREADS=$T

  # Run perf stat AND the binary in one go.
  # 2> saves perf output (stderr) to perf_tmp.txt
  # 1> saves program output (stdout) to prog_tmp.txt
  perf stat -o perf_tmp.txt -e cycles,instructions,cache-misses,context-switches,cpu-migrations,page-faults ./"$BINARY" >prog_tmp.txt

  # Extract Metrics from perf output (stderr)
  # Note: awk field positions might vary slightly by perf version; check perf_tmp.txt if empty
  REAL_TIME=$(grep "seconds time elapsed" perf_tmp.txt | awk '{print $1}')
  USER_TIME=$(grep "seconds user" perf_tmp.txt | awk '{print $1}')
  SYS_TIME=$(grep "seconds sys" perf_tmp.txt | awk '{print $1}')
  IPC=$(grep "insn per cycle" perf_tmp.txt | awk '{print $4}')
  CS=$(grep "context-switches" perf_tmp.txt | awk '{print $1}' | tr -d ',')
  CACHE_MISS=$(grep "cache-misses" perf_tmp.txt | awk '{print $1}' | tr -d ',')

  # Optional: Extract "Time" or "Energy" from your C++ program output (stdout)
  # Assuming your C++ prints "Time (s): X.XXXX"
  # INTERNAL_TIME=$(grep "Time" prog_tmp.txt | awk '{print $2}')

  echo "$T,$REAL_TIME,$USER_TIME,$SYS_TIME,$IPC,$CS,$CACHE_MISS" >>$OUTPUT_FILE

  # Print a quick summary to console so you know it's working
  echo "Completed $T threads. Time: $REAL_TIME s, IPC: $IPC"
done

# Cleanup
rm perf_tmp.txt prog_tmp.txt
echo "---------------------------------------"
echo "Analysis complete. Data saved to $OUTPUT_FILE."
