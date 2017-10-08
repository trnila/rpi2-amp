OBJECTS=loadmetal devmem2 logcat clearmem uio mailbox_send

all: $(OBJECTS)

uio: uio.o lib.o mailbox.o
	$(CC) $(CFLAGS) -o $@ $^

mailbox_send: mailbox_send.o mailbox.o lib.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJECTS) *.o
