# ============================================================
# Makefile for Correlation Assignment (UCS645)
# ============================================================

# Compiler and base flags
CXX      = g++
CXXFLAGS = -std=c++11 -Wall -O2 -fopenmp

# Source files
SOURCES  = main.cpp correlate.cpp
HEADERS  = correlate.h

# Object files (generated per target via build rules)
OBJ_SEQ  = main_seq.o  correlate_seq.o
OBJ_PAR  = main_par.o  correlate_par.o
OBJ_OPT  = main_opt.o  correlate_opt.o

# Executables
TARGET_SEQ = corr_sequential
TARGET_PAR = corr_parallel
TARGET_OPT = corr_optimized

# ============================================================
# Default: build all three versions
# ============================================================
all: $(TARGET_SEQ) $(TARGET_PAR) $(TARGET_OPT)

# ============================================================
# Sequential build  (-DSEQUENTIAL disables OpenMP in correlate)
# ============================================================
$(TARGET_SEQ): $(OBJ_SEQ)
	$(CXX) $(CXXFLAGS) -o $@ $^

main_seq.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -DSEQUENTIAL -c $< -o $@

correlate_seq.o: correlate.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -DSEQUENTIAL -c $< -o $@

# ============================================================
# Parallel build  (-DPARALLEL enables OpenMP loops)
# ============================================================
$(TARGET_PAR): $(OBJ_PAR)
	$(CXX) $(CXXFLAGS) -o $@ $^

main_par.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -DPARALLEL -c $< -o $@

correlate_par.o: correlate.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -DPARALLEL -c $< -o $@

# ============================================================
# Optimized build  (default branch in correlate.cpp)
# Adds -march=native for SIMD / AVX auto-vectorization
# ============================================================
$(TARGET_OPT): $(OBJ_OPT)
	$(CXX) $(CXXFLAGS) -march=native -o $@ $^

main_opt.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -march=native -c $< -o $@

correlate_opt.o: correlate.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -march=native -c $< -o $@

# ============================================================
# Convenience run targets  (usage: make run_seq NY=500 NX=1000 T=4)
# ============================================================
NY ?= 500
NX ?= 1000
T  ?= 4

run_seq: $(TARGET_SEQ)
	./$(TARGET_SEQ) $(NY) $(NX) 1

run_par: $(TARGET_PAR)
	./$(TARGET_PAR) $(NY) $(NX) $(T)

run_opt: $(TARGET_OPT)
	./$(TARGET_OPT) $(NY) $(NX) $(T)

# ============================================================
# perf stat wrappers  (requires Linux perf tool)
# ============================================================
perf_seq: $(TARGET_SEQ)
	perf stat ./$(TARGET_SEQ) $(NY) $(NX) 1

perf_par: $(TARGET_PAR)
	perf stat ./$(TARGET_PAR) $(NY) $(NX) $(T)

perf_opt: $(TARGET_OPT)
	perf stat ./$(TARGET_OPT) $(NY) $(NX) $(T)

# ============================================================
# Clean all build artifacts
# ============================================================
clean:
	rm -f $(OBJ_SEQ) $(OBJ_PAR) $(OBJ_OPT) \
	      $(TARGET_SEQ) $(TARGET_PAR) $(TARGET_OPT)

.PHONY: all clean run_seq run_par run_opt perf_seq perf_par perf_opt
