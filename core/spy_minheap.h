#ifndef _SPY_MINHEAP_INCLUDE_H_
#define _SPY_MINHEAP_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

typedef spy_uint_t spy_minheap_key_t;

typedef struct spy_minheap_node_s spy_minheap_node_t;

struct spy_minheap_node_s {
	spy_minheap_key_t key;
};

typedef struct spy_minheap_s spy_minheap_t;

struct spy_minheap_s {
	spy_uint_t root; // 根节点索引
	spy_uint_t last; // 最末尾索引
	spy_uint_t max_num; // 最小堆节点最大个数
	spy_minheap_node_t *node; // 节点集合
};

#define spy_minheap_full(heap)	(heap->max_num - 1 == heap->last)

#define spy_minheap_empty(heap) (heap->last <= 1)

void spy_minheap_init(spy_uint_t number);

spy_int_t spy_minheap_insert(spy_minheap_t *heap, spy_minheap_node_t node);

spy_int_t spy_minheap_delete(spy_minheap_t *heap);

spy_int_t
spy_minheap_replace(spy_minheap_t *heap, size_t index, spy_uint_t new);

#define spy_minheap_init(tree, s, i)                                           \
    ngx_rbtree_sentinel_init(s);                                              \
    (tree)->root = s;                                                         \
    (tree)->sentinel = s;                                                     \
    (tree)->insert = i

#endif
