#!/bin/bash

set -xe

cc -Wall -Wextra gol3d.c -lraylib -lm -o gol3d
