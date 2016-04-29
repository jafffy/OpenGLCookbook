CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

transform-sample: transform-sample.cpp sample.cpp
	$(CC) transform-sample.cpp sample.cpp shader.cpp -o transform-sample  $(GLFW_DEP) $(LIB)
