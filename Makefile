COMPILER = gcc
CCFLAGS = -Wall -g

all: search index

module1: search

readIndexModule.o: readIndexModule.c search.h
	$(COMPILER) $(CCFLAGS) -c readIndexModule.c search.h

search.o: search.c search.h
	$(COMPILER) $(CCFLAGS) -c search.c search.h

search: readIndexModule.o search.o
	$(COMPILER) $(CCFLAGS) -o search search.o readIndexModule.o

index.o: index.c index.h tokenizer.h
	$(COMPILER) $(CCFLAGS) -c index.c index.h tokenizer.h

tokenizer.o: tokenizer.c tokenizer.h
	$(COMPILER) $(CCFLAGS) -c tokenizer.c tokenizer.h

index: index.o tokenizer.o 
	$(COMPILER) $(CCFLAGS) -o index index.o tokenizer.o

clean:
	rm -f *.o
	rm -f search
	rm -f index
	rm -f *.h.gch
