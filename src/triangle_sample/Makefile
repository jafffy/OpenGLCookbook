CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

triangle-sample: triangle-sample.cpp sample.cpp
	$(CC) triangle-sample.cpp sample.cpp -o triangle-sample  $(GLFW_DEP) $(LIB)
