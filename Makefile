CC=gcc
CFLAGS=-O3 -Wall -g -ggdb -fomit-frame-pointer
LDFLAGS=-lSDL -lpthread -lrt

EXE = imanes
OBJS= config.o cpu.o instruction_set.o loop.o main.o mapper.o \
      pad.o palette.o parse_file.o ppu.o screen.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	-rm *.o

distclean: clean
	-rm $(EXE)
