CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
OPTFLAGS = -O3

TARGET = benchmark
OBJS = benchmark.o matrix_operations.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $(TARGET) $(OBJS)

benchmark.o: benchmark.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c benchmark.cpp

matrix_operations.o: matrix_operations.cpp matrix_operations.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c matrix_operations.cpp

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
