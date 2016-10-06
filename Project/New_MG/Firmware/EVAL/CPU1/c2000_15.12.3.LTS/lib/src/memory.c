/****************************************************************************/
/*  memory.c v15.12.3							  */
/*                                                              */
/* Copyright (c) 1993-2016 Texas Instruments Incorporated       */
/* http://www.ti.com/                                           */
/*                                                              */
/*  Redistribution and  use in source  and binary forms, with  or without */
/*  modification,  are permitted provided  that the  following conditions */
/*  are met:                                                    */
/*                                                              */
/*     Redistributions  of source  code must  retain the  above copyright */
/*     notice, this list of conditions and the following disclaimer. */
/*                                                              */
/*     Redistributions in binary form  must reproduce the above copyright */
/*     notice, this  list of conditions  and the following  disclaimer in */
/*     the  documentation  and/or   other  materials  provided  with  the */
/*     distribution.                                            */
/*                                                              */
/*     Neither the  name of Texas Instruments Incorporated  nor the names */
/*     of its  contributors may  be used to  endorse or  promote products */
/*     derived  from   this  software  without   specific  prior  written */
/*     permission.                                              */
/*                                                              */
/*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS */
/*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, */
/*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY */
/*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
/*                                                              */
/****************************************************************************/

/*****************************************************************************/
/*									     */
/*  This module contains the functions which implement the dynamic memory    */
/*  management routines.  The algorithms used are based on the algorithms    */
/*  described in Knuth's "The Art Of Computer Programming, Vol 1" on pages   */
/*  435-441. Algorithm 2.5A has been modified to improve the resistance to   */
/*  fragmentation.							     */
/*									     */
/*  Knuth gives two reasons for prefering "first fit" over "best fit".	     */
/*   1) The algorithm is significantly faster, since the whole of the free   */
/*	store does not have to be searched for each allocation		     */
/*	(or deallocation).						     */
/*   2) The first fit algorithm is more resistant to overflow during	     */
/*	repeated allocation and deallocation than is the best fit algorithm  */
/*	(See problems 36-43 on page 456).				     */
/*									     */
/*  The following assumptions/rules apply:				     */
/*									     */
/*   1) Packets are allocated a minimum of MINSIZE words	     	     */
/*   2) The heap can be reset at any time by calling the function	     */
/*	"minit"								     */
/*   3) The heap size must be declared in the linker command file using	     */
/*	the -heap option.						     */
/*									     */
/*  The following items are defined in this module :			     */
/*	 minit()    : Function to initialize dynamic memory management	     */
/*	 malloc()   : Function to allocate memory from mem mgmt system.	     */
/*	 calloc()   : Allocate and clear memory from mem mgmt system.	     */
/*	 realloc()  : Reallocate a packet				     */
/*	 free()	    : Function to free allocated memory.		     */
/*									     */
/*	_sys_memory : Array to contain all memory allocate by system.	     */
/*	 sys_free   : Pointer to free list				     */
/*									     */
/*	 free_memory() : Return total amount of available free memory.	     */
/*	 max_free() : Return largest single free memory block in heap.	     */
/*									     */
/*****************************************************************************/

/*****************************************************************************/
/*  DEBUG Support.							     */
/*  When the symbol DEBUG has been #defined, an extra WORD is added to the   */
/*  allocation header, and is set to the value 0xDEAD in every allocated and */
/*  free block of memory.  The integrety of the heap can then be checked by  */
/*  calling the function "void * chkheap()". This function returns zero if   */
/*  all the memory blocks have the correct signature.  Otherwise, it returns */
/*  the offset to the first location of an invalid value in a block header.  */
/*****************************************************************************/

#undef _INLINE				/* DISABLE INLINE EXPANSION	    */

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <_lock.h>
#include <limits.h>

#ifdef DEBUG
#define GUARDWORD 0xDEAD
#endif

/*****************************************************************************/
/* Declare the memory pool as a .usect called .esysmem.	The size of the	     */
/* section .esysmem is determined by the linker via the -heap option	     */
/*****************************************************************************/
#pragma DATA_SECTION(_sys_memory, ".esysmem")
int _sys_memory[];

