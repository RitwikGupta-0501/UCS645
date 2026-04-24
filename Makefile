NVCC = nvcc
COMPUTE_CAP = 50
NVCC_FLAGS = -O2 -arch=sm_$(COMPUTE_CAP)
BUILD_DIR = build

# List of source files
SRCS = q1_sum.cu q2_mergesort.cu q3_vectoradd_prof.cu

# Dynamically generate the target paths
TARGETS = $(patsubst %.cu, $(BUILD_DIR)/%, $(SRCS))

all: $(TARGETS)

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Pattern rule to compile CUDA files
# The '| $(BUILD_DIR)' ensures the directory is created before compiling
$(BUILD_DIR)/%: %.cu | $(BUILD_DIR)
	$(NVCC) $(NVCC_FLAGS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

run_all: all
	./$(BUILD_DIR)/q1_sum
	./$(BUILD_DIR)/q2_mergesort
	./$(BUILD_DIR)/q3_vectoradd_prof
