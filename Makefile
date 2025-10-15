CC = gcc
CFLAGS = -Wall -pedantic-errors -std=c99

TARGET = p1_mabdurr2_202

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)