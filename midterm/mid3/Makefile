CC := mpicc
CFLAGS := -Wall
TARGET := mid3
v := 1

all: $(TARGET)

$(TARGET):f74102022_mid3.c
	$(CC) -o $@ $^

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
