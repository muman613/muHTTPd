#!/bin/bash
##############################################################################
#   Build the project
##############################################################################

# Build the library
pushd libmuHTTPd
make clean
make -j 4
popd
# Build the demo
pushd muHTTPdemo
make clean
make -j 4
popd

