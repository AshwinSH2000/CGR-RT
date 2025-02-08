CC = g++
CXXFLAGS = -std=c++17 -I./json/include
SRC = raytracer.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = raytracer

$(EXEC):
	$(CC) $(CXXFLAGS) $(SRC) -o $(EXEC)

clean:
	rm -f *.o $(EXEC)