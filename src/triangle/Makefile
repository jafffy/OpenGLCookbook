CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

triangle: triangle.cpp
	$(CC) triangle.cpp shader.cpp -o triangle $(GLFW_DEP) $(LIB)
