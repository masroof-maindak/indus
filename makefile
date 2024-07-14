all:
	cc -std=c99 -Wall src.c -o indus -lreadline && ./indus
