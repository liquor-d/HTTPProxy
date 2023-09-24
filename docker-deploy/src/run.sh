#!/bin/bash
make clean

make

echo 'Running HTTP proxy server......'

./main

while true
do
    sleep 1
done

