CC = gcc
CFLAGS = -std=gnu23 -O3 -Wall -Wextra -Wpedantic -Werror -march=native
TARGET = eras
SRC = main.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
