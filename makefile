FLAGS= -Wall -Wextra -ggdb

test: tests/test.h tests/test.c
	cc -o build/hello  ${FLAGS} tests/scenarios/hello/hello.c
	cc -o build/echo  ${FLAGS} tests/scenarios/echo/echo.c
	cc -o build/echo2  ${FLAGS} tests/scenarios/echo/echo2.c
	cc -o build/error  ${FLAGS} tests/scenarios/error/error.c
	cc -o build/test  ${FLAGS} tests/test.c
	build/test
