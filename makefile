
TARGET := machine
SRCS := machine.cpp
CFLAGS := -O -g -Wall

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f -- $(TARGET)
