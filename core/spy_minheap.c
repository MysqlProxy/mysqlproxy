#include <spy_config.h>
#include <spy_core.h>

spy_int_t spy_minheap_insert(spy_minheap_t *heap, spy_minheap_node_t node) {

	int i;
	if (spy_minheap_full(heap)) {
		return SPY_ERROR;
	}

	i = ++(heap->last);
	// 和父节点比较，直到根节点
	while ((i != 1) && (node.key < heap->node[i / 2].key)) {
		// 子父交换，上移
		heap->node[i] = heap->node[i / 2];
		i /= 2;
	}
	heap->node[i] = node;
	return SPY_OK;
}

spy_int_t spy_minheap_delete(spy_minheap_t *heap) {

	spy_uint_t parent, child;
	spy_minheap_node_t temp;
	if (spy_minheap_empty(heap)) {
		return SPY_ERROR;
	}

	temp = heap->node[heap->last--]; // 取最后一个节点
	parent = 1;
	child = 2;

	// 非最后一个节点
	while (child <= heap->last) {
		// 左节点，还是右节点
		if (child < heap->last && heap->node[child].key
				> heap->node[child + 1].key)
			child++;

		// 最末节点比child小或者等于直接跳出，比如只有一个节点的情况
		if (temp.key <= heap->node[child].key)
			break;

		// 父子交换，下移
		heap->node[parent] = heap->node[child];
		parent = child;
		child *= 2;
	}

	heap->node[parent] = temp;
	return SPY_OK;
}

spy_int_t spy_minheap_replace(spy_minheap_t *heap, size_t index, spy_uint_t new) {

	size_t i;

	if (index >= heap->max_num - 1) {
		return SPY_ERROR;
	}

	i = index;

	while ((i != 1) && (new < heap->node[i / 2].key)) {
		heap->node[i] = heap->node[i / 2];
		i /= 2;
	}

	while ((i <= heap->last) && (new > heap->node[i * 2].key)) {
		heap->node[i] = heap->node[i * 2];
		i *= 2;
	}

	heap->node[i].key = new;

	return SPY_OK;
}


#ifdef _SPY_MINHEAP_UNIT_TEST_

int main() {

	int i;
	spy_minheap_t heap;

	heap.root = 1;
	heap.last = 1;
	heap.max_num = 10;
	heap.node = malloc(sizeof(spy_minheap_node_t) * 10);

	for (i = 0; i < heap.max_num; i++) {
		heap.node[i].key = 0;
	}

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

	spy_minheap_insert(&heap, root_node);
	spy_minheap_insert(&heap, second_node);
	spy_minheap_insert(&heap, thrid_node);
	spy_minheap_insert(&heap, fourth_node);
	spy_minheap_insert(&heap, fifth_node);
	spy_minheap_insert(&heap, sixth_node);

	//spy_minheap_delete(&heap);

	for (i = 0; i < heap.max_num; i++) {
		spy_log_stdout("%2d", heap.node[i].key);
	}

	spy_log_stdout("--------------");
	spy_minheap_replace(&heap, 3, 23);

	for (i = 0; i < heap.max_num; i++) {
		spy_log_stdout("%2d", heap.node[i].key);
	}

	spy_log_stdout("last : %d", heap.last);

	exit(EXIT_SUCCESS);
}

#endif
