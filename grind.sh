#!/bin/bash
valgrind --leak-check=full -q $1
