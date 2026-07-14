CC = gcc
CFLAGS = 
LDFLAGS = -lgsl -lgslcblas -lm
SRC = main.c catalog.c astro_math.c locate.c



locate: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o locate $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
