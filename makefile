CC = gcc
CFLAGS = -Wall -Wextra -I.
DEBUG_FLAGS = -g -O0
LDFLAGS = -lreadline
SRCS = src.c prompt/prompt.c utils/utils.c builtins/builtins.c
OBJS = $(SRCS:.c=.o)
TARGET = indus
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)

uninstall:
	rm -f $(BINDIR)/$(TARGET)

.PHONY: all clean install uninstall
