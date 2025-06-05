CXX = g++
CXXFLAGS = -Wall -g
TESTFLAGS = -lgtest -lgmock -pthread

TARGET = skiplist
OBJ = src/main.o

TESTSRC = test/skip_list_test.cpp
TESTTARGET = test-skip-list

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

all: $(TARGET)

test: $(TESTSRC)
	$(CXX) $(TESTSRC) $(CXXFLAGS) -o $(TESTTARGET) $(TESTFLAGS)

clean:
	rm -f $(TESTTARGET)
	rm -f $(TARGET)
	rm -f $(OBJ)

.PHONY: all test clean
