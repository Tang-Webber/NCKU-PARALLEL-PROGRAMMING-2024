CC := mpicc
CFLAGS := -Wall
TARGET := mid4
v := 1

all: $(TARGET)

$(TARGET):f74102022_mid4.c
	$(CC) -o $@ $^

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
