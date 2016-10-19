CFLAGS += -std=c++11 -Werror -Wall -Wextra -pedantic -g -O0
CFLAGS += -I../src -I./libarduino
CFLAGS += $(COVERAGE_CFLAGS)

# FIXME: work out how to apply this to the clang compiler only
CFLAGS += -Wno-gnu-zero-variadic-macro-arguments


CHECK_CFLAGS = $(shell pkg-config --cflags check)
CHECK_LIBS = $(shell pkg-config --libs check)

LIBARDUINO_SOURCES=$(wildcard libarduino/*.cpp)
LIBARDUINO_OBJECTS=$(LIBARDUINO_SOURCES:%.cpp=%.o)

LIBETHERSIA_SOURCES=$(wildcard ../src/*.cpp)
LIBETHERSIA_OBJECTS=$(LIBETHERSIA_SOURCES:%.cpp=%.o)

TEST_SUITES=$(wildcard *.tc)
TEST_SOURCES=$(TEST_SUITES:%.tc=%.cpp)
TEST_COMMANDS=$(TEST_SUITES:%.tc=%.cmd)



test: check

check: $(TEST_COMMANDS)
	@$(foreach f,$^,./$(f) &&) echo "All tests passed."

%.o: %.cpp
	$(CXX) $(CFLAGS) $(CHECK_CFLAGS) -c -o $@ $<

libarduino.a: $(LIBARDUINO_OBJECTS)
	$(AR) rcs $@ $^

libethersia.a: $(LIBETHERSIA_OBJECTS)
	$(AR) rcs $@ $^

%.cpp: %.tc
	checkmk $< > $@ || rm -f $@

%.cmd: %.o libarduino.a libethersia.a
	$(CXX) -o $@ $< -L. -lethersia -larduino $(CHECK_LIBS) $(CFLAGS)


clean:
	rm -f libarduino.a $(LIBARDUINO_OBJECTS)
	rm -f libethersia.a $(LIBETHERSIA_OBJECTS)
	rm -f $(TEST_SOURCES) $(TEST_COMMANDS)

.PHONY: test check clean
