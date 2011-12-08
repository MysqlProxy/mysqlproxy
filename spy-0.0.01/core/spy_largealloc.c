
#define _UNIT_TEST_
#define _DEBUG_
#ifdef _UNIT_TEST_
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <assert.h>
#endif

#include "spy_alloc.h"
#include "spy_largealloc.h"



//  return handler of bigalloccontrol
spy_bac_t * spy_largecreatpool( void )
{
	if( SPY_DEFAULT_POOL_SIZE <= 1 || SPY_DEFAULT_POOL_SIZE > 2048 )
	{
	#ifdef	__LINUX__POSIX__
		fprintf(stderr,"Why donT you Only use small Memcontrol or go dead ???\n");
		return NULL ;
	#endif
	}
	spy_mb_t * inlinepool = spy_createpool( sizeof(spy_bac_t)+3*sizeof(spy_sac_t)*(DEFAULT_POOL_SIZE+1) );
	
	spy_bac_t * nbc = ( spy_bac_t *)spy_alloc( inlinepool , sizeof( spy_bac_t) );
	nbc->spymbinbc = inlinepool ;
	

	int i = 0 ;
	unsigned int Allocsize = 0 ;
	int index = DEFAULT_POOL_SIZE ;
	
	for(  i = 0 ; i <= (index&~3) ; i+=4 )
	{
		nbc->AllocedInfo[i]   = 0 ;
		nbc->AllocedInfo[i+1] = 0 ;
		nbc->AllocedInfo[i+2] = 0 ;
		nbc->AllocedInfo[i+3] = 0 ;
	}

	for ( i = (index&~3); i < index ; i++)
	{
		nbc->AllocedInfo[i] = 0 ;
	}

	#ifdef _DEBUG_
		fprintf(stderr,"\nnow start to run sacindex[i]\n");
		fprintf(stderr,"index:%d\n",index);
	#endif
	for( i = 1 ; i<= index ; i++ )
	{


		Allocsize = i<<11 ;
	#ifdef _DEBUG_
		fprintf(stderr,"%d\n",Allocsize);
	#endif
		//  alloced from pool
		nbc->sacindex[i] = ( spy_sac_t *)spy_alloc( inlinepool , sizeof( spy_sac_t ) );
		//  alloced from system
		nbc->sacindex[i]->RetAddr = ( spy_address * )malloc( Allocsize ) ;
	
		nbc->sacindex[i]->Flag = EMPTY ;
		//  alloced from pool
		nbc->sacindex[i]->next =  ( spy_sac_t *)spy_alloc( inlinepool ,  sizeof( spy_sac_t ) );
		//  alloced from system
		nbc->sacindex[i]->next->RetAddr = ( spy_address * )malloc( Allocsize ) ;

		nbc->sacindex[i]->next->Flag = EMPTY ;

		nbc->sacindex[i]->next->next = NULL  ;

	}

	nbc->TotalAlloced = 0 ;

	return nbc ;
}

/*
typedef struct spy_largeblock_t
{
	spy_address *retaddr;
	struct spy_largeblock_t *next
}spy_lb_t;

typedef struct spy_Memblock_t
{
	spy_address    *start ;       	//start address
	spy_address    *end   ;		//end address
	spy_address    *curent;		//current address
	spy_length     max    ;		//max size of this pool list node 
	spy_length     more   ;		//size of remain
	spy_lb_t       *large ;         //point to large block memory
	struct spy_Memblock_t *next ; 	//next node of pool list
	struct spy_Memblock_t *cur  ;  	//point to current pool node
}spy_mb_t;

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


*/

void * spy_largeAlloc(spy_bac_t  * BCHandler , ssize_t Asize)
{
	if( BCHandler == NULL )
#ifdef	_DEBUG_
		fprintf(stderr,"Why donT you Only use small Memcontrol or go dead ???\n");
		return (void *)0; 
#endif


	ssize_t AllocSize = Asize ;
	
// ALIGN MARCO can be used
	AllocSize = AllocSize % 1024 ? (AllocSize + (1024 - AllocSize % 1024 )) : AllocSize ;

	int index =AllocSize>>10 ;

	spy_sac_t * pSC = BCHandler->sacindex[index];
	while( pSC->next != NULL  )
	{
		if( pSC->Flag == EMPTY )
		{
			pSC->Flag = OCUP ;
			BCHandler->TotalAlloced += AllocSize ;
			BCHandler->AllocedInfo[index]++;
			return (void *)(pSC->RetAddr);
		}
		pSC = pSC->next;
	}

	pSC->next = ( spy_sac_t *) malloc( sizeof( spy_sac_t ) );
	pSC->next->Flag = OCUP ;
	pSC->next->RetAddr = (spy_address *)malloc( AllocSize );
	pSC->next->next = NULL ;
	BCHandler->TotalAlloced += AllocSize ;
	BCHandler->AllocedInfo[index]++;
	return (void*)(pSC->next->RetAddr);
}


#if 0
int main()
{
	spy_bac_t *lbc = spy_largecreatpool();
	int *p = (int *)spy_largeAlloc(lbc,1);
	int *p2 = (int *)spy_largeAlloc(lbc,1024*128);
	printf("%x\n",lbc);
	return 0;
}
#endif












