/* 
    basic nanoMPK.h file 
    TODO: hashed (virturalized) pkeys or self-defined allocator
*/
#ifndef NANO_MPK_H
#define NANO_MPK_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "pkeys.h"

#define NANO_USE_LOG 1

#define errExit(msg)        \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define _to_log(format, ...)                                  \
    do                                                        \
    {                                                         \
        if (NANO_USE_LOG)                                     \
        {                                                     \
            fprintf(stderr, "[nano] " format, ##__VA_ARGS__); \
            fflush(NULL);                                     \
        }                                                     \
    } while (0)

#define mpk_set(__pkey, __prot)   \
    do                            \
    {                             \
        _pkey_set(__pkey, __prot) \
    } while (0)

#define nano_mmap_domain(addr, length, prot, flags, fd, offset, Pkey)                                                                                         \
    ({                                                                                                                                                        \
        void *__tmp = NULL;                                                                                                                                   \
        __tmp = ((__tmp = mmap(addr, length, prot, flags, fd, offset)) == MAP_FAILED || pkey_mprotect(__tmp, length, prot, Pkey) == -1) ? (void *)-1 : __tmp; \
        __tmp;                                                                                                                                                \
    })

    typedef struct _mpk_node
    {
        void *buf;
        size_t len;
        int prot;
        int pkey;
        int id;
        struct _mpt_node *next;
    } mpk_node;

    static inline int
    my_pkey_set(int pkru)
    {
        asm volatile(".byte 0x0f,0x01,0xef\n\t"
                     :
                     : "a"(pkru), "c"(0), "d"(0));
        return 0;
    }

    // called only once at nanoNF load and unload
    // return 0 for success, other for failure
    int nanoMPK_init(size_t *poolsizearray);
    int nanoMPK_fini();

    //called when a NF start or end
    int NF__start__();
    int NF__end__();

    // Create memory domain and return it to user
    // not implemented in dlmalloc version
    int mpk_create(void);

    // Remove memory domain mpk from kernel
    // not implemented in dlmalloc version
    int mpk_kill(int mpk_id);

    // Allocate memory region in memory domain mpk
    void *mpk_mmap(int mpk_id, void *__addr, size_t __len, int __prot, int __flags, int __fd, __off_t __offset);

    // Allocate npages pages in memory domain mpk
    void *mpk_alloc(int mpk_id, size_t nbytes); // in dlmalloc not implemented
    void *mpk_malloc(size_t nbytes);
    void *mpk_zmalloc(size_t nbytes);
    void *mpk_remalloc(void *ptr, size_t nbytes);

    // Deallocate npages pages in memory domain mpk
    void mpk_free(void *ptr);

#define NANO_MPKED_FUNC0(ret, f, fint) \
    ret f()                            \
    {                                  \
        ret ret_val;                   \
        NF__start__();                 \
        ret_val = fint();              \
        NF__end__();                   \
        return ret_val;                \
    }
#define NANO_MPKED_FUNC1(ret, f, fint, type1) \
    ret f(type1 arg1)                         \
    {                                         \
        ret ret_val;                          \
        NF__start__();                        \
        ret_val = fint(arg1);                 \
        NF__end__();                          \
        return ret_val;                       \
    }
#define NANO_MPKED_FUNC2(ret, f, fint, type1, type2) \
    ret f(type1 arg1, type2 arg2)                    \
    {                                                \
        ret ret_val;                                 \
        NF__start__();                               \
        ret_val = fint(arg1, arg2);                  \
        NF__end__();                                 \
        return ret_val;                              \
    }
#define NANO_MPKED_FUNC3(ret, f, fint, type1, type2, type3) \
    ret f(type1 arg1, type2 arg2, type3 arg3)               \
    {                                                       \
        ret ret_val;                                        \
        NF__start__();                                      \
        ret_val = fint(arg1, arg2, arg3);                   \
        NF__end__();                                        \
        return ret_val;                                     \
    }
#define NANO_MPKED_FUNC4(ret, f, fint, type1, type2, type3, type4) \
    ret f(type1 arg1, type2 arg2, type3 arg3, type4 arg4)          \
    {                                                              \
        ret ret_val;                                               \
        NF__start__();                                             \
        ret_val = fint(arg1, arg2, arg3, arg4);                    \
        NF__end__();                                               \
        return ret_val;                                            \
    }
#define NANO_MPKED_FUNC5(ret, f, fint, type1, type2, type3, type4, type5) \
    ret f(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5)     \
    {                                                                     \
        ret ret_val;                                                      \
        NF__start__();                                                    \
        ret_val = fint(arg1, arg2, arg3, arg4, arg5);                     \
        NF__end__();                                                      \
        return ret_val;                                                   \
    }
#define NANO_MPKED_FUNC6(ret, f, fint, type1, type2, type3, type4, type5, type6)  \
    ret f(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6) \
    {                                                                             \
        ret ret_val;                                                              \
        NF__start__();                                                            \
        ret_val = fint(arg1, arg2, arg3, arg4, arg5, arg6);                       \
        NF__end__();                                                              \
        return ret_val;                                                           \
    }
#define NANO_MPKED_FUNC7(ret, f, fint, type1, type2, type3, type4, type5, type6, type7)       \
    ret f(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6, type7 arg7) \
    {                                                                                         \
        ret ret_val;                                                                          \
        NF__start__();                                                                        \
        ret_val = fint(arg1, arg2, arg3, arg4, arg5, arg6, arg7);                             \
        NF__end__();                                                                          \
        return ret_val;                                                                       \
    }
#define NANO_MPKED_FUNC8(ret, f, fint, type1, type2, type3, type4, type5, type6, type7, type8)            \
    ret f(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6, type7 arg7, type8 arg8) \
    {                                                                                                     \
        ret ret_val;                                                                                      \
        NF__start__();                                                                                    \
        ret_val = fint(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);                                   \
        NF__end__();                                                                                      \
        return ret_val;                                                                                   \
    }

#ifdef __cplusplus
}
#endif

#endif /* NANO_MPK_H */
