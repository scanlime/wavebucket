/*
 * Very simple serial Vibeam remote client.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <algorithm>
#include "vibeam.h"


Vibeam::Vibeam()
{
    fd = open("/dev/tty.usbserial-A501JKKZ", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

    if (fd < 0) {
        perror("Error opening Vibeam port");
        return;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);                           /* 9600 baud */
    options.c_cflag |= (CLOCAL | CREAD);                    /* Enable receiver and set the local mode */
    options.c_cflag &= ~PARENB;                             /* No parity */
    options.c_cflag &= ~CSIZE;                              /* Mask the character bit size */
    options.c_cflag |= CS8;                                 /* 8 data bits */
    options.c_cflag &= ~CSTOPB;                             /* 1 stop bits */
    options.c_cflag &= ~CRTSCTS;                            /* Disable hardware flow control */
    options.c_iflag &= ~(IXON | IXOFF | IXANY);             /* Disable software flow control */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);     /* Set raw input and output */
    options.c_oflag &= ~OPOST;
    tcsetattr(fd, TCSANOW, &options);

    // Opening the serial port reboots our arduino. Blah.
    bootTime = time(NULL);

    // This is the default number of bytes we can write without an ACK.
    tokens = 1;
}

Vibeam::~Vibeam()
{
    if (fd >= 0)
        close(fd);
}

void Vibeam::txPower(int value)
{
    uint8_t byte;

    // Wait for the Arduino to boot
    if (bootTime) {
        if (time(NULL) > bootTime + 2)
            bootTime = 0;
        else
            return;
    }

    // Replenish our flow control tokens
    while (read(fd, &byte, 1) == 1)
        tokens++;

    if (tokens) {
        byte = std::max<int>(0, std::min<int>(127, value));
        write(fd, &byte, 1);
        tokens--;
    }
}
