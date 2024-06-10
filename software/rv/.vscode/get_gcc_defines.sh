#!/bin/bash

riscv64-unknown-elf-gcc -mabi=ilp32 -march=rv32imc -E -dM - < /dev/null | sort | sed -e 's/"/\\"/g' | sed -r -e 's/#define ([^ ]+).([^\n]+)/"\1=\2",/g'
