#!/bin/sh
#
# Create an HTML Coverage Report using lcov
#

if ! which lcov &>/dev/null; then
    echo "lcov is not installed."
    exit -1
fi

if ! which genhtml &>/dev/null; then
    echo "genhtml is not installed."
    exit -1
fi


cd `dirname $0`

if [ ! -d coverage ]; then
    mkdir coverage
fi

if [ -e Makefile ]; then
    make clean
fi

lcov --directory src --zerocounters

export CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"

if ! make test; then
    echo "Some tests failed; aborting."
    exit -1
fi

lcov --directory src --capture --output-file coverage.info
genhtml -t "EtherSia Coverage Report" --output-directory coverage coverage.info
