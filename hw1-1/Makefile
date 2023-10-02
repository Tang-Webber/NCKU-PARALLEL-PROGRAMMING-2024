CC := mpicc
CFLAGS := -Wall
TARGET := hw1_1
v := 1

all: $(TARGET)

$(TARGET):F74102022_hw1_1.c
	$(CC) -o $@ $^

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
