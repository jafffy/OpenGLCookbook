CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

instancing: instancing.cpp
	$(CC) instancing.cpp shader.cpp -o instancing $(GLFW_DEP) $(LIB)
