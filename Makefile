CXX = g++
CXXFLAGS = -std=c++11 -O3

all:
	$(CXX) $(CXXFLAGS) *.cpp -o lexical_analyser

clean:
	rm -f *.o lexical_analyser