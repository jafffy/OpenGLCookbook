CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

all: transform.cpp
	$(CC) transform.cpp shader.cpp -o transform $(GLFW_DEP) $(LIB)
