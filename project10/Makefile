CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3

TARGET = JackAnalyzer

SRCS = main.cpp JackTokenizer.cpp CompilationEngine.cpp
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
