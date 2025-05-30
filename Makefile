CXX = g++
CXXFLAGS = -Wall -g
TESTFLAGS = -lgtest -lgmock -pthread

TESTSRC = test/skip_list_test.cpp
TESTTARGET = test-skip-list

all:

test: $(TESTSRC)
	$(CXX) $(TESTSRC) $(CXXFLAGS) -o $(TESTTARGET) $(TESTFLAGS)

clean:
	rm -f $(TESTTARGET)

.PHONY: all test clean
