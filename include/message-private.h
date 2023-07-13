#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include "sdmessage.pb-c.h"

MessageT *read_all(int socket);

int write_all(int sock, MessageT *message);

#endif