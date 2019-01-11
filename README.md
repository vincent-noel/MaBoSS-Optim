# MaBoSS-Optim [![Build Status](https://travis-ci.org/vincent-noel/MaBoSS-Optim.svg?branch=master)](https://travis-ci.org/vincent-noel/MaBoSS-Optim)
Optimization of MaBoSS models

## Dependencies
* g++
* flex
* bison
* JsonCpp

## Example
    ./MaBoSS-Optim -c examples/Four_cycle.cfg -s examples/settings.json -o result.json -r simulation_values examples/Four_cycle.bnd
