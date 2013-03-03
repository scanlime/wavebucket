#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <algorithm>
#include "ledcomm.h"


LEDComm::LEDComm()
{
    fd = open("/dev/tty.usbmodem12341", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    busy = false;

    if (fd < 0) {
        perror("Error opening LED port");
        return;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
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

}

LEDComm::~LEDComm()
{
    if (fd >= 0)
        close(fd);
}

void LEDComm::update()
{
    // Wait for the previous frame to finish or time out
    for (int tries = 0; busy && tries < 5; tries++) {

        // Drain RX buffer
        while (read(fd, buffer, 1) == 1) {
            if (buffer[0] == '*')
                busy = false;
        }

        if (busy) {
            // Wait with timeout
            fd_set fds;
            struct timeval timeout;
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            timeout.tv_sec = 0;
            timeout.tv_usec = 30000;
            select(fd + 1, &fds, NULL, NULL, &timeout);
        }
    }

    // Blocking write loop

    const uint8_t *writePtr = buffer;
    int remaining = sizeof buffer;
    buffer[0] = 'U';

    while (remaining > 0) {
        int r = write(fd, writePtr, 1);
        if (r > 0) {
            remaining -= r;
            writePtr += r;
        } else {
            // Wait with timeout
            fd_set fds;
            struct timeval timeout;
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            timeout.tv_sec = 0;
            timeout.tv_usec = 30000;
            select(fd + 1, NULL, &fds, NULL, &timeout);
        }
    }

    busy = true;
}
