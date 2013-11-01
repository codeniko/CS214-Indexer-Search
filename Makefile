COMPILER = gcc
CCFLAGS = -Wall -g

all: search

module1: search

readIndexModule.o: readIndexModule.c search.h
	$(COMPILER) $(CCFLAGS) -c readIndexModule.c search.h

search.o: search.c search.h
	$(COMPILER) $(CCFLAGS) -c search.c search.h

search: readIndexModule.o search.o
	$(COMPILER) $(CCFLAGS) -o search search.o readIndexModule.o

clean:
	rm -f *.o
	rm -f search
