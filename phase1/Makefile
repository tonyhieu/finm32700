CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
OPTFLAGS = -O3

TARGET = benchmark
TEST_TARGET = tests
CACHE_TARGET = benchmark_cache
OPT_TARGET = benchmark_optimization

OBJS = benchmark.o matrix_operations.o
TEST_OBJS = tests.o matrix_operations.o
CACHE_OBJS = benchmark_cache.o matrix_operations.o
OPT_OBJS = benchmark_optimization.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $(TARGET) $(OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

$(CACHE_TARGET): $(CACHE_OBJS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $(CACHE_TARGET) $(CACHE_OBJS)

benchmark.o: benchmark.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c benchmark.cpp

tests.o: tests.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c tests.cpp

benchmark_cache.o: benchmark_cache.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c benchmark_cache.cpp

benchmark_optimization.o: benchmark_optimization.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c benchmark_optimization.cpp

matrix_operations.o: matrix_operations.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c matrix_operations.cpp

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Build and run cache locality benchmarks
run_cache: $(CACHE_TARGET)
	./$(CACHE_TARGET)

# Build and run optimization comparison benchmarks (compile at both -O0 and -O3)
run_optimization:
	@echo "=== Compiling with -O0 (no optimization) ==="
	$(CXX) $(CXXFLAGS) -O0 -c matrix_operations.cpp -o matrix_operations_O0.o
	$(CXX) $(CXXFLAGS) -O0 -c benchmark_optimization.cpp -o benchmark_optimization_O0.o
	$(CXX) $(CXXFLAGS) -O0 -o benchmark_O0 benchmark_optimization_O0.o matrix_operations_O0.o
	@echo ""
	@echo "=== Running with -O0 ==="
	./benchmark_O0
	@echo ""
	@echo "=== Compiling with -O3 (aggressive optimization) ==="
	$(CXX) $(CXXFLAGS) -O3 -c matrix_operations.cpp -o matrix_operations_O3.o
	$(CXX) $(CXXFLAGS) -O3 -c benchmark_optimization.cpp -o benchmark_optimization_O3.o
	$(CXX) $(CXXFLAGS) -O3 -o benchmark_O3 benchmark_optimization_O3.o matrix_operations_O3.o
	@echo ""
	@echo "=== Running with -O3 ==="
	./benchmark_O3
	@echo ""
	@rm -f benchmark_O0 benchmark_O3 matrix_operations_O0.o matrix_operations_O3.o benchmark_optimization_O0.o benchmark_optimization_O3.o

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(CACHE_TARGET) $(OPT_TARGET)
	rm -f $(OBJS) $(TEST_OBJS) $(CACHE_OBJS) $(OPT_OBJS)
	rm -f benchmark_O0 benchmark_O3 matrix_operations_O0.o matrix_operations_O3.o benchmark_optimization_O0.o benchmark_optimization_O3.o

.PHONY: all test clean run_cache run_optimization
