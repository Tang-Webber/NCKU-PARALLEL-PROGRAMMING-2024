CC := g++
CFLAGS := -Wall
TARGET := hw5_1
v := 1

all: $(TARGET)

$(TARGET):hw5_1.cpp
	$(CC) -o $@ $^ -fopenmp

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
