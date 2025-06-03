FROM alpine:3
RUN apk add --no-cache make nasm qemu-system-i386
RUN wget http://justine.lol/emulator.com -O /bin/blinkenlights
RUN chmod +x /bin/blinkenlights