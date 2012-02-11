#ifndef _SPY_MINHEAP_INCLUDE_H_
#define _SPY_MINHEAP_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

typedef spy_uint_t spy_minheap_key_t;
typedef spy_int_t spy_minheap_key_int_t;
typedef size_t spy_minheap_index_t;

typedef struct spy_minheap_node_s spy_minheap_node_t;

struct spy_minheap_node_s {
	spy_minheap_key_t key;
	spy_minheap_index_t index;
};

typedef struct spy_minheap_s spy_minheap_t;

struct spy_minheap_s {
	size_t root; // 根节点索引
	size_t last; // 最末尾索引
	spy_uint_t max_num; // 最小堆节点最大个数
	spy_minheap_node_t **node; // 节点集合
};

#define spy_minheap_full(heap)	(((spy_minheap_t *)heap)->max_num == heap->last)

#define spy_minheap_empty(heap) (((spy_minheap_t *)heap)->last == 0)

#define spy_minheap_min(heap)  ((((spy_minheap_t *)heap)->last == 0) ? NULL :  \
		((spy_minheap_t *)heap)->node[1])

spy_int_t spy_minheap_insert(spy_minheap_t *heap, spy_minheap_node_t *node,
		size_t index);

spy_int_t spy_minheap_delete(spy_minheap_t *heap, size_t index);

void spy_minheap_init(spy_minheap_t *heap, spy_uint_t max_num);

#endif
