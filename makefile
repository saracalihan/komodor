FLAGS= -Wall -Wextra -ggdb

all: compile run
compile: src/main.c
	cc -o build/main  ${FLAGS} src/main.c
run: build/main
	./build/main
