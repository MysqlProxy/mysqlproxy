#include <spy_config.h>
#include <spy_core.h>

void spy_minheap_init(spy_minheap_t *heap, spy_uint_t max_num) {
	heap->root = 0;
	heap->last = 0;
	heap->max_num = max_num;
	heap->node = spy_alloc(sizeof(spy_minheap_node_t *) * 10, spy_global->log);
}

spy_int_t spy_minheap_insert(spy_minheap_t *heap, spy_minheap_node_t *node,
		size_t index) {

	size_t parent;

	// 没满的heap
	if (spy_minheap_full(heap)) {
		return SPY_ERROR;
	}

	heap->last++;
	//child = heap->last++;
	parent = (index - 1) / 2;
	// 和父节点比较，直到根节点
	while (index && (node->key < heap->node[parent]->key)) {
		// 子父交换，上移
		(heap->node[index] = heap->node[parent])->index = index;
		index = parent;
		parent = (index - 1) / 2;
	}

	(heap->node[index] = node)->index = index;

	return SPY_OK;
}

spy_int_t spy_minheap_delete(spy_minheap_t *heap, size_t index) {

	spy_uint_t child;

	// 非空的heap
	if (spy_minheap_empty(heap)) {
		return SPY_ERROR;
	}

	heap->last--;
	// 左节点
	child = 2 * index + 1;
	// 非最后一个节点
	while (child < heap->last) {
		// 左节点，还是右节点
		if (child < heap->last - 1 && heap->node[child]->key > heap->node[child
				+ 1]->key)
			child++;
		// 最末节点比child小或者等于直接跳出
		if (heap->node[heap->last]->key <= heap->node[child]->key)
			break;
		// 父子交换，下移
		(heap->node[index] = heap->node[child])->index = index;
		// 左节点
		index = child;
		child = index * 2 + 1;
	}

	(heap->node[index] = heap->node[heap->last])->index = index;

	return SPY_OK;
}

#ifdef _SPY_MINHEAP_UNIT_TEST_

int main() {

	int i;
	spy_minheap_t heap;

	heap.root = 0;
	heap.last = 0;
	heap.max_num = 5;
	heap.node = malloc(sizeof(spy_minheap_node_t *) * 5);

	spy_minheap_node_t root_node;
	root_node.key = 20;

	spy_minheap_node_t second_node;
	second_node.key = 15;

	spy_minheap_node_t thrid_node;
	thrid_node.key = 2;

	spy_minheap_node_t fourth_node;
	fourth_node.key = 14;

	spy_minheap_node_t fifth_node;
	fifth_node.key = 10;

	spy_minheap_node_t sixth_node;
	sixth_node.key = 21;

	spy_minheap_insert(&heap, &root_node, heap.last);
	spy_minheap_insert(&heap, &second_node, heap.last);
	spy_minheap_insert(&heap, &thrid_node, heap.last);
	spy_minheap_insert(&heap, &fourth_node, heap.last);
	spy_minheap_insert(&heap, &fifth_node, heap.last);
	spy_minheap_insert(&heap, &sixth_node, heap.last);

	//spy_minheap_delete(&heap);

	for (i = heap.root; i < heap.last; i++) {
		spy_log_stdout("index : %d, key : %d", heap.node[i]->index,
				heap.node[i]->key);
	}

	spy_log_stdout("last : %d", heap.last);
	spy_log_stdout("--------------");
	//spy_minheap_replace(&heap, 3, 23);

	spy_minheap_delete(&heap, 3);

	for (i = heap.root; i < heap.last; i++) {
		spy_log_stdout("index : %d, key : %d", heap.node[i]->index,
				heap.node[i]->key);
	}

	spy_log_stdout("last : %d", heap.last);

	exit(EXIT_SUCCESS);
}

#endif
