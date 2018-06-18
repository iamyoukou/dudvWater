INCS=-I../include
LIBS=-L../lib -lfreeimage -lGLEW -lglfw \
 -framework GLUT -framework OpenGL -framework Cocoa

SRC=../src

all: app

app: main.o common.o
	g++ $(LIBS) main.o common.o -o app
	rm -f *.o

main.o: $(SRC)/main.cpp
	g++ -c $(INCS) $(SRC)/main.cpp -o main.o

common.o: $(SRC)/common.cpp
	g++ -c $(INCS) $(SRC)/common.cpp -o common.o

.PHONY: clean

clean:
	rm -vf app
