#include <errno.h>
#include <fcntl.h>
#include <linux/hdreg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <strings.h>
#include <unistd.h>
#include <scsi/sg.h>

#include "ata_drives.h"

int openDrive(const char* drivePath, int* fd)
{
    if (geteuid() >  0)
    {
        printf("ERROR: Must be root to use\n");
        return 1;
    }

    if ((*fd = open(drivePath, O_RDWR)) < 0)
    {
        printf("ERROR: Cannot open device %s\n", drivePath); 
        return 1;
    }

    printf("Drive opened: %s\n", drivePath);

return 0;
}

int setFeature(int fd, int feature)
{
    printf("Setting feature %d for %d\n", feature, fd);

        unsigned char senseBuf[32];

        unsigned char buf[512];
        bzero(buf, 512);

        unsigned char cmdblk[16];
        bzero(cmdblk, sizeof(cmdblk));

        cmdblk[0]    = 0x85;   // SG_ATA_16
        cmdblk[1]    = 0x6;    // SG_ATA_PROTO_NON_DATA 
        cmdblk[2]    = 0x20;   // SG_CDB2_CHECK_COND
        cmdblk[4]    = feature;
//        cmdblk[4]    = 0x82;   // SETFEATURES_DIS_WCACHE;
        cmdblk[6]    = 0;
        cmdblk[8]    = 0;
        cmdblk[10]   = 0;
        cmdblk[12]   = 0;
        cmdblk[13]   = 0x40;
        cmdblk[14]   = ATA_OP_SETFEATURES;
//        cmdblk[14]   = 0xef;   //WIN_SETFEATURES;

        sg_io_hdr_t sghdr;
        bzero(&sghdr, sizeof(sg_io_hdr_t));

        sghdr.interface_id      = 'S';
        sghdr.cmdp              = cmdblk;
        sghdr.cmd_len           = sizeof(cmdblk);
        sghdr.dxfer_direction   = SG_DXFER_NONE;
        sghdr.dxferp            = NULL;
        sghdr.dxfer_len         = 0;
        sghdr.sbp               = senseBuf;
        sghdr.mx_sb_len         = sizeof(senseBuf);
        sghdr.timeout           = SCSI_DEFAULT_TIMEOUT;


        unsigned long int err;
        err = ioctl(fd, SG_IO, &sghdr);
        if(err)
        {
            printf("Error = %d\n", err);
            perror("ioctl(SG_IO)");
        }

return 0;
}

/*
int getFeature(int fd, int feature)
{
    unsigned char inqCmdBlk[16];
//= {INQ_CMD_CODE, 0, 0, 0, INQ_REPLY_LEN, 0};
//\x85\x08\x0e\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x40\xec\x00
    inqCmdBlk[0] = 0x85;
    inqCmdBlk[1] = 0x08;
    inqCmdBlk[2] = 0x0e;
    inqCmdBlk[6] = 0x01;
    inqCmdBlk[13] = 0x40;
    inqCmdBlk[14] = 0xec;
//    inqCmdBlk[0] = 0x12;
    inqCmdBlk[0] = 0x85;
    inqCmdBlk[1] = 0x08;
    inqCmdBlk[2] = 0;
    inqCmdBlk[3] = 0;
    inqCmdBlk[4] = 512;
    inqCmdBlk[5] = 1;

    unsigned char inqBuff[512];
    unsigned char sense_buffer[32];
    sg_io_hdr_t sg;
    bzero(&sg, sizeof(sg_io_hdr_t));
    bzero(inqBuff, sizeof(inqBuff));
    bzero(sense_buffer, sizeof(sense_buffer));

    sg.interface_id = 'S';
    sg.cmd_len = sizeof(inqCmdBlk);
    sg.mx_sb_len = sizeof(sense_buffer);
    sg.dxfer_direction = SG_DXFER_FROM_DEV;
    sg.dxfer_len = 512;
    sg.dxferp = inqBuff;
    sg.cmdp = inqCmdBlk;
    sg.sbp = sense_buffer;
    sg.timeout = SCSI_DEFAULT_TIMEOUT;

    if (ioctl(fd, SG_IO, &sg) < 0)
        error(1, errno, "couldn't inquire %s", argv[1]);
    else
    {
       printf("inquire ok\n");
       printf("Write-cache is : %d\n", inqBuff[85]);
    }


exit(0);
}
*/
