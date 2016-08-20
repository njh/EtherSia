CFLAGS = -std=c++11 -Wall -Wextra -pedantic
CFLAGS += -I../../src -I../../tests/libarduino

LIBARDUINO_SOURCES=$(wildcard ../../tests/libarduino/*.cpp)
LIBARDUINO_OBJECTS=$(LIBARDUINO_SOURCES:%.cpp=%.o)

LIBETHERSIA_SOURCES=$(wildcard ../../src/*.cpp)
LIBETHERSIA_OBJECTS=$(LIBETHERSIA_SOURCES:%.cpp=%.o)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

LinuxPacketPrinter: LinuxPacketPrinter.o libarduino.a libethersia.a
	$(CXX) -o $@ $< -L. -lethersia -larduino $(CFLAGS)

libarduino.a: $(LIBARDUINO_OBJECTS)
	$(AR) rcs $@ $^

libethersia.a: $(LIBETHERSIA_OBJECTS)
	$(AR) rcs $@ $^

clean:
	rm -f libarduino.a $(LIBARDUINO_OBJECTS)
	rm -f libethersia.a $(LIBETHERSIA_OBJECTS)
	rm -f LinuxPacketPrinter LinuxPacketPrinter.o

.PHONY: clean
