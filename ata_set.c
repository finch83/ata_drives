#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "ata_drives.h"


int main(int argc, char *argv[])
{
/*
    unsigned char shift = 0;
    shift |= 1 << 7;
    printf("Shift: %d\n", shift);
*/
    int fd;

    if ( openDrive(argv[1], &fd) != 0 )
    {
        exit(1);
    }

    unsigned char set_cache = SETFEATURES_DIS_WCACHE;
    if ( 3 == argc )
    {
        set_cache = (argv[2][0] - '0') == 0 ?
                    SETFEATURES_DIS_WCACHE :
                    SETFEATURES_EN_WCACHE;
    }
    printf("%sabling cahche\n", set_cache==SETFEATURES_DIS_WCACHE ? "Dis" : "En");

    if ( setFeature(fd, set_cache) != 0 )
    {
        printf("setFeature return error\n");
    }
/*
    if ( getFeature3(fd, 0) != 0 )
    {
        printf("setFeature return error\n");
    }
*/

exit(0);
}
