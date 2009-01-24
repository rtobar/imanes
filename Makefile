CC=gcc
CFLAGS=-O3 -Wall -g
LDFLAGS=-lSDL -lpthread

EXE = tobyNes
OBJS= cpu.o instruction_set.o loop.o main.o mapper.o palette.o parse_file.o ppu.o screen.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	-rm *.o

distclean: clean
	rm $(EXE)
