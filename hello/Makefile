all: app.bin

%.o: %.S
	gcc -c $^ -o $@ ${CFLAGS}

app.elf: start.o
	ld -nostartfile -T basic.ld $^ -o $@

app.bin: app.elf
	objcopy $^ -O binary $@

run: app.bin
	../tools/firmware_load 0x20000000 $^
	../tools/devmem2 0x400000bc w 0x20000000 # set address of our first instruction


clean:
	rm -f *.o app.bin app.elf
