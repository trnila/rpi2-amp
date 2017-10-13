OBJECTS=devmem2 logcat clearmem firmware_load

all: $(OBJECTS)

uio: uio.o lib.o mailbox.o
	$(CC) $(CFLAGS) -o $@ $^

mailbox_send: mailbox_send.o mailbox.o lib.o
	$(CC) $(CFLAGS) -o $@ $^

firmware_load: firmware_load.o lib.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJECTS) *.o
