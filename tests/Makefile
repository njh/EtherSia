CFLAGS = -std=c++11 -Wall -Wextra -pedantic
CFLAGS += -I../src -I./libarduino

# FIXME: work out how to apply this to the clang compiler only
# CFLAGS += -Werror -Wno-gnu-zero-variadic-macro-arguments -Wno-constant-conversion -Wno-tautological-constant-out-of-range-compare

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
	$(CXX) $(CHECK_LIBS) -o $@ $^


clean:
	rm -f libarduino.a $(LIBARDUINO_OBJECTS)
	rm -f libethersia.a $(LIBETHERSIA_OBJECTS)
	rm -f $(TEST_SOURCES) $(TEST_COMMANDS)

.PHONY: test check clean
