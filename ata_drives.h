#define SCSI_DEFAULT_TIMEOUT        20000
#define SETFEATURES_EN_WCACHE	    0x02
#define SETFEATURES_EN_WCACHE	0x02
#define SETFEATURES_DIS_WCACHE	    0x82
#define ATA_OP_SETFEATURES          0xef

int setFeature(int fd, int feature);
int getFeature(int fd, int feature);
int openDrive(const char* drivePath, int* fd);
