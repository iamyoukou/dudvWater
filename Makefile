CXX=g++-11
INCS=-c -std=c++17 \
-I/usr/local/Cellar/glew/2.2.0_1/include \
-I/usr/local/Cellar/glfw/3.3.6/include \
-I/usr/local/Cellar/freeimage/3.18.0/include \
-I/usr/local/Cellar/glm/0.9.9.8/include \
-I/usr/local/Cellar/assimp/5.1.2/include \
-I/Users/YJ-work/cpp/myGL_glfw/dudvWater/header

LIBS=-L/usr/local/Cellar/glew/2.2.0_1/lib -lglfw \
-L/usr/local/Cellar/glfw/3.3.6/lib -lGLEW \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-L/usr/local/Cellar/assimp/5.1.2/lib -lassimp \
-framework GLUT -framework OpenGL -framework Cocoa

SRC_DIR=/Users/YJ-work/cpp/myGL_glfw/dudvWater/src

all: main normal2dudv

main: main.o common.o skybox.o water.o
	$(CXX) $(LIBS) $^ -o $@

main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(INCS) $^ -o $@

normal2dudv: normal2dudv.o common.o
	$(CXX) $(LIBS) $^ -o $@

normal2dudv.o: $(SRC_DIR)/normal2dudv.cpp
	$(CXX) $(INCS) $^ -o $@

common.o: $(SRC_DIR)/common.cpp
	$(CXX) $(INCS) $^ -o $@

skybox.o: $(SRC_DIR)/skybox.cpp
	$(CXX) $(INCS) $^ -o $@

water.o: $(SRC_DIR)/water.cpp
	$(CXX) $(INCS) $^ -o $@


.PHONY: cleanImg cleanObj

cleanImg:
	rm -vf ./result/*

cleanObj:
	rm -vf *.o
