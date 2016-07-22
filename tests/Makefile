CFLAGS = -std=c++11 -Wall -Werror -Wextra -pedantic
CFLAGS += -I../src -I./libarduino

CHECK_CFLAGS = $(shell pkg-config --cflags check)
CHECK_LIBS = $(shell pkg-config --libs check)

check: check_mac_address.cmd check_util.cmd
	./check_mac_address.cmd
	./check_util.cmd

test: check

%.cpp: %.tc
	checkmk $< > $@ || rm -f $@

check_mac_address.cmd: check_mac_address.cpp
	c++ $(CFLAGS) \
     -o $@ $< \
     $(CHECK_LIBS) \
     libarduino/Print.cpp \
     ../src/IPv6Address.cpp \
     ../src/MACAddress.cpp \
     ../src/util.cpp

check_util.cmd: check_util.cpp
	c++ $(CFLAGS) \
     -o $@ $< \
     $(CHECK_LIBS) \
     libarduino/Print.cpp \
     ../src/util.cpp

clean:
	rm -f check_mac_address.cmd check_mac_address.cpp check_util.cmd check_util.cpp

.PHONY: test check clean
