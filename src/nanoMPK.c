#include "nanoMPK.h"
#include <sys/mman.h>
#include "shared_malloc.h"

/*
    using dlmalloc
    TODO: change API to memgrid, seal meta data, lessen and protect global variables
*/

struct _mpk_pool 
{
    int pkey;
    void *pool_location;
    unsigned long long pool_size;
} mpk_pool[NR_PKEYS - 1];

static int current_mpkid;

#define NANO_MPK_POOL(i) ((struct sh_memory_pool *)(mpk_pool[i].pool_location))

int nanoMPK_init(size_t *PoolSizeArray)
{
    current_mpkid = 0;
    
    for (size_t i = 0; i < NR_PKEYS; i++)
    {
        mpk_pool[i].pkey = pkey_alloc(0, 0);
        if (PoolSizeArray[i] == 0)
        {
            _to_log("Shared Memory allocation empty - DON'T USE any mpk_malloc for this pool!");
            continue;
            //return 1;
        }

        // at least allocate 2 pages (otherwise shmem library will fail)
        if (PoolSizeArray[i] < 8192)
            PoolSizeArray[i] = 8192;

        mpk_pool[i].pool_location = nano_mmap_domain(NULL, PoolSizeArray[i], PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0, mpk_pool[i].pkey);

        if (mpk_pool[i].pool_location == MAP_FAILED)
        {
            perror("Could not allocate pool memory");
            return 1;
        }

        //TODO: mapping to physic only when necessary
        memset(mpk_pool[i].pool_location, 0, PoolSizeArray[i]);

        if (init_sh_mempool(mpk_pool[i].pool_location, PoolSizeArray[i]) == (NANO_MPK_POOL(i)))
        {
            _to_log("Allocated shared memory at %p of size %lld", NANO_MPK_POOL(i), PoolSizeArray[i]);
        }
        else
        {
            perror("Failed to allocate shared memory");
            return 1;
        }
    }
    return 0;
}

int nanoMPK_fini()
{
    int error_count = 0;
    for (size_t i = 0; i < NR_PKEYS - 1; i++)
    {
        pkey_free(mpk_pool[i].pkey);
        error_count += munmap(mpk_pool[i].pool_location, mpk_pool[i].pool_size);
    }
    if (error_count)
    {
        perror("munmap");
        return 1;
    }
    else
        return 0;
}

int NF__start__(void)
{
    do
        current_mpkid = (current_mpkid+1)%(NR_PKEYS - 1);
    while(mpk_pool[current_mpkid].pool_size == 0);
    my_pkey_set(~(0x3 << (2 * mpk_pool[current_mpkid].pkey)));
    // my_pkey_set(0x55555555); for test
    return 0;
}

int NF__end__(void)
{// set current pool ready for map and malloc
        if (init_sh_mempool(mpk_pool[current_mpkid].pool_location, mpk_pool[current_mpkid].pool_size) == (NANO_MPK_POOL(current_mpkid)))
        {
            _to_log("Allocated shared memory at %p of scurrent_mpkidze %llu", NANO_MPK_POOL(current_mpkid), mpk_pool[current_mpkid].pool_size);
            return 0;
        }
        else
        {
            _to_log("Failed to allocate shared memory");
            return 1;
        }
}
//use nano_mmap_domain instead
void *mpk_mmap(int mpk_id, void *__addr, size_t __len, int __prot,
               int __flags, int __fd, __off_t __offset)
{
    void *__tmp = NULL;
    __tmp = mmap(__addr, __len, __prot, __flags, __fd, __offset);
    if (__tmp == MAP_FAILED)
        perror("mpk_mmap");
    if (pkey_mprotect(__tmp, __len, __prot, mpk_pool[current_mpkid].pkey) == -1)
        return __tmp;
    else
        perror("pkey_mprotect");
}

void *mpk_malloc(size_t nbytes)
{
    return sh_malloc(nbytes, NANO_MPK_POOL(current_mpkid));
}

void *mpk_zmalloc(size_t nbytes)
{
    void *ptr = mpk_malloc(nbytes);
    memset(ptr, 0, nbytes);
    return ptr;
}

void *mpk_remalloc(void *ptr, size_t nbytes)
{
    return sh_realloc(ptr, nbytes, NANO_MPK_POOL(current_mpkid));
}

void mpk_free(void *ptr)
{
    sh_free(ptr, NANO_MPK_POOL(current_mpkid));
}
