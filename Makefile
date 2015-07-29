
LIBS = -lGL -lGLU -lglut -lm `pkg-config --cflags --libs guile-2.0`

all:
	gcc -o tropism src/main.c $(LIBS)
	chmod 755 tropism
