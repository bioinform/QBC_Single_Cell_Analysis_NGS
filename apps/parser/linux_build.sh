#!/usr/bin/env bash
set -ex

curl -LO https://cmake.org/files/v3.9/cmake-3.9.4-Linux-x86_64.tar.gz
tar xzf cmake-3.9.4-Linux-x86_64.tar.gz

export PATH=`readlink -e cmake-3.9.4-Linux-x86_64/bin`:${PATH}

rm -rf build
mkdir build
pushd build
cmake ../cpp
make -j
popd
