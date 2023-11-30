CC := gcc
CFLAGS := -Wall
TARGET := hw4_2
v := 1

all: $(TARGET)

$(TARGET):f74102022_hw4_2.c
	$(CC) -o $@ $^ -lpthread

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
