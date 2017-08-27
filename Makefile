CXX = g++
CXXFLAGS = -std=c++11 -O3
CXXFLAGSDEBUG = -std=c++11 -O0 -g3

all:
	$(CXX) $(CXXFLAGS) *.cpp -o analyser

debug:
	$(CXX) $(CXXFLAGSDEBUG) *.cpp -o analyser

clean:
	rm -f *.o analyser
