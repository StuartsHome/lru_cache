#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/hashmap.h"
#include <stdbool.h>
#include <assert.h>

/*
 * Each node contains one value and one pointer.
 * The pointer always points to the next node of the list.
 * If the pointer is NULL, then it is the last node. 
 * */
struct node {
	int key;
	int val;
	struct node * next;
	struct node * prev;
};

struct LRUCache {
	int capacity;
	struct node * head;
	struct node * tail;
	struct HashMap* map;
};

// node_t* CreateNode() {
//	node_t * new_node;
//	new_node = (node_t *) malloc(sizeof(node_t));
//	return new_node;
//}
//

void unlink_node(struct LRUCache* obj, struct node* new_node) {
	new_node->prev->next = new_node->next;
	new_node->next->prev = new_node->prev;
}

void add_head(struct LRUCache* obj, struct node* new_node) {
	struct node* temp = NULL;
	temp = obj->head->next;

	obj->head->next = new_node;
	new_node->prev = obj->head;
	new_node->next = temp;
	temp->prev = new_node;
}

void remove_tail(struct LRUCache* obj) {
	int size = get_size_hashmap(obj->map);
	if (size == 0) {
		return;
	}
	struct node* tail = obj->tail->prev;
	remove_hashmap(obj->map, tail->key);
	unlink_node(obj, tail);
}

struct LRUCache* lRUCacheCreate(int cap, HashFunction hash, CompareFunction compare) {
	struct LRUCache *cache = (struct LRUCache *) malloc(sizeof(struct LRUCache));

	struct node *hd = (struct node *) malloc(sizeof(struct node));
	struct node *tl = (struct node *) malloc(sizeof(struct node));


	// Hash map.
	struct HashMap* map = new_hashmap(hash, compare);

	hd->key = -2;
	hd->val = -2;
	tl->key = -2;
	tl->val = -2;

	hd->next = tl;
	tl->prev = hd;

	cache->capacity = cap;
	cache->head = hd;
	cache->tail = tl;
	cache->map = map;
	
	return cache;
}

void print_node(struct node* new_node) {
	if (new_node == NULL) {
		printf("print_node, new_node == NULL\n");
	}

	printf("print_node, new_node->key %d\n", new_node->key);
	printf("print_node, new_node->val %d\n", new_node->val);
	printf("print_node, new_node->prev->val %d\n", new_node->prev->val);
	printf("print_node, new_node->next->val %d\n", new_node->next->val);
}

int lRUCacheGet(struct LRUCache* obj, int key) {
	print_hashmap(obj->map);

	// Check if key in map.
	void* v_val = get_hashmap(obj->map, key);
	if (v_val == NULL) {
		return -1;
	}

	struct node* new_node = NULL;
	//new_node = (struct node *) malloc(sizeof(struct node));
	new_node = (struct node*)v_val;

	if (new_node->val == 0) {
		return -1;
	}

	// Unlink node.
	unlink_node(obj, new_node);
	
	// Add head.
	add_head(obj, new_node);
	
	return new_node->val;
}

// Add new item to the cache.
void lRUCachePut(struct LRUCache* obj, int key, int value) {
	void* got = get_hashmap(obj->map, key);

	// If key not in the map.
	if (got == NULL) {
		// Number of buckets.
		int size = get_size_hashmap(obj->map);
		if (size >= obj->capacity) {
			// Remove tail.
			remove_tail(obj);
		}
		// Create a new node.
		struct node * new_node = NULL;
		new_node = (struct node *) malloc(sizeof(struct node));
		new_node->val = value;
		new_node->key = key;
	
		// Add the new node to the map.
		insert_hashmap(obj->map, key, new_node);

		// Add head.
		add_head(obj, new_node);

		print_node(new_node);

		return;
	}

	// Cast void pointer to node.
	struct node* got_node;
	got_node = (struct node*)got;
	print_node(got_node);

	got_node->val = value;
	got_node->key = key;
	
	// Add the new node to the map.
	insert_hashmap(obj->map, key, got_node);
	print_node(got_node);

	lRUCacheGet(obj, key);
}

void LRUCacheFree(struct LRUCache* obj) {
}
// Hashmap hash function.
size_t hash(int key) {
	return key % 32;
}

// Hashmap compare function.
bool compare(int key1, int key2) {
	return (key1 == key2);	
}

int main() {
	// New cache.
	struct LRUCache* obj;
	obj = lRUCacheCreate(2, hash, compare);
	
	lRUCachePut(obj, 1, 1);

	lRUCachePut(obj, 2, 2);
	assert(lRUCacheGet(obj, 1) == 1);

	lRUCachePut(obj, 3, 3);				// Capacity is 2, this Put evicts key 2.
	assert(lRUCacheGet(obj, 2) == -1); 	// Not found.
	lRUCachePut(obj, 4, 4);				// Evicts key 1.

	assert(lRUCacheGet(obj, 1) == -1);	// Not found.				
	assert(lRUCacheGet(obj, 3) == 3);
	assert(lRUCacheGet(obj, 4) == 4);

	printf("FINISHED\n");
	return 0;
}

