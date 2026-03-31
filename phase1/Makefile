CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
OPTFLAGS = -O3

TARGET = benchmark
TEST_TARGET = tests
OBJS = benchmark.o matrix_operations.o
TEST_OBJS = tests.o matrix_operations.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $(TARGET) $(OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

benchmark.o: benchmark.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c benchmark.cpp

tests.o: tests.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c tests.cpp

matrix_operations.o: matrix_operations.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c matrix_operations.cpp

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJS) $(TEST_OBJS)

.PHONY: all test clean
