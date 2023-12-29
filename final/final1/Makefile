CC := gcc
CFLAGS := -Wall
TARGET := final1
v := 1

all: $(TARGET)

$(TARGET):f74102022_final1.c
	$(CC) -o $@ $^ -fopenmp

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
