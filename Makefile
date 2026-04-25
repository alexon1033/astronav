CC = gcc
CFLAGS =
LDFLAGS = -lm
TARGET = starlocate
SRC = main.c catalog.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
