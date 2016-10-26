EXAMPLES := $(wildcard */*.ino)
ARDUINO := arduino
ARDUINO_FLAGS := --verify --board arduino:avr:uno

all: $(EXAMPLES)

$(EXAMPLES):
	$(ARDUINO) $(ARDUINO_FLAGS) $@
	@echo

.PHONY: all $(EXAMPLES)
