#!/bin/bash

cd  build || exit
cmake ..
make || exit
./TestBed/DimensionalApp


