#include "nanoMPK.h"
#include <string.h> //for memset
#ifndef USE_INNER_MALLOC
#include <malloc.h>
#endif

#define TABLE_SIZE 0x4000
typedef struct _HashEntry {
	int key;
	mpk_node value;
} HashEntry;

HashEntry *mmap_table;
int *pkey_arr;
char *table;

void alloc_hash(void) {
  int i = 0;
#ifdef USE_INNER_MALLOC
	table = mpk_malloc(TABLE_SIZE * sizeof(HashEntry));
#else
    table = (char*)malloc(TABLE_SIZE * sizeof(HashEntry));
#endif
  memset(table, -1, 0x1000);
  pkey_arr = (int *) table;
  mmap_table = (HashEntry *)(table + 0x1000);
	for(i = 0; i < TABLE_SIZE; i++) {
		mmap_table[i].key = -1;
		memset(&mmap_table[i].value, 0, sizeof(mpk_node));
    mmap_table[i].value.pkey = -1;
  }
}
   
mpk_node* hash_get(int key) {
	int hash = (key % TABLE_SIZE);
  //printk("key : %d, %d\n", mmap_table[hash].key, key);
	while (mmap_table[hash].key != -1 && mmap_table[hash].key != key)
		hash = (hash + 1) % TABLE_SIZE;
	if (mmap_table[hash].key == -1)
		return NULL;
	else
		return &mmap_table[hash].value;
}

void hash_put(int key, mpk_node* value) {
	int hash = (key % TABLE_SIZE);
	while (mmap_table[hash].key != -1 && mmap_table[hash].key != key)
		hash = (hash + 1) % TABLE_SIZE;
/*	
 *	if (table[hash].key != -1) {
		table[hash].key = -1;
		table[hash].value = NULL;
	}	
  */
	mmap_table[hash].key = key;
  memcpy(&mmap_table[hash].value, value, sizeof(mpk_node));
//	table[hash].value = value;
}

