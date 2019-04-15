#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "ata_drives.h"


int main(int argc, char *argv[])
{
    int fd;

    if ( openDrive(argv[1], &fd) != 0 )
    {
        exit(1);
    }

    setFeature(fd, SETFEATURES_DIS_WCACHE);


exit(0);
}
