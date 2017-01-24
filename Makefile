docs:
	$(MAKE) -C docs

examples:
	$(MAKE) -C examples

test:
	$(MAKE) -C tests test

coverage: coverage-html

coverage.info:
	lcov --zerocounters --directory src
	$(MAKE) -C tests COVERAGE_CFLAGS="-fprofile-arcs -ftest-coverage" test
	lcov --capture --directory src --output-file coverage.info --no-external
	lcov --remove coverage.info --output-file coverage.info `pwd`/src/libarduino/*

coverage-html: coverage.info
	mkdir -p coverage
	genhtml -t "EtherSia Coverage Report" --output-directory coverage $<

clean:
	$(MAKE) -C docs clean
	$(MAKE) -C tests clean
	find . -name "*.gcno" -delete
	find . -name "*.gcda" -delete
	rm -Rf coverage.info coverage/

.PHONY: docs examples test coverage coverage-html clean
