#ifndef _IF_INCLUDE_SPY_ALLOC_
#define _IF_INCLUDE_SPY_ALLOC_
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

spy_mb_t * spy_createpool(spy_length length);

void *spy_alloc( spy_mb_t *MemblockControl ,spy_length length ) ;

void spy_freepool( spy_mb_t * FreeBlock ) ;
#endif
