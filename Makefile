CC=gcc
CFLAGS=-Wall -g
TARGET=miris

all: $(TARGET)

$(TARGET): main.o dir_multi_graph.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o dir_multi_graph.o

main.o: main.c dir_multi_graph.h
	$(CC) $(CFLAGS) -c main.c

dir_multi_graph.o: dir_multi_graph.c dir_multi_graph.h
	$(CC) $(CFLAGS) -c dir_multi_graph.c


run:  
	./$(TARGET) `input.txt` `output.txt`

# Run the program with valgrind to check for memory leaks
leaks:
	valgrind --leak-check=full ./$(TARGET) -i `input.txt` -o `output.txt`

clean:
	rm -f *.o $(TARGET)
