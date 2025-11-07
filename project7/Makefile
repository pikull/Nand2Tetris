CXX = g++
CXXFLAGS = -std=c++17 -Wall

TARGET = VMTranslator

SRCS = main.cpp Parser.cpp CodeWriter.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(CXXFLAGS) += -03
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
	chmod +x $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
