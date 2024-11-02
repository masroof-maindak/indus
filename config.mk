CC = gcc

CFLAGS = -Wall -Wextra -pedantic
RELEASE_FLAGS = -march=native -O3
DEBUG_FLAGS = -g -O0
LDFLAGS = -lreadline

SRCDIR = src
BINDIR = bin
BUILDDIR = build

TARGET = indus
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

SRCS = $(wildcard ${SRCDIR}/*.c)
OBJS = ${SRCS:${SRCDIR}/%.c=${BUILDDIR}/%.o}
