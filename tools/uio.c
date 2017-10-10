#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib.h"
#include "mailbox.h"

int main() {
	mailbox_init();

	char *data = mapPhys(0x20000000, 1024*1024);
	if(!data) {
		printf("Could not map memory\n");
		exit(1);
	}

	const int max_irq = 9;
	int fds[max_irq];
	for(int i = 0; i < max_irq; i++) {
		char buf[32];
		int n = snprintf(buf, sizeof(buf), "/dev/uio%d", i);
		if(n < 0 || n >= sizeof(buf)) {
			printf("failed to create filename");
			return 1;
		}

		fds[i]  = open(buf, O_RDWR);
		if (fds[i] < 0) {
			perror("open");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < max_irq; i++) {
		unsigned int irq = 10 + i;
		ssize_t nb = write(fds[i], &irq, sizeof(irq));
		if (nb < sizeof(irq)) {
			perror("write");
			exit(1);
		}
	}

	for(;;) {
		fd_set readFds;
		FD_ZERO(&readFds);
		for(int i = 0; i < max_irq; i++) {
			FD_SET(fds[i], &readFds);
		}
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		if(select(fds[max_irq - 1] + 1, &readFds, NULL, NULL, &tv)) {
			for(int i = 0; i < max_irq; i++) {
				if(FD_ISSET(fds[i], &readFds)) {
					unsigned int irq = 10 + i;
					int cpu = i / 3;
					int mailbox = i % 3 + 1;

					printf("i:%d fd:%d irq:%d cpu:%d mailbox:%d\n", i, fds[i], irq, cpu, mailbox);

					unsigned int info;
					ssize_t nb;
					nb = read(fds[i], &info, sizeof(info));
					if (nb == sizeof(info)) {
						unsigned int addr = mailbox_read(cpu, mailbox);

						printf("received %d %x\n", nb, addr);
						printf("'%s'\n", data + (addr - 0x20000000));


						mailbox_clear(cpu, mailbox);
					}

					nb = write(fds[i], &irq, sizeof(irq));
					if (nb < sizeof(irq)) {
						perror("write");
						exit(1);
					}
					
				}
			}
			
		} else {
			printf(".\n");
		}
	}

	return 0;
}
