CC=g++
OPTIONS= -std=gnu++0x -O3
EJEC=MergedTrie_Example
OBJ= MergedTrie.o mainMergedTrie.o

all: $(EJEC)

$(EJEC):  $(OBJ)
	$(CC) $(OPTIONS) $(OBJ) -o $(EJEC)

mainMergedTrie.o: mainMergedTrie.cpp MergedTrie.h MergedTrie_settings.h
	$(CC) $(OPTIONS) -c mainMergedTrie.cpp -o mainMergedTrie.o

MergedTrie.o: MergedTrie.cpp MergedTrie.h MergedTrie_settings.h
	$(CC) $(OPTIONS) -c MergedTrie.cpp -o MergedTrie.o
