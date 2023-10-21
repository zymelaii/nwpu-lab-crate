/*
 * Filesystem buffer cache
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.7 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_BUFCACHE_H
#define GEEKOS_BUFCACHE_H

#include <geekos/ktypes.h>
#include <geekos/list.h>
#include <geekos/synch.h>

struct Block_Device;

/*
 * Bits for FS_Buffer flags.
 */
#define FS_BUFFER_DIRTY	0x01	/*!< Buffer contains uncommitted data. */
#define FS_BUFFER_INUSE	0x02	/*!< Buffer is in use. */

struct FS_Buffer;
DEFINE_LIST(FS_Buffer_List, FS_Buffer);

/*!
 * A buffer containing the data of one filesystem block.
 */
struct FS_Buffer {
    ulong_t fsBlockNum;		/*!< Filesystem block number. */
    void *data;			/*!< In-memory data of block. May be out of sync with disk. */
    uint_t flags;		/*!< Flags representing state of buffer. */
    DEFINE_LINK(FS_Buffer_List, FS_Buffer);
};

IMPLEMENT_LIST(FS_Buffer_List, FS_Buffer);

/*!
 * A cache for buffers containing the data for filesystem blocks.
 * Filesystem implementations should generally do all of their
 * I/O through a buffer cache.
 */
struct FS_Buffer_Cache {
    struct Block_Device *dev;		/*!< Block device. */
    uint_t fsBlockSize;			/*!< Size of filesystem blocks. */
    uint_t numCached;			/*!< Current number of buffers (cached blocks). */
    struct FS_Buffer_List bufferList;	/*!< List of buffers. */
    struct Mutex lock;			/*!< Lock for synchronization. */
    struct Condition cond;		/*!< Condition: waiting for a buffer. */
};

struct FS_Buffer_Cache *Create_FS_Buffer_Cache(struct Block_Device *dev, uint_t fsBlockSize);
int Sync_FS_Buffer_Cache(struct FS_Buffer_Cache *cache);
int Destroy_FS_Buffer_Cache(struct FS_Buffer_Cache *cache);

int Get_FS_Buffer(struct FS_Buffer_Cache *cache, ulong_t fsBlockNum, struct FS_Buffer **pBuf);
void Modify_FS_Buffer(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf);
int Sync_FS_Buffer(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf);
int Release_FS_Buffer(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf);

#endif /* GEEKOS_BUFCACHE_H */
