CC := mpicc
CFLAGS := -Wall
TARGET := mid5
v := 1

all: $(TARGET)

$(TARGET):f74102022_mid5.c
	$(CC) -o $@ $^ -lm

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
