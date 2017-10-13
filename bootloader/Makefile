NAME=bootloader
OBJECTS=bootloader.o
CFLAGS=-march=armv7-a

all: bootloader.img

%.o: %.S
	gcc -c $^ -o $@ ${CFLAGS}

$(NAME).elf: $(OBJECTS)
	ld -nostartfile -T linker.ld $^ -o $@

$(NAME).img: $(NAME).elf
	objcopy $^ -O binary $@

clean:
	rm -f $(NAME).elf $(NAME).img $(NAME).o