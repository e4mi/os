deps:
  nasm
  qemu-system-i386 (for emulation)
  blinkenlights (for debugging)

optionally use docker:
  docker build -t os . && docker run --rm -itv./:/w -w/w os

usage:
  ./mk   # builds ./os.bin
  ./emu  # ctrl+a,x to exit
  ./dbg  # ctrl+c to exit
