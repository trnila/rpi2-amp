OBJECTS=start.o \
		main.o \
		log.o \
		mini-printf.o \
		irq.o \
		gpio.o \
		timer.o \
		uart.o \
		runtime.o \
		tools/mailbox.o \
		freertos/croutine.o \
		freertos/queue.o \
		freertos/event_groups.o \
		freertos/list.o \
		freertos/tasks.o \
		freertos/timers.o \
		freertos/portable/GCC/RaspberryPi/port.o \
		freertos/portable/GCC/RaspberryPi/portisr.o \
		freertos/portable/MemMang/heap_1.o


CFLAGS=-I. -I./freertos/include/ -I./freertos/ -I./freertos/portable/GCC/RaspberryPi/ -march=armv7-a \
	   -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
	   -D BAREMETAL=1

all: app.img

%.o: %.S
	gcc -c $^ -o $@ ${CFLAGS}

app.elf: $(OBJECTS)
	ld -nostartfile -T rpi.x $^ -o $@

app.img: app.elf
	objcopy $^ -O binary $@

clean:
	rm -f app.img app.elf
	find -name "*.o" -delete
	make -C tools clean

run: app.img
	# TODO: fix
	rm tools/mailbox.o
	make -C tools
	make -C bootloader
	./tools/firmware_load 0x20000000 ./bootloader/bootloader.img
	./tools/firmware_load 0x20008000 ./app.img
	./tools/devmem2 0x400000bc w 0x20000000 # set address of our first instruction
	./tools/mailbox_send 3 0 0x20008000

reload: app.img
	./tools/mailbox_send 3 0 1
	./tools/firmware_load 0x20008000 ./app.img
	./tools/mailbox_send 3 0 0x20008000
