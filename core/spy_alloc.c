

#define _UNIT_TEST_

#ifdef _UNIT_TEST_
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <assert.h>
#endif

#include "spy_alloc.h"
/*

#define spy_address		unsigned char 

#define spy_length		unsigned int  

#ifndef _FROM_CONFIG_
#define SPY_DEFAULT_POOL_SIZE  1024*64
#endif 

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



*/
spy_mb_t * spy_createpool(spy_length length)
{
	spy_mb_t * MemblockControl = (spy_mb_t *)malloc( sizeof( spy_mb_t ));

	if( length %4 )
	{
		length += (4 - ( length % 4 ) );
	}
	
	MemblockControl->start = (spy_mb_t *)malloc( length ) ;
	MemblockControl->end = MemblockControl->start + length ;
	MemblockControl->curent = MemblockControl->start ;
	MemblockControl->max = length ;
	MemblockControl->more = length ;
	MemblockControl->cur = MemblockControl ;
	MemblockControl->next = NULL ;
	MemblockControl->large = NULL ;

	return MemblockControl ;
}

void *spy_alloc( spy_mb_t *MemblockControl ,spy_length length )
{
// ALIGN may use ALIGN MACRO
	if( length %4 )
	{
		length += (4 - ( length % 4 ) );
	}
	if( length == 0 )
		return (void *)0;
	
	if( length > SPY_DEFAULT_POOL_SIZE  )
	{
		if( MemblockControl->large == NULL )
		{
			MemblockControl->large = ( spy_lb_t *) malloc( sizeof( spy_lb_t ) );
			MemblockControl->large->next == NULL ;
			MemblockControl->large->retaddr = ( spy_address *)malloc( length ) ;
			return (void *)MemblockControl->large->retaddr;
		}
	
		spy_lb_t *largeblock = MemblockControl->large ;
		while( largeblock->next != NULL )
		{
			largeblock = largeblock->next ;
		}
		largeblock->retaddr = ( spy_address *)malloc( length );
		largeblock->next = NULL ;
		return (void *)largeblock->retaddr;		
	}

	//____________________________________________________________________________________________
	void *retmem = MemblockControl->cur->curent ;
	if( MemblockControl->cur->more >= length )
	{

		MemblockControl->cur->curent += length ;
		MemblockControl->cur->more -= length ;
		return (void *)( retmem );
	}
	else
	{
		while( MemblockControl->cur->next != NULL )
		{
			if( MemblockControl->cur->more >= length )
			{

				MemblockControl->cur->curent += length ;
				MemblockControl->cur->more -= length ;
				return (void *)( retmem );
			}
			else
			{
				MemblockControl->cur = MemblockControl->cur->next ;
			}
		}

		if( MemblockControl->cur->next == NULL )
		{
			MemblockControl->cur->next = (spy_mb_t *)malloc( sizeof(spy_mb_t) );      // ÄÚŽæ¹ÜÀí¿éµÄÉêÇë
			MemblockControl->cur->next->start = (spy_address *)malloc(SPY_DEFAULT_POOL_SIZE );  // ÊµŒÊÄÚŽæ³ØµÄÉêÇë
			MemblockControl->cur = MemblockControl->cur->next ;
			MemblockControl->cur->curent = MemblockControl->cur->start ;
			MemblockControl->cur->end = MemblockControl->curent+SPY_DEFAULT_POOL_SIZE  ;
			MemblockControl->cur->max = SPY_DEFAULT_POOL_SIZE ;
			MemblockControl->cur->more = SPY_DEFAULT_POOL_SIZE  ;
			MemblockControl->cur->next = NULL ;


			retmem = MemblockControl->cur->curent ;
			MemblockControl->cur->curent += length ;
			MemblockControl->cur->more -= length ;
			MemblockControl->cur = MemblockControl ;
			return (void *)( retmem );

		}
	}
	return (void *)0 ;
}


void spy_freepool( spy_mb_t * FreeBlock )
{
	if( NULL == FreeBlock )
		return ;

	if ( FreeBlock->large != NULL )
	{	
		spy_lb_t * pLarge1 = FreeBlock->large ;
		spy_lb_t * pLarge2 = pLarge1->next ;

		assert( pLarge1 != NULL ) ;
		do
		{
			free( pLarge1->retaddr );
			free( pLarge1 );
			pLarge1 = pLarge2 ;
			if ( pLarge2 != NULL )
				pLarge2 = pLarge2->next ;
			else
				break;
		}while( pLarge2 == NULL );

	}
	spy_mb_t * P1  = FreeBlock ;
	spy_mb_t * P2 ;

	P2 = P1->next ;

	while( P1->next != NULL  )
	{
		free(P1->start);
		free(P1);
		P1 = P2 ;
		P2 = P2->next ;
	}
	free(P1->start);
	free(P1);
}

/*
#ifdef _UNIT_TEST_
int main(int argc, char* argv[])
{
	spy_mb_t *pool = spy_createpool(1024);
	int *P[1000] ;
	int i = 0 ;
	for( i = 0 ; i < 1000 ; i++ )
	{
		P[i] = (int *)spy_alloc(pool, 333 );
	}
	for( i = 0 ; i < 1000 ; i++ )
	{
		printf("0x%8x \n",P[i] );
	}
	
	//  alloc big block memory
	int *p = (int *)spy_alloc(pool,sizeof(int)*1024*32);
	p[100] = 20 ;
	printf("\n%d\n",p[100]);
	printf("%d\n",p[2041]);
	spy_freepool(pool);
	return 0;
}
#endif
*/

