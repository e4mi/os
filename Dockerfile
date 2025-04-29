FROM alpine:3
RUN apk add nasm qemu-system-i386
RUN wget https://justine.lol/emulator.com -O /usr/local/bin/blinkenlights
RUN chmod +x /usr/local/bin/blinkenlights