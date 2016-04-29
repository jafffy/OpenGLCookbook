CC=g++ -std=c++11
LIB=-lglew 
GLFW_DEP= `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` -framework OpenGL

sample-test: sample-test.cpp sample.cpp
	$(CC) sample-test.cpp sample.cpp -o hello  $(GLFW_DEP) $(LIB)
