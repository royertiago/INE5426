CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -Werror -ggdb
LIBS = lexertl
ILIBS = $(patsubst %, -isystem %/, $(LIBS) )

SOURCES := $(shell find -name "*.cpp" $(patsubst %,! -wholename ./%/\*, $(LIBS) ) )
OBJ = $(SOURCES:.cpp=.o)
OBJDEPS = $(SOURCES:.cpp=.d)

a.out: $(OBJ)
	$(CXX) $^

$(OBJDEPS): %.d : %.cpp
	g++ -std=c++0x -MM $< -MF $@ -MT "$*.o $*.d" $(ILIBS)

$(OBJ): %.o : %.cpp makefile
	$(CXX) $(CXXFLAGS) $(ILIBS) -c $< -o $@

include $(OBJDEPS)

.PHONY: clean veryclean
clean:
	-rm $(OBJ) $(OBJDEPS)

veryclean: clean
	-rm a.out
