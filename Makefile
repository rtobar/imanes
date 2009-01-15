CC=gcc
CFLAGS=-O3 -Wall

EXE = tobyNes
OBJS= main.o parse_file.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	-rm *.o

distclean: clean
	rm $(EXE)
