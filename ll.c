/* 
 * SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2017 Abinav Puthan Purayil
 * */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "ll.h"

/* Adds node before (or at) ref so that (*ref) points to new allocation and
 * its "next" points the "old ref". If ref or (*ref) is NULL then a new
 * allocation for ref is returned .(Caller should assign data!) Returns a
 * pointer to the new zeroed allocation. */
struct ll_node *ll_add(struct ll_node **ref)
{
	struct ll_node *new;

	new = (struct ll_node *)zalloc(sizeof(struct ll_node));

	if (ref) {
		if (*ref) {
			new->next = (*ref);
		} else {
			new->next = NULL;
		}
		*ref = new;
	} else {
		new->next = NULL;
	}

	return new;
}

/* returns a ref to node found from head having the same key (of length
 * nkey) using memcmp(), returns NULL if not found */
struct ll_node **ll_find(struct ll_node **head, 
		const void *key, size_t nkey)
{
	struct ll_node **walk;

	if (!head || !(*head) ||
			!(key) || !(nkey))
		return NULL;

	ll_for_ref(walk, head) {
		if ((*walk)->nkey == nkey) {
			if (memcmp((*walk)->key, key, nkey) == 0)
				return walk;
		}
	}

	return NULL;
}

/* returns node if found with the same key (of length nkey), if not found
 * adds a node using ll_add and returns its allocation (note:it will be a
 * zeroed allocation) */
struct ll_node *ll_repl(struct ll_node **head, 
		const void *key, size_t nkey)
{
	struct ll_node **ref_target;

	ref_target = ll_find(head, key, nkey);

	if (ref_target) {
		return *ref_target;
	} else {
		return ll_add(head);
	}
}

/* Frees ll_node dbl-referenced by ref so that (*ref) points to
 * (*ref)->next. Returns the # of nodes deleted Make sure node's data and
 * key are freed prior to calling this function */
int ll_rm(struct ll_node **ref)
{
	struct ll_node *target;

	if (!ref || !(*ref))
		return 0;

	target = (*ref);
	(*ref) = (*ref)->next;

	return 1;
}

/* Frees all nodes, their data & key (using free_data and key) from ref.
 * *ref assigned to null, returns # of nodes deleted */
int ll_rmall(struct ll_node **ref,
		void (*free_data)(void *), void (*free_key)(void *))
{
	struct ll_node *cur, *next;
	int delcnt = 0;

	if (!ref || !(*ref))
		return 0;

	for (cur=*ref; cur; cur=next) {
		next = cur->next;
		if (free_data)
			free_data(cur->data);
		if (free_key)
			free_key(cur->key);
		free(cur);
		delcnt++;
	}

	*ref = NULL;
	return delcnt;
}
