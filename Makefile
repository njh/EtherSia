docs:
	$(MAKE) -C docs

test:
	$(MAKE) -C tests test

clean:
	$(MAKE) -C docs clean
	$(MAKE) -C tests clean

.PHONY: clean docs
