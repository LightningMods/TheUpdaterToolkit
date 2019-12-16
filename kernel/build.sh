#!/bin/bash

build_kernel_payload() {
    cd kernel_payload
    export KERNEL_VER=505
    make clean
    make
    export KERNEL_VER=474
    make clean
    make
    cd ..
}

echo "building kernel payload ..."

echo "=> kernel payload ..."
build_kernel_payload

echo ""
echo "Done."
