#include <errno.h>
#include <fcntl.h>
#include <linux/hdreg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <scsi/sg.h>

#include "ata_drives.h"


//
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


//
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
        cmdblk[6]    = 0;
        cmdblk[8]    = 0;
        cmdblk[10]   = 0;
        cmdblk[12]   = 0;
        cmdblk[13]   = 0x40;
        cmdblk[14]   = ATA_OP_SETFEATURES;

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


//HDIO_GET_IDENTITY
int getFeature2(int fd, int feature)
{
struct hd_driveid  driveid; 
fd = open("/dev/sda", O_RDONLY);  // validated fd.
int retval;
retval = ioctl(fd, HDIO_GET_IDENTITY, &driveid);
if(retval < 0) {
            perror("ioctl(HDIO_GET_IDENTITY)");
            return -1;
}
printf("Word82: %d\n", driveid.command_set_1);
printf("Word85: %d\n", driveid.cfs_enable_1);

return 0;
}
//

int getFeature1(int fd, int feature)
{
#define INQ_REPLY_LEN 96
#define INQ_CMD_CODE 0x12
#define INQ_CMD_LEN 6

    int sg_fd, k;
    unsigned char inqCmdBlk[INQ_CMD_LEN] =
                    {INQ_CMD_CODE, 0, 0, 0, INQ_REPLY_LEN, 0};
/* This is a "standard" SCSI INQUIRY command. It is standard because the
 * CMDDT and EVPD bits (in the second byte) are zero. All SCSI targets
 * should respond promptly to a standard INQUIRY */
    unsigned char inqBuff[INQ_REPLY_LEN];
    unsigned char sense_buffer[32];
    sg_io_hdr_t io_hdr;

//    if ((ioctl(fd, SG_GET_VERSION_NUM, &k) < 0) || (k < 30000)) {
//        printf("is not an sg device, or old sg driver\n");
//        return 1;
//    }
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(inqCmdBlk);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = INQ_REPLY_LEN;
    io_hdr.dxferp = inqBuff;
    io_hdr.cmdp = inqCmdBlk;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 20000;

    if (ioctl(sg_fd, SG_IO, &io_hdr) < 0) {
        perror("sg_simple0: Inquiry SG_IO ioctl error");
        return 1;
    }

    char * p = (char *)inqBuff;
    printf("Some of the INQUIRY command's response:\n");
    printf("    %.8s  %.16s  %.4s\n", p + 8, p + 16, p + 32);
    printf("INQUIRY duration=%u millisecs, resid=%d\n",
           io_hdr.duration, io_hdr.resid);

return 0;
}


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
/*
    inqCmdBlk[0] = 0x85;
    inqCmdBlk[1] = 0x08;
    inqCmdBlk[2] = 0;
    inqCmdBlk[3] = 0;
    inqCmdBlk[4] = 512;
    inqCmdBlk[5] = 1;
*/
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

    unsigned long int err;
    err = ioctl(fd, SG_IO, &sg);
    if(err)
    {
        printf("Error = %d\n", err);
        perror("ioctl(SG_IO)");
        return -1;
    }
    else
    {
//       printf("inquire ok\n");
       printf("Write-cache is : %d\n", inqBuff[85]);
    }


return 0;
}

/*
__u64 tf_to_lba (struct ata_tf *tf)
{
        __u32 lba24, lbah;
        __u64 lba64;

        lba24 = (tf->lob.lbah << 16) | (tf->lob.lbam << 8) | (tf->lob.lbal);
        if (tf->is_lba48)
                lbah = (tf->hob.lbah << 16) | (tf->hob.lbam << 8) | (tf->hob.lbal);
        else
                lbah = (tf->dev & 0x0f);
        lba64 = (((__u64)lbah) << 24) | (__u64)lba24;
        return lba64;
}
*/

int getFeature3(int fd, int feature)
{
    #define SG_ATA_16               0x85
    #define SG_ATA_16_LEN           16
    #define ATA_OP_IDENTIFY		0xec

    struct sg_io_hdr io_hdr;
    unsigned char cdb[SG_ATA_16_LEN] = {0};
    unsigned char sb[32] = {0};
    unsigned char buf[512] = {0};

        cdb[0] = SG_ATA_16;
        cdb[1] = 0x08;
        cdb[2] = 0x0e;
        cdb[6] = 0x01; //No. of Sectors To Read
//        cdb[13] = ATA_USING_LBA;
        cdb[14] = ATA_OP_IDENTIFY;

        io_hdr.cmd_len = SG_ATA_16_LEN;
        io_hdr.interface_id = 'S';
        io_hdr.mx_sb_len= sizeof(sb);
        io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
        io_hdr.dxfer_len = sizeof(buf);
        io_hdr.dxferp = buf;
        io_hdr.cmdp = cdb;
        io_hdr.sbp = sb;
//        io_hdr.pack_id = tf_to_lba(&tf);
       	io_hdr.timeout = 0;

        printf("Word82: %d\n", buf[82]);
        printf("Word85: %d\n", buf[85]);

return 0;
}

