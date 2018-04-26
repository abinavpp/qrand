/* 
 * SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2017 Abinav Puthan Purayil
 * */

#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "ll.h"
#include "util.h"
#include "hash.h"

/* Returns hash index for key of size nkey by using hs->hash_fn */
long ht_geti(struct ht_struct *hs, const void *key, size_t nkey)
{
	long h_index;

	if (!hs || !hs->size || !hs->table ||
			!nkey || !key)
		return -1;

	h_index = hs->hash_fn(key, nkey, hs->size);
	return h_index;
}

/* Returns a new ll_node entry for key of size nkey */
struct ll_node *ht_add(struct ht_struct *hs, 
		const void *key, size_t nkey)
{
	long h_index;

	h_index = ht_geti(hs, key, nkey);
	if (h_index == -1)
		return NULL;

	return ll_add(&(hs->table[h_index]));
}

/* calls ht_add and assigns the new node's key and nkey */
struct ll_node *ht_addk(struct ht_struct *hs, 
		 void *key, size_t nkey)
{
	struct ll_node *new;

	if ((new = ht_add(hs, key, nkey))) {
		LL_ADD_KEY(new, key, nkey);
	}
	return new;
}

/* Does the same as ht_add except if the key is occupied then it returns
 * that ll_node instead of creating a new one */ 
struct ll_node *ht_repl(struct ht_struct *hs, 
		const void *key, size_t nkey)
{
	long h_index;

	h_index = ht_geti(hs, key, nkey);
	if (h_index == -1)
		return NULL;

	return ll_repl(&(hs->table[h_index]), key, nkey);
}



/* Returns ref to node with its key=key of size nkey, only returns for
 * one node, hence it works as expected with ht_repl() but not with
 * ht_add(). Returns NULL if not found */
struct ll_node **ht_find(struct ht_struct *hs, 
		const void *key, size_t nkey)
{
	struct ll_node **walk;
	long h_index;

	h_index = ht_geti(hs, key, nkey);
	if (h_index == -1)
		return NULL;

	return ll_find(&hs->table[h_index], key, nkey);
}

/* Removes ll_node referenced by node, make sure its data and key
 * are freed prior to calling this function.*/
int ht_rm(struct ht_struct *hs, struct ll_node **node)
{
	return ll_rm(node);
}

/* Removes all hs->size buckets using free_data and free_key 
 * functions from hs */
int ht_rmall(struct ht_struct *hs,
		void (*free_data)(void *), void (*free_key)(void *))
{
	size_t i;

	if (!hs)
		return 0;

	for (i = 0; i < hs->size; i++) {
		if (hs->table[i]) {
			ll_rmall(&hs->table[i], free_data, free_key);
		}
	}
}
