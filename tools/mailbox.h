#pragma once

void mailbox_init();
void mailbox_send(int core, int mailbox, unsigned int value);
void mailbox_clear(int core, int mailbox);
unsigned int mailbox_read(int core, int mailbox);
