CC = mpicc
CFLAGS = -Wall -O2 -lm
BUILD_DIR = build

# List of source files
SRCS = q1_daxpy.c q2_bcast.c q3_dotprod.c q4_primes.c q5_perfect.c

# Dynamically generate the target paths (e.g., build/q1_daxpy)
TARGETS = $(patsubst %.c, $(BUILD_DIR)/%, $(SRCS))

all: $(TARGETS)

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Pattern rule: Tells make how to build any executable in the build dir from a .c file
# The '| $(BUILD_DIR)' ensures the directory is created before compiling
$(BUILD_DIR)/%: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

run_all: all
	mpirun -np 4 ./$(BUILD_DIR)/q1_daxpy
	mpirun -np 4 ./$(BUILD_DIR)/q2_bcast
	mpirun -np 4 ./$(BUILD_DIR)/q3_dotprod
	mpirun -np 4 ./$(BUILD_DIR)/q4_primes
	mpirun -np 4 ./$(BUILD_DIR)/q5_perfect
