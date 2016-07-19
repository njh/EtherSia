CFLAGS = -std=c++11 -Wall -Werror -Wextra -pedantic

CHECK_CFLAGS=$(shell pkg-config --cflags check)
CHECK_LIBS=$(shell pkg-config --libs check)

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
	   $(CHECK_LIBS) \
     check_util.cpp \
     libarduino/Print.cpp \
     ../src/util.cpp

clean:
	rm -f check_util check_util.cpp

.PHONY: test check clean
