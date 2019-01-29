COMPILER = g++
FLAGS = -g3 -Wall -Werror -Wextra -Wpedantic -std=c++17
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = async_logger.exe

$(EXECUTABLE):
	$(COMPILER) $(FLAGS) -c main.cpp
	$(COMPILER) -o $(EXECUTABLE) $(OBJECTS)
	powershell.exe -Command "./$(EXECUTABLE)"

# $(COMPILER) -o $(EXECUTABLE) main.o