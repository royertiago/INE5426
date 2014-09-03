CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -pedantic -Wextra -ggdb
LIBS = lexertl
ILIBS = $(patsubst %, -I%/, $(LIBS) )

SOURCES := $(shell find -name "*.cpp" $(patsubst %,! -wholename ./%/\*, $(LIBS) ) )
OBJ = $(SOURCES:.cpp=.o)
OBJDEPS = $(SOURCES:.cpp=.d)

a.out: $(OBJ)
	$(CXX) $^

$(OBJDEPS): %.d : %.cpp
	g++ -std=c++0x -MM $< -MF $@ -MT "$*.o $*.d" $(LIBS)

$(OBJ): %.o : %.cpp makefile
	$(CXX) $(CXXFLAGS) $(ILIBS) -c $< -o $@

include $(OBJDEPS)

.PHONY: clean
clean:
	-rm $(OBJ) $(OBJDEPS)
