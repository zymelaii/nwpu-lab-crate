/*
 * Filesystem buffer cache
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/mem.h>
#include <geekos/malloc.h>
#include <geekos/blockdev.h>
#include <geekos/bufcache.h>

/*
 * Maximum number of buffers that are cached per-filesystem.
 */
#define FS_BUFFER_CACHE_MAX_BLOCKS 128

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

int bufCacheDebug = 0;
#define Debug(args...) if (bufCacheDebug) Print(args)

/* XXX */
int noEvict = 0;

/*
 * Get number of sectors per filesystem block for given
 * fs buffer cache.
 */
static uint_t Get_Num_Sectors_Per_FS_Block(struct FS_Buffer_Cache *cache)
{
    return (cache->fsBlockSize / SECTOR_SIZE);
}

/*
 * Read or write a filesystem buffer.
 */
static int Do_Buffer_IO(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf,
    int (*IO_Func)(struct Block_Device *dev, int blockNum, void *buf))
{
    uint_t offset;
    int sectorCount = 0;
    int blockNum = buf->fsBlockNum * Get_Num_Sectors_Per_FS_Block(cache);
    char *ptr = (char*) buf->data;

    for (offset = 0; offset < cache->fsBlockSize; offset += SECTOR_SIZE) {
	int rc = IO_Func(cache->dev, blockNum, ptr + offset);
	if (rc != 0)
	    return rc;
	++sectorCount;
	++blockNum;
    }
    KASSERT(offset == cache->fsBlockSize);
    KASSERT(sectorCount == Get_Num_Sectors_Per_FS_Block(cache));

    return 0;
}

/*
 * If necessary, write back uncomitted buffer contents to block device.
 */
static int Sync_Buffer(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf)
{
    int rc = 0;

    KASSERT(IS_HELD(&cache->lock));

    if (buf->flags & FS_BUFFER_DIRTY) {
	if ((rc = Do_Buffer_IO(cache, buf, Block_Write)) == 0)
	    buf->flags &= ~(FS_BUFFER_DIRTY);
    }

    return rc;
}

/*
 * Move a buffer to the front of the cache buffer list,
 * to indicate that it has been used recently.
 */
static void Move_To_Front(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf)
{
    Remove_From_FS_Buffer_List(&cache->bufferList, buf);
    Add_To_Front_Of_FS_Buffer_List(&cache->bufferList, buf);
}

/*
 * Get buffer for given block, and mark it in use.
 * Must be called with cache mutex held.
 */
static int Get_Buffer(struct FS_Buffer_Cache *cache, ulong_t fsBlockNum, struct FS_Buffer **pBuf)
{
    struct FS_Buffer *buf, *lru = 0;
    int rc;

    Debug("Request block %lu\n", fsBlockNum);

    KASSERT(IS_HELD(&cache->lock));

    /*
     * Look for existing buffer.
     * As a side-effect, finds the least recently used
     * buffer that is not in use (if any).
     */
    buf = Get_Front_Of_FS_Buffer_List(&cache->bufferList);
    while (buf != 0) {
	if (buf->fsBlockNum == fsBlockNum) {
	    /* If buffer is in use, wait until it is available. */
	    while (buf->flags & FS_BUFFER_INUSE) {
		Debug("Waiting for block %lu\n", fsBlockNum);
		Cond_Wait(&cache->cond, &cache->lock);
	    }
	    goto done;
	}

	/* If buffer isn't in use, it's a candidate for LRU. */
	if (!(buf->flags & FS_BUFFER_INUSE))
	    lru = buf;

	buf = Get_Next_In_FS_Buffer_List(buf);
    }

    /*
     * If number of allocated buffers does not exceed the
     * limit, allocate a new one.
     */
    if (cache->numCached < FS_BUFFER_CACHE_MAX_BLOCKS) {
	buf = (struct FS_Buffer*) Malloc(sizeof(*buf));
	if (buf != 0) {
	    buf->data = Alloc_Page();
	    if (buf->data == 0)
		Free(buf);
	    else {
		/* Successful creation */
		buf->fsBlockNum = fsBlockNum;
		buf->flags = 0;
		Add_To_Front_Of_FS_Buffer_List(&cache->bufferList, buf);
		++cache->numCached;
		goto readAndAcquire;
	    }
	}
    }
    
    /*
     * If there is no LRU buffer, then we have exceeded
     * the number of available buffers.
     */
    if (lru == 0)
	return ENOMEM;

    KASSERT(!noEvict);

    /* Make sure the LRU buffer is clean. */
    if ((rc = Sync_Buffer(cache, lru)) != 0)
	return rc;

    /* LRU buffer is clean, so we can steal it. */
    buf = lru;
    buf->flags = 0;
    Move_To_Front(cache, buf);

readAndAcquire:
    /*
     * The buffer selected should be clean (no uncommitted data),
     * and should have been moved to the front of the buffer list
     * (to show it has just been referenced).
     */
    KASSERT(!(buf->flags & FS_BUFFER_DIRTY));
    KASSERT(Get_Front_Of_FS_Buffer_List(&cache->bufferList) == buf);

    /* Read block data into buffer. */
    if ((rc = Do_Buffer_IO(cache, buf, Block_Read)) != 0)
	return rc;

done:
    /* Buffer is now in use. */
    buf->flags |= FS_BUFFER_INUSE;

    /* Success! */
    Debug("Acquired block %lu\n", fsBlockNum);
    *pBuf = buf;
    return 0;
}

