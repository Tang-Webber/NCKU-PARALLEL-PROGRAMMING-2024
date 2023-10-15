CC := mpic++
CFLAGS := -Wall
TARGET := hw2_2
v := 1

all: $(TARGET)

$(TARGET):F74102022_hw2_2.cpp
	$(CC) -o $@ $^

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
