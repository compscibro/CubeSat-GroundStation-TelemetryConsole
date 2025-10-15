CC = gcc
CFLAGS = -Wall -pedantic-errors -std=c99

TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)