CC=gcc
CFLAGS=-O3 -Wall -g
LDFLAGS=-lSDL -lpthread

EXE = tobyNes
OBJS= cpu.o instruction_set.o main.o mapper.o parse_file.o screen.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	-rm *.o

distclean: clean
	rm $(EXE)
