OBJECTS=start.o main.o log.o mini-printf.o irq.o

all: app.img

%.o: %.S
	gcc -c $^ -o $@


%.o: %.c
	gcc -c $^ -o $@

app.elf: $(OBJECTS)
	ld -Ttext 0x20000000 -nostartfile -T rpi.x $^ -o $@

app.img: app.elf
	objcopy $^ -O binary $@

clean:
	rm -f *.o app.img app.elf
	make -C tools clean

run: app.img
	make -C tools 
	sleep 5
	./tools/loadmetal app.img
	./tools/devmem2 0x400000bc w 0x20000000 # set address of our first instruction
	
	
