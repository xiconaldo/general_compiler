CXX = g++
CXXFLAGS = -std=c++11 -O3
CXXFLAGSDEBUG = -std=c++11 -O0 -g3
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.c=.o)

all: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o analyser

compile: $(SRC)
	$(CXX) $(CXXFLAGS) $^ -c

debug:
	$(CXX) $(CXXFLAGSDEBUG) *.cpp -o analyser

clean:
	rm -f *.o analyser
