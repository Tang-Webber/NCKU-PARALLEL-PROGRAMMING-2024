CC := gcc
CFLAGS := -Wall
TARGET := hw5_2
v := 1

all: $(TARGET)

$(TARGET):f74102022_hw5_2.c
	$(CC) -o $@ $^ -fopenmp

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
