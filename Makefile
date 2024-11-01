CXX = g++
CXXFLAGS = -std=c++11 -pthread

OBJS = jogo_da_velha.o main.o
TARGET = jogo_da_velha

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lpthread

jogo_da_velha.o: jogo_da_velha.cpp jogo_da_velha.h
	$(CXX) $(CXXFLAGS) -c jogo_da_velha.cpp

main.o: main.cpp jogo_da_velha.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f $(OBJS) $(TARGET)
