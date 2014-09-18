CXX := /usr/lib/gcc-snapshot/bin/g++
CXXFLAGS := -std=c++1y -Wall -Wextra -pedantic -Werror -ggdb
LIBS := lexertl
ILIBS := $(patsubst %, -isystem %/, $(LIBS) )

SOURCES := $(shell find -name "*.cpp" $(patsubst %,! -wholename ./%/\*, $(LIBS) ) )
OBJ := $(SOURCES:.cpp=.o)
OBJDEPS := $(SOURCES:.cpp=.d)

a.out: $(OBJ)
	$(CXX) $^

$(OBJ): %.o : %.cpp makefile
	$(CXX) $(CXXFLAGS) $(ILIBS) -c $< -o $@
	g++ -std=c++0x -MM $< -MF $*.d -MT "$*.o" $(ILIBS)
	sed -e 's/^.*://' -e 's/\\//' -e 's/ /\n/g' $*.d | sed -e 's/\(..*\)/\1:/' >> $*.d

-include $(OBJDEPS)

.PHONY: clean veryclean
clean:
	-find \( -name "*.o" -or -name "*.d" \) -exec rm '{}' \;

veryclean: clean
	-rm a.out
