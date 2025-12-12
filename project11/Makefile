CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3

TARGET = JackCompiler

SRCS = main.cpp JackTokenizer.cpp CompilationEngine.cpp SymbolTable.cpp VMWriter.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
	chmod +x $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
