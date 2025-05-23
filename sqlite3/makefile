CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra 
TARGET = sqlite
OBJS = main.o sqlite.o




all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c sqlite.h
	$(CC) $(CFLAGS) -c main.c

sqlite.o: sqlite.c sqlite.h
	$(CC) $(CFLAGS) -c sqlite.c

clean:
	rm -f $(TARGET) *.o

run: all
	./$(TARGET)
