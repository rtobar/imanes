CC=gcc
CFLAGS=-O3 -Wall -g -ggdb -fomit-frame-pointer
LDFLAGS=-lSDL -lpthread -lrt

EXE = imanes
OBJS= config.o cnrom.o cpu.o instruction_set.o loop.o main.o mapper.o \
      nrom.o pad.o palette.o parse_file.o ppu.o screen.o unrom.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	-rm -f *.o

distclean: clean
	-rm -f $(EXE)
