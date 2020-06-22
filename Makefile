CXX=llvm-g++
INCS=-c -std=c++17 \
-I/usr/local/Cellar/glew/2.1.0_1/include \
-I/usr/local/Cellar/glfw/3.3.2/include \
-I/usr/local/Cellar/freeimage/3.18.0/include \
-I/usr/local/Cellar/glm/0.9.9.8/include \
-I/Users/YJ-work/cpp/myGL_glfw/dudvWater/header

LIBS=-L/usr/local/Cellar/glew/2.1.0_1/lib -lglfw \
-L/usr/local/Cellar/glfw/3.3.2/lib -lGLEW \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-framework GLUT -framework OpenGL -framework Cocoa

SRC_DIR=/Users/YJ-work/cpp/myGL_glfw/dudvWater/src

all: main

main: main.o common.o skybox.o
	$(CXX) $(LIBS) $^ -o $@
	rm -f *.o

main.o: $(SRC_DIR)/main.cpp
	$(CXX) -c $(INCS) $^ -o $@

common.o: $(SRC_DIR)/common.cpp
	$(CXX) -c $(INCS) $^ -o $@

skybox.o: $(SRC_DIR)/skybox.cpp
	$(CXX) -c $(INCS) $^ -o $@

.PHONY: clean

clean:
	rm -vf main ./result/*
