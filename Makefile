CC=gcc
CFLAGS=-O3 -Wall -g
LDFLAGS=-lSDL

EXE = tobyNes
OBJS= main.o parse_file.o screen.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	-rm *.o

distclean: clean
	rm $(EXE)
