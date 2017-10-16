CXX = g++
CXXFLAGS = -std=c++11 -O3
CXXFLAGSDEBUG = -std=c++11 -O0 -g3
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

compiler: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -c -o $@

debug:
	$(CXX) $(CXXFLAGSDEBUG) *.cpp -o compiler

clean:
	rm -f *.o compiler
