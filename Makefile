
all:
	gcc -o tropism main.c -lGL -lGLU -lglut -lm
	chmod 755 tropism
