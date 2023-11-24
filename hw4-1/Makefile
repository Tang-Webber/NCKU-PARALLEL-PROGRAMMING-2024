CC := gcc
CFLAGS := -Wall
TARGET := hw4_1
v := 1

all: $(TARGET)

$(TARGET):hw4_1.c
	$(CC) -o $@ $^ -lpthread

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