/*
 * Synchronize cache with disk.
 */
static int Sync_Cache(struct FS_Buffer_Cache *cache)
{
    int rc = 0;
    struct FS_Buffer *buf;

    KASSERT(IS_HELD(&cache->lock));

    buf = Get_Front_Of_FS_Buffer_List(&cache->bufferList);
    while (buf != 0) {
	if ((rc = Sync_Buffer(cache, buf)) != 0)
	    break;
	buf = Get_Next_In_FS_Buffer_List(buf);
    }

    return rc;
}

/*
 * Free the memory used by a filesystem buffer.
 */
static void Free_Buffer(struct FS_Buffer *buf)
{
    KASSERT(!(buf->flags & (FS_BUFFER_DIRTY | FS_BUFFER_INUSE)));
    Free_Page(buf->data);
    Free(buf);
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Create a cache of filesystem buffers.
 */
struct FS_Buffer_Cache *Create_FS_Buffer_Cache(struct Block_Device *dev, uint_t fsBlockSize)
{
    struct FS_Buffer_Cache *cache;

    KASSERT(dev != 0);
    KASSERT(dev->inUse);

    /*
     * Currently, we don't allow filesystem blocks
     * larger than the hardware page size.
     */
    KASSERT(fsBlockSize <= PAGE_SIZE);

    cache = (struct FS_Buffer_Cache*) Malloc(sizeof(*cache));
    if (cache == 0)
	return 0;

    cache->dev = dev;
    cache->fsBlockSize = fsBlockSize;
    cache->numCached = 0;
    Clear_FS_Buffer_List(&cache->bufferList);
    Mutex_Init(&cache->lock);
    Cond_Init(&cache->cond);

    return cache;
}

/*
 * Synchronize contents of cache with the disk
 * by writing out all dirty buffers.
 */
int Sync_FS_Buffer_Cache(struct FS_Buffer_Cache *cache)
{
    int rc;

    Mutex_Lock(&cache->lock);
    rc = Sync_Cache(cache);
    Mutex_Unlock(&cache->lock);

    return rc;
}

/*
 * Destroy a filesystem buffer cache.
 * None of the buffers in the cache must be in use.
 * The cache must not be used after this function returns!
 */
int Destroy_FS_Buffer_Cache(struct FS_Buffer_Cache *cache)
{
    int rc;
    struct FS_Buffer *buf;

    Mutex_Lock(&cache->lock);

    /* Flush all contents back to disk. */
    rc = Sync_Cache(cache);

    /* Free all of the buffers. */
    buf = Get_Front_Of_FS_Buffer_List(&cache->bufferList);
    while (buf != 0) {
	struct FS_Buffer *next = Get_Next_In_FS_Buffer_List(buf);
	Free_Buffer(buf);
	buf = next;
    }
    Clear_FS_Buffer_List(&cache->bufferList);

    Mutex_Unlock(&cache->lock);

    /* Free the cache object itself. */
    Free(cache);

    return rc;
}

/*
 * Get a buffer for given filesystem block.
 */
int Get_FS_Buffer(struct FS_Buffer_Cache *cache, ulong_t fsBlockNum, struct FS_Buffer **pBuf)
{
    int rc;

    Mutex_Lock(&cache->lock);
    rc = Get_Buffer(cache, fsBlockNum, pBuf);
    Mutex_Unlock(&cache->lock);

    return rc;
}

/*
 * Mark the given buffer as being modified.
 */
void Modify_FS_Buffer(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf)
{
    KASSERT(buf->flags & FS_BUFFER_INUSE);
    buf->flags |= FS_BUFFER_DIRTY;
}

/*
 * Explicitly synchronize given buffer with its on-disk storage,
 * without releasing the buffer.
 */
int Sync_FS_Buffer(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf)
{
    int rc;

    KASSERT(buf->flags & FS_BUFFER_INUSE);

    Mutex_Lock(&cache->lock);
    rc = Sync_Buffer(cache, buf);
    Mutex_Unlock(&cache->lock);

    return rc;
}

/*
 * Release given buffer.
 */
int Release_FS_Buffer(struct FS_Buffer_Cache *cache, struct FS_Buffer *buf)
{
    int rc = 0;

    KASSERT(buf->flags & FS_BUFFER_INUSE);

    Mutex_Lock(&cache->lock);

    /*
     * If the buffer is OK to release,
     * mark it as no longer in use and notify any
     * thread waiting to use it.
     */
    if (rc == 0) {
	buf->flags &= ~(FS_BUFFER_INUSE);
	Cond_Broadcast(&cache->cond);
    }
    Debug("Released block %lu\n", buf->fsBlockNum);

    Mutex_Unlock(&cache->lock);

    return rc;
}

