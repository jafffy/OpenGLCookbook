CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

all: glfw-test.cpp
	$(CC) glfw-test.cpp -o glfw-test $(GLFW_DEP) $(LIB)
