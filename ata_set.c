#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "ata_drives.h"


int main(int argc, char *argv[])
{
    int fd;

    if ( openDrive(argv[1], &fd) != 0 )
    {
        printf("Exited: openDrive return error\n");
        exit(1);
    }

    if ( setFeature(fd, SETFEATURES_DIS_WCACHE) != 0 )
    {
        printf("setFeature return error\n");
    }

    if ( getFeature(fd, 0) != 0 )
    {
        printf("setFeature return error\n");
    }


exit(0);
}
