CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

instancing-sample: instancing-sample.cpp sample.cpp
	$(CC) instancing-sample.cpp sample.cpp shader.cpp -o instancing-sample  $(GLFW_DEP) $(LIB)
