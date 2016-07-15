CFLAGS = -std=c++11 -Wall -Werror -Wextra -pedantic

check: check_util
	./check_util

test: check

%.cpp: %.tc
	checkmk $< > $@ || rm -f $@

check_util: check_util.cpp
	c++ $(CFLAGS) \
	   -I../src \
	   -I./libarduino \
	   -o check_util \
     -lcheck \
     check_util.cpp \
     libarduino/print.cpp \
     ../src/util.cpp

clean:
	rm -f check_util check_util.cpp

.PHONY: test check clean
