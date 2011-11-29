#ifndef _IF_INCLUDE_SPY_ALLOCLARGE_
#define _IF_INCLUDE_SPY_ALLOCLARGE_

#include "spy_alloc.h"
#define ssize_t unsigned int


#define OCUP 1

#define EMPTY 0




#define DEFAULT_ALING   	1024


//#define ALIGN(x,a)    ( ( (x)+(a)-1) &~ ((a)-1) ) 


#define DEFAULT_POOL_SIZE_PAR    1024

enum { DEFAULT_POOL_SIZE = 1024 };

#define spy_address		unsigned char 

#define spy_length		unsigned int  

#ifndef _FROM_CONFIG_
#define SPY_DEFAULT_POOL_SIZE  1024*64
#endif 

typedef struct spy_smallalloc_control_t
{
	spy_address			  * RetAddr 	;
	struct spy_smallalloc_control_t   * next 	;
	int				    Flag	;
}spy_sac_t;

typedef struct spy_bigalloc_control_t
{
	spy_sac_t                            * sacindex[DEFAULT_POOL_SIZE+1];
	long long 			       TotalAlloced ;
	int		                       AllocedInfo[DEFAULT_POOL_SIZE+1];
	spy_mb_t		             * spymbinbc ;
}spy_bac_t;

spy_bac_t * spy_largecreatpool( void ) ;

void * spy_largeAlloc(spy_bac_t  * BCHandler , ssize_t Asize);


#endif
