CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm
TARGET = localstar
SRC = visible.c catalog.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
