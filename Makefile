OBJECTS=armc-03.o armc-08-cstartup.o armc-08-start.o

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
	./tools/loadmetal app.img
	./tools/devmem2 0x400000bc w 0x20000000
	
	