/*****************************************************************************/
/* "PACKET" is the template for a data packet.  "Packet_size" is the number  */
/* of words allocated for the user, excluding the size ("OVERHEAD")	     */
/* required for management of the packet.  When a packet is on the free	     */
/* list, the field "next_free" is a pointer to the next member of the	     */
/* freelist; when the packet is in use (allocated), this field is invalid,   */
/* overwritten by user data (the address of this field is the address	     */
/* returned by malloc).  A negative size indicates a free packet.	     */
/*****************************************************************************/
typedef long memsz_t;
#define MEMSZ_MAX  LONG_MAX

typedef struct fpack
{
    memsz_t packet_size; /* in words */
#ifdef DEBUG
    int guard;
#endif
    struct fpack *next_free;
} PACKET;

#define LIMIT	((PACKET *) -1)

/*****************************************************************************/
/* OVERHEAD is the space in the packet required to store the packet size.    */
/* This macro is used for two purposes: to compute the size of the	     */
/* allocation, and to ensure the packet is aligned properly.		     */
/*****************************************************************************/
#define OVERHEAD offsetof(PACKET, next_free)

/*****************************************************************************/
/* MINSIZE is the smallest packet we will allocate.  It is required that     */
/* (MINSIZE + OVERHEAD >= sizeof(PACKET)), else the packet won't be large    */
/* enough to store the bookkeeping needed to be placed in the free list.     */
/*****************************************************************************/
#define MINSIZE 2 

/*****************************************************************************/
/* _SYSMEM_SIZE is the symbol that linker defines as the size of heap.	     */
/* Access of that value from 'C' is done by taking the address of this symbol*/
/* The C55x linker sets this symbol to the number of *bytes* in the heap, so */
/* we must convert thus number to words right away.                          */
/*****************************************************************************/
extern int _SYSMEM_SIZE;
#define MEMORY_SIZE ((memsz_t)&_SYSMEM_SIZE)

/*****************************************************************************/
/* The list of free blocks						     */
/*****************************************************************************/
static PACKET *sys_free;

/*****************************************************************************/
/* The start of the heap						     */
/*****************************************************************************/
static PACKET *sys_base;

/*****************************************************************************/
/* The total size of the heap.						     */
/*****************************************************************************/
static memsz_t memsize;

/*****************************************************************************/
/* This flag tells malloc if the heap needs to be initialized	             */
/*****************************************************************************/
static int first_call = 1;


/*****************************************************************************/
/*                                                                           */
/*  CHECK_ALLOC_SIZE - Verify that an allocation of the requested size is    */
/*                     possible without an overflow during the process.      */
/*                                                                           */
/*****************************************************************************/
inline static int check_alloc_size(size_t size)
{
    /*-----------------------------------------------------------------------*/
    /* Make sure the value of size is small enough that we will not overflow */
    /* the memsz_t type in the malloc/realloc calculations.  SDSCM00049633.  */
    /*-----------------------------------------------------------------------*/
    if (size > (MEMSZ_MAX - OVERHEAD - MINSIZE - 1))
        return 0;
    return 1;
}

/*****************************************************************************/
/*									     */
/*  MINIT - This function performs the initial setup of the heap, and can    */
/*          be called by the user to completely reset the memory management  */
/*          system.				                             */
/*									     */
/*****************************************************************************/
void minit(void)
{
    _lock();
    memsize = MEMORY_SIZE;

    /*-----------------------------------------------------------------------*/
    /* To initialize the memory system, set up the free list to point to     */
    /* the entire heap, and initialize heap to a single empty packet.	     */
    /*-----------------------------------------------------------------------*/
    /* We may need to adjust the start of the heap to ensure that the	     */
    /* address of the field "next_free" is strictly aligned.		     */
    /*-----------------------------------------------------------------------*/
    if (((memsz_t)_sys_memory ^ OVERHEAD) & 1)
    {
	sys_free = (PACKET *) (_sys_memory + 1);
	--memsize;
    }
    else
    {
	sys_free = (PACKET *) _sys_memory;
    }
    if (memsize & 1) --memsize;

    sys_free->packet_size = -(memsize - OVERHEAD); /* NEGATIVE==FREE */
    sys_free->next_free	  = LIMIT;

#ifdef DEBUG
    sys_free->guard = GUARDWORD;
#endif
    sys_base = sys_free;

    first_call = 0; /* CLEAR THE FLAG */
    _unlock();
}

/*****************************************************************************/
/*									     */
/*  MALLOC - Allocate a packet of a given size, and return pointer to it.    */
/*									     */
/*****************************************************************************/
void *malloc(size_t size)
{
    memsz_t allocsize;
    PACKET *current, *next, *prev;

    if (check_alloc_size(size) == 0) return 0;

    allocsize = (memsz_t)size;

    if (allocsize == 0) return 0;

    /*-----------------------------------------------------------------------*/
    /* We may need to adjust the size of the allocation request to ensure    */
    /* that the address of the field "next_free" remains strictly aligned    */
    /* in all packets on the free list.					     */
    /*-----------------------------------------------------------------------*/
    if ((allocsize ^ OVERHEAD) & 1) ++allocsize;

    _lock();

    if (first_call) minit();

    current = sys_free;
    prev = 0;

    /*-----------------------------------------------------------------------*/
    /* Find the first block large enough to hold the requested allocation    */
    /*-----------------------------------------------------------------------*/
    while (current != LIMIT && -current->packet_size < allocsize)
    {
	prev = current;
	current = current->next_free;
    }

    if (current == LIMIT)
    {
        /*-------------------------------------------------------------------*/
        /* No block large enough was found, so return NULL.		     */
        /*-------------------------------------------------------------------*/
	_unlock();
	return 0;
    }

    if (-current->packet_size > (allocsize + OVERHEAD + MINSIZE))
    {
        /*-------------------------------------------------------------------*/
        /* The packet is larger than needed; split the block and mark the    */
        /* smaller-addressed block as used.  The smaller-addressed block     */
        /* was chosen as a way to ensure that freed blocks get recycled	     */
        /* before allocations are made from the large original free block.   */
        /* However, this may tend to increase the length of the free list    */
        /* search for a large enough block.				     */
        /*-------------------------------------------------------------------*/
        /* Knuth's algorithm 2.5a instead allocates the larger-addressed     */
        /* block to the user.  This tends to leave the largest free blocks   */
        /* at the beginning of the free list.  Knuth's 2.5a' uses a "rover"  */
        /* pointer to prevent small free blocks from being concentrated in   */
        /* any part of the list.					     */
        /*-------------------------------------------------------------------*/
	next = (PACKET *)((char *)current + allocsize + OVERHEAD);
	next->packet_size=current->packet_size+allocsize+OVERHEAD;/*NEG==FREE*/
#ifdef DEBUG
	next->guard = GUARDWORD;
#endif
	current->packet_size = allocsize; /* POSITIVE==IN USE */

	if (prev) prev->next_free = next;
	else      sys_free        = next;

	next->next_free = current->next_free;
    }
    else
    {
        /*-------------------------------------------------------------------*/
        /* Allocate the whole block and remove it from the free list.	     */
        /*-------------------------------------------------------------------*/
	if (prev) prev->next_free = current->next_free;
	else      sys_free        = current->next_free;

	current->packet_size = -current->packet_size; /* POSITIVE==IN USE */
    }

    _unlock();
    return &(current->next_free);
}

/*****************************************************************************/
/*									     */
/*  FREE - Return a packet allocated by malloc to free memory pool.	     */
/*									     */
/*****************************************************************************/
void free(void *userptr)
{
    PACKET *sysblock, *next, *prev;

    if (userptr == 0) return; /* HANDLE NULL POINTER */

    _lock();

    next = sys_free;
    prev = 0;
    sysblock = (PACKET *)((char *)userptr - OVERHEAD);

    /*-----------------------------------------------------------------------*/
    /* Search the free list for the *free* packets physically closest to     */
    /* the packet to be freed.  PREV is the closest free packet with a	     */
    /* smaller address, and NEXT is the closest free packet with a larger    */
    /* address.								     */
    /*-----------------------------------------------------------------------*/
    while (next < sysblock)
    {
	prev = next;
	next = next->next_free;
    }

    /*-----------------------------------------------------------------------*/
    /* Coallesce with next block if possible.				     */
    /*-----------------------------------------------------------------------*/
    if ((char *)sysblock + sysblock->packet_size + OVERHEAD == (char *)next)
    {
	sysblock->next_free = next->next_free;
	sysblock->packet_size += -next->packet_size + OVERHEAD;	/* POS==USED */
#ifdef DEBUG
	next->guard = 0;
#endif
    }
    else sysblock->next_free = next;	/* START TO PUT INTO LIST */

    if (prev)				/* ARE WE THE NEW HEAD OF THE LIST */
    {
        /*-------------------------------------------------------------------*/
        /* sysblock is not the head of the free list; try to coallesce with  */
        /* prev								     */
        /*-------------------------------------------------------------------*/
	if ((char *)prev - prev->packet_size + OVERHEAD == (char *)sysblock)
	{
	    prev->next_free = sysblock->next_free;
	    prev->packet_size += -sysblock->packet_size - OVERHEAD;/*NEG==FREE*/
#ifdef DEBUG
	    sysblock->guard = 0;
#endif
	}
	else
	{
	    prev->next_free = sysblock;
	    sysblock->packet_size = -sysblock->packet_size; /* NEGATIVE==FREE */
	}
    }
    else
    {
	sys_free = sysblock;
	sysblock->packet_size = -sysblock->packet_size; /* NEGATIVE==FREE */
    }
    _unlock();
}

/*****************************************************************************/
/*									     */
/*  REALLOC - Reallocate a packet to a new size.			     */
/*									     */
/*****************************************************************************/
void *realloc(void *userptr, size_t size)
{
    PACKET *next, *prev, *sysblock;
    memsz_t newsize;

    /*-----------------------------------------------------------------------*/
    /* Handle special cases						     */
    /*-----------------------------------------------------------------------*/
    if (check_alloc_size(size) == 0) return 0;

    newsize = (memsz_t)size;

    if (newsize == 0)
    {
	free(userptr);
	return 0;
    }

    if (userptr == 0) return malloc(newsize);

    /*-----------------------------------------------------------------------*/
    /* We may need to adjust the size of the allocation request to ensure    */
    /* that the address of the field "next_free" remains strictly aligned    */
    /* in all packets on the free list.					     */
    /*-----------------------------------------------------------------------*/
    if ((newsize ^ OVERHEAD) & 1) ++newsize;

    /*-----------------------------------------------------------------------*/
    /* Find the start of the system block containing the old allocation.     */
    /*-----------------------------------------------------------------------*/
    sysblock = (PACKET *)((char *)userptr - OVERHEAD);

    /*-----------------------------------------------------------------------*/
    /* Find the system block physically after SYSBLOCK on the heap.	     */
    /*-----------------------------------------------------------------------*/
    next = (PACKET *)((char *)userptr + sysblock->packet_size);

    /*-----------------------------------------------------------------------*/
    /* If we are growing the packet, check if we must revert to calling      */
    /* malloc and copying the data.  We must do so if:                       */
    /* 	   1) sysblock is the last block in the heap, or		     */
    /* 	   2) the next block is not free, or				     */
    /* 	   3) the next block is free but not big enough.		     */
    /*-----------------------------------------------------------------------*/
    _lock();
    if ((newsize > sysblock->packet_size) && 
	(((char *)next >= (char *)sys_base + memsize) ||
	 (next->packet_size > 0)                      ||	
	 ((next->packet_size < 0) &&
	  (newsize > sysblock->packet_size - next->packet_size + OVERHEAD))))
    {
	void *ptr;
	_unlock();
	ptr = malloc(newsize);
	if (ptr)
	{
	    memcpy(ptr, userptr, sysblock->packet_size);
	    free(userptr);
	}
	return ptr;
    }

    /*-----------------------------------------------------------------------*/
    /* Once we reach here, we know we can realloc in place.		     */
    /*-----------------------------------------------------------------------*/
    /* Search the free list for the *free* packets physically closest to     */
    /* the packet to be freed.  PREV is the closest free packet with a	     */
    /* smaller address, and NEXT is the closest free packet with a larger    */
    /* address.								     */
    /*-----------------------------------------------------------------------*/
    next = sys_free;
    prev = 0;

    while (next < sysblock)
    {
	prev = next;
	next = next->next_free;
    }

    if ((char *)next == (char *)userptr + sysblock->packet_size)
    {
        /*-------------------------------------------------------------------*/
        /* The system block immediately following SYSBLOCK is free.	     */
        /* Coalesce with it, then try to free the unused portion.	     */
        /*-------------------------------------------------------------------*/
#ifdef DEBUG
	next->guard = 0;
#endif
	if (sysblock->packet_size + -next->packet_size - newsize <= MINSIZE)
	{
            /*---------------------------------------------------------------*/
            /* The next block is completely absorbed.			     */
            /*---------------------------------------------------------------*/
	    if (prev) prev->next_free = next->next_free;
	    else      sys_free        = next->next_free;
	    sysblock->packet_size += -next->packet_size + OVERHEAD; /* SGN<0 */
	}
	else
	{
            /*---------------------------------------------------------------*/
            /* The next block has enough space left over to break off a new  */
            /* block and add it to the free list.			     */
            /*---------------------------------------------------------------*/
	    PACKET *nextnext = next->next_free;
	    int nextsize = -next->packet_size + sysblock->packet_size - newsize;

	    PACKET *newnext = (PACKET*)((char *)userptr + newsize);
	    newnext->packet_size = -nextsize; /* NEGATIVE==FREE */
	    newnext->next_free = nextnext;
#ifdef DEBUG
	    newnext->guard = GUARDWORD;
#endif
	    sysblock->packet_size = newsize; /* POSITIVE==IN USE */
	    if (prev) prev->next_free = newnext;
	    else      sys_free        = newnext;
	}
    }
    else
    {
        /*-------------------------------------------------------------------*/
        /* Shrink in place if there is enough extra to make a free block     */
        /*-------------------------------------------------------------------*/
	if (sysblock->packet_size - newsize >= OVERHEAD + MINSIZE)
	{
	    int nextsize = sysblock->packet_size - newsize - OVERHEAD;

	    PACKET *newnext = (PACKET*)((char *)userptr + newsize);
	    newnext->packet_size = -nextsize; /* NEGATIVE==FREE */
	    newnext->next_free = next;
#ifdef DEBUG
	    newnext->guard = GUARDWORD;
#endif
	    sysblock->packet_size = newsize; /* POSITIVE==IN USE */
	    if (prev) prev->next_free = newnext;
	    else      sys_free        = newnext;
	}
    }
    _unlock();
    return userptr;
}

/*****************************************************************************/
/*									     */
/*  CALLOC - Allocate a packet of a given size, set the data in the	     */
/*		 packet to nulls, and return a pointer to it.		     */
/*									     */
/*****************************************************************************/
void *calloc(size_t num, size_t size)
{
    size_t  i   = size * num;
    void   *ret = malloc(i);
    if (ret)
    {
	char *c = ret;
	while (i--) *c++ = 0;
    }
    return ret;
}


/*****************************************************************************/
/*									     */
/*  CHKHEAP - Check the integrety of the memory heap.  If the heap	     */
/*	      is corrupt, returns the address of the corrupt header, else    */
/*	      returns 0.  Always returns 0 in release version.		     */
/*									     */
/*****************************************************************************/
void *chkheap(void)
{
#ifdef DEBUG
    PACKET *pkt, *top;
    _lock();
    /* find the start of the heap */
    pkt = sys_base;
    top = (PACKET *)((char *)sys_base + memsize - sizeof(PACKET));

    while (pkt < top)
    {
	if (pkt->guard != GUARDWORD)
	{
	    _unlock();
	    return (void *) &pkt->guard;
	}

	if (pkt->packet_size > 0)
	    pkt = (PACKET *)((char *)pkt + pkt->packet_size + OVERHEAD);
	else
	    pkt = (PACKET *)((char *)pkt + -pkt->packet_size + OVERHEAD);
    }
    _unlock();
#endif

    return 0;
}

/*****************************************************************************/
/*									     */
/*  FREE_MEMORY - returns the total amount of free memory available for	     */
/*		  allocation.  The memory may be fragmented		     */
/*									     */
/*****************************************************************************/
int free_memory(void)
{
    struct fpack *ptr;
    int memsz = 0;

    _lock();
    ptr = sys_free;

    if (!first_call)	/* IF MEMORY IS INITIALIZED */
    {
	while (ptr != LIMIT)
	{
	    memsz -= ptr->packet_size;
	    ptr = ptr->next_free;
	}
    }
    _unlock();
    return memsz;
}

/*****************************************************************************/
/*									     */
/*  MAX_FREE - returns the size of the largest single block of memory	     */
/*	       available for allocation.				     */
/*									     */
/*****************************************************************************/
int max_free(void)
{
    struct fpack *ptr;
    int memsz = 0;

    _lock();
    ptr = sys_free;

    if (!first_call)	/* IF MEMORY IS INITIALIZED */
    {
	while (ptr != LIMIT)
	{
	    if (memsz > ptr->packet_size)
		memsz = ptr->packet_size;
	    ptr = ptr->next_free;
	}
    }
    _unlock();
    return -memsz;
}
