CC=gcc
CFLAGS=-I.
ODIR=obj

hellomake: ata_set.o ata_drives.o
	$(CC) -o ata_set ata_set.o ata_drives.o
	rm -f ./*o
