CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Iinclude
DEBUG_FLAGS = -g -O0
LDFLAGS = -lreadline
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = indus
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)

uninstall:
	rm -f $(BINDIR)/$(TARGET)

.PHONY: all clean install uninstall

