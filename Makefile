CC = gcc
CFLAGS = -Iinclude -I../../extern_h -Wall -Wextra $(shell pkg-config --cflags raylib)
LIBS  = $(shell pkg-config --libs raylib) -Llib -lopengl32 -lgdi32 -lwinmm -lcomdlg32
SRC = main.c 
all:main

main: $(SRC) 
	$(CC) $(CFLAGS) $^ $(LIBS) -o main

clean: 
	rm -rf *.o *.exe 
