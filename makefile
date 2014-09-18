CXX := /usr/lib/gcc-snapshot/bin/g++
CXXFLAGS := -std=c++1y -Wall -Wextra -pedantic -Werror -ggdb

# Library definitions
# ILIBS is the compiler-flags-version of LIBS
LIBS := lexertl Catch Catch/single_include
ILIBS := $(patsubst %, -isystem %/, $(LIBS) ) -I./

# Variable definitions
MAIN	:= main.cpp
SOURCES := $(shell find -name "*.cpp" $(patsubst %,! -wholename ./%/\*, $(LIBS) ) )
DEPS	:= $(SOURCES:.cpp=.d)
TSOURCES:= $(filter		%.test.cpp, $(SOURCES))
MSOURCES:= $(filter-out %.test.cpp, $(SOURCES))
MOBJ	:= $(MSOURCES:.cpp=.o)
TOBJ	:= $(TSOURCES:.cpp=.o)
OBJ		:= $(filter-out %$(MAIN:.cpp=.o), $(MOBJ))
# TSOURCES: test sources, MSOURCES: main sources,
# TOBJ: test objects, MOBJ: main objects.
# OBJ contains all main objects, but without main.o, that defines "int main()".

all: a.out test/test

a.out: $(MOBJ)
	$(CXX) $^

test: test/test
	test/test

test/test: $(OBJ) $(TOBJ)
	$(CXX) $^ -o test/test


$(MOBJ) $(TOBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) $(ILIBS) -c $< -o $@
	g++ -std=c++0x -MM $< -MF $*.d -MT "$*.o" $(ILIBS)
	sed -e 's/^.*://' -e 's/\\//' -e 's/ /\n/g' $*.d | sed -e 's/\(..*\)/\1:/' >> $*.d

-include $(DEPS)

.PHONY: clean veryclean test
clean:
	-find \( -name "*.o" -or -name "*.d" \) -exec rm '{}' \;

veryclean: clean
	-rm a.out
