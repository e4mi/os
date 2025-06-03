FROM alpine:3
RUN apk add make nasm qemu-system-i386
RUN wget http://justine.lol/emulator.com -O /bin/emulator
RUN chmod +x /bin/emulator