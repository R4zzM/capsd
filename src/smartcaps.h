#ifndef SMARTCAPS_H
#define SMARTCAPS_H

#include <sys/poll.h>

struct kbdstate {
    struct pollfd *pfds; /* Filedescriptors to use with poll system call */
    int npfds;           /* Number of initialized filedescriptors */
    int capsdown;        /* Is CAPSLOCK pressed down */
    int lastcode;        /* Last keycode that was pressed */ 
};

#endif /* SMARTCAPS_H */
