#!/bin/bash
gcc main.c -o program `pkg-config --cflags --libs cairo x11`
