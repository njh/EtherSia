docs:
	$(MAKE) -C docs

test:
	$(MAKE) -C tests test

clean:
	$(MAKE) -C docs clean

.PHONY: clean docs
