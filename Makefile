docs:
	$(MAKE) -C docs

test:
	$(MAKE) -C tests test

clean:
	$(MAKE) -C docs clean
	$(MAKE) -C tests clean
	find . -name "*.gcno" -delete
	find . -name "*.gcda" -delete
	rm -Rf coverage.info coverage/

.PHONY: clean docs
