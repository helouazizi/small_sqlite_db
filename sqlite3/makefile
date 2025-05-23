CC = gcc
CFLAGS = -Wall -Wextra -std=c99
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
