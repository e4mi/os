#!/bin/sh
nasm --reproducible -o os.bin os.nasm && ndisasm os.bin | grep -v "  0000 "