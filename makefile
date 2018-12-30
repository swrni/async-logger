COMPILER = g++
FLAGS = -g -Wall -std=c++17
SOURCES = main.cpp queue_test.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = async_logger.exe
TESTABLES = queue_test
REBUILDABLES = $(OBJECTS) $(LINK_TARGET)

all: $(EXECUTABLE)

test:
	$(COMPILER) $(FLAGS) -c $(TESTABLES).cpp
	$(COMPILER) -o $(TESTABLES).exe $(TESTABLES).o
	powershell.exe -Command "./$(TESTABLES).exe"

$(EXECUTABLE): $(OBJECTS)
	$(COMPILER) $(FLAGS) $@ $<

$(OBJECTS): $(SOURCES)
	$(COMPILER) $(FLAGS) -c $<






















# $@ expands to the target, here: "async_logger.exe".
# $^ expands to the dependencies, here: "Queue.o" and "main.o".
# -g generate addictional symbolic debugging information.
# $(LINK_TARGET): $(OBJECTS)
# 	g++.exe -std=c++17 -Wall -g -o $@ $^

# Use "*.o" files as targets, and "*.cpp" as dependency.
# $@ expands to the target, here: "*.o".
# $< expands to the dependency, here: "*.cpp".
# %.o: %.cpp
# 	g++.exe -g -o $@ -c $<

# These are passed to the previous rule.
# main.o: main.cpp Queue.hpp
# Queue.o: Queue.hpp