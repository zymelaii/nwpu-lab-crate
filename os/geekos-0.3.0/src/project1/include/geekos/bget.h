/*

    Interface definitions for bget.c, the memory management package.

*/

#if defined (GEEKOS)

// Adapted for geekos: http://www.cs.umd.edu/~daveho/geekos/
// Original version of BGET downloaded from: http://www.fourmilab.ch/bget/
// $Revision: 1.3 $

// GeekOS changes are (mostly) confined to #if defined (GEEKOS)
// sections.

// Yes, we have prototypes :-)
#define PROTOTYPES

#endif // defined (GEEKOS)

#ifndef _
#ifdef PROTOTYPES
#define  _(x)  x		      /* If compiler knows prototypes */
#else
#define  _(x)  ()                     /* It it doesn't */
#endif /* PROTOTYPES */
#endif

typedef long bufsize;
void	bpool	    _((void *buffer, bufsize len));
void   *bget	    _((bufsize size));
void   *bgetz	    _((bufsize size));
void   *bgetr	    _((void *buffer, bufsize newsize));
void	brel	    _((void *buf));
void	bectl	    _((int (*compact)(bufsize sizereq, int sequence),
		       void *(*acquire)(bufsize size),
		       void (*release)(void *buf), bufsize pool_incr));
void	bstats	    _((bufsize *curalloc, bufsize *totfree, bufsize *maxfree,
		       long *nget, long *nrel));
void	bstatse     _((bufsize *pool_incr, long *npool, long *npget,
		       long *nprel, long *ndget, long *ndrel));
void	bufdump     _((void *buf));
void	bpoold	    _((void *pool, int dumpalloc, int dumpfree));
int	bpoolv	    _((void *pool));
