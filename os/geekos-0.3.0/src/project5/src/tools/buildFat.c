#include <geekos/pfat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECTOR_SIZE 512

int roundToNextBlock(int x)
{
    if (x % SECTOR_SIZE == 0) {
        return x;
    } else {
        return (x + SECTOR_SIZE - (x % SECTOR_SIZE));
    }
}

int main(int argc, char *argv[])
{
    int i;
    int fd;
    int fd2;
    int ret;
    int curr;
    int *fat;
    int blocks;
    int diskSize;
    int fileCount;
    char *imageFile;
    struct stat sbuf;
    int firstFreeBlock;
    bootSector bSector;
    directoryEntry *directory;
    int writeBoot = 0;

    if (argc <= 1) {
        printf("usage: buildFat [-b <boot block> ] <diskImage> <files>\n");
	exit(-1);
    }

    curr = 2;
    if (!strcmp(argv[1], "-b")) {
        /* it's a boot disk */
	printf("writing boot block\n");
	curr += 2;
	writeBoot = 1;
    }

    imageFile = argv[curr-1];
    printf("image file = %s\n", imageFile);

    ret = stat(imageFile, &sbuf);
    if (ret) {
        perror("stat");
	exit(-1);
    }

    fileCount = argc - curr;
    diskSize = sbuf.st_size;
    if (diskSize % SECTOR_SIZE != 0) {
        printf("image is not a multiple of 512 bytes\n");
	exit(-1);
    }

    blocks = diskSize / SECTOR_SIZE;

    bSector.magic = PFAT_MAGIC;
    bSector.fileAllocationOffset = 1;
    bSector.fileAllocationLength = roundToNextBlock(blocks)/SECTOR_SIZE*4;
    fat = (int *) calloc(blocks, sizeof(int));
    bSector.rootDirectoryOffset = bSector.fileAllocationLength + 1;
    bSector.rootDirectoryCount = fileCount;

    fd = open(imageFile, O_WRONLY, 0);
    if (fd < 0) {
        perror("image File open:");
	exit(-1);
    }

    if (writeBoot) {
        /* copy boot block */
	char buffer[SECTOR_SIZE];

	fd2 = open(argv[2], O_RDONLY, 0);

	ret = read(fd2, buffer, SECTOR_SIZE);
	if (ret != SECTOR_SIZE) {
	    printf("unable to read boot record\n");
	}

	lseek(fd, 0, SEEK_SET);

	ret = write(fd, buffer, SECTOR_SIZE);
	if (ret != SECTOR_SIZE) {
	    printf("unable to write boot record\n");
	}

	close(fd2);
    }

    firstFreeBlock = bSector.rootDirectoryOffset + 
        roundToNextBlock(sizeof(directoryEntry) * fileCount)/ SECTOR_SIZE;
    printf("first data blocks is %d\n", firstFreeBlock);

    directory = (directoryEntry*) malloc(sizeof(directoryEntry) * fileCount);
    for (i=0; i < fileCount; i++) {
	int j;
	int numBlocks;
	const char *filename = argv[i+curr];

	directory[i].firstBlock = firstFreeBlock;

	ret = stat(filename, &sbuf);
	if (ret != 0) {
	    printf("Error stating %s\n", filename);
	    exit(-1);
	}
	assert(ret == 0);
	numBlocks = roundToNextBlock(sbuf.st_size)/SECTOR_SIZE;
	directory[i].fileSize = sbuf.st_size;

        if (writeBoot) {
	   if (i== 0) {
	       /* setup.bin */
	       bSector.setupStart = firstFreeBlock;
	       bSector.setupSize = numBlocks;
	       printf("setup file starts at %d, %d sectors long\n",
		   bSector.setupStart, bSector.setupSize);
	   } else if (i==1) {
	       /* kernel.exe */
	       bSector.kernelStart = firstFreeBlock;
	       bSector.kernelSize = numBlocks;
	       printf("kernel file starts at %d, %d sectors long\n",
		   bSector.kernelStart, bSector.kernelSize);
	   }
	}
	for (j=0; j < numBlocks-1; j++) {
	    fat[firstFreeBlock] = firstFreeBlock + 1;
	    ++firstFreeBlock;

	    if (firstFreeBlock > (diskSize/SECTOR_SIZE)) {
		printf("Error: %s is full\n", imageFile);
		exit(-1);
	    }
	}
	fat[firstFreeBlock++] = FAT_ENTRY_EOF;

	lseek(fd, directory[i].firstBlock * SECTOR_SIZE, SEEK_SET);

	/* copy the file to the disk */
	fd2 = open(filename, O_RDONLY, 0);
	assert(fd2);

	/* Remove leading directory path components */
	if (strrchr(filename, '/') != 0)
	    filename = strrchr(filename, '/') + 1;

	/* Set filename in directory entry */
	strncpy(directory[i].fileName, filename, sizeof(directory[i].fileName));

	printf("file %s starts at block %d\n", directory[i].fileName, directory[i].firstBlock);
	lseek(fd, directory[i].firstBlock * SECTOR_SIZE, SEEK_SET);

	/* copy the file to the disk */
	for (j=0; j < numBlocks; j++) {
	    int ret2;
	    char buffer[SECTOR_SIZE];

	    ret = read(fd2, buffer, SECTOR_SIZE);
	    assert(ret >= 0);
	    ret2 = write(fd, buffer, ret);
	    assert(ret2 == ret);
	}
	close(fd2);
    }

    lseek(fd, SECTOR_SIZE, SEEK_SET);
    write(fd, fat, sizeof(int) * blocks);

    lseek(fd, bSector.rootDirectoryOffset * SECTOR_SIZE, SEEK_SET);
    printf("putting the directory at sector %d\n", bSector.rootDirectoryOffset);
    write(fd, directory, sizeof(directoryEntry) * fileCount);

    /* write out boot record */
    lseek(fd, PFAT_BOOT_RECORD_OFFSET, SEEK_SET);
    ret = write(fd, &bSector, sizeof(bSector));
    assert(ret == sizeof(bSector));

    close(fd);

    exit(0);
}
