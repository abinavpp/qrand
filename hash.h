/* 
 * SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2017 Abinav Puthan Purayil
 * */

#ifndef _HASHH
#define _HASHH

#include "ll.h"
#include "util.h"

struct ht_struct {
	/* size will be mod for hash_fn */
	size_t size;

	/* return hash % mod for n bytes */
	long (*hash_fn) 
		(const void *bytes, size_t n, size_t mod);

	struct ll_node **table;
};

extern long ht_geti(struct ht_struct *hs, const void *key, size_t nkey);

extern struct ll_node *ht_add(struct ht_struct *hs, 
		const void *key, size_t nkey);

extern struct ll_node *ht_addk(struct ht_struct *hs, 
		void *key, size_t nkey);

extern struct ll_node *ht_repl(struct ht_struct *hs, 
		const void *key, size_t nkey);

extern struct ll_node **ht_find(struct ht_struct *hs, 
		const void *key, size_t nkey);

extern int ht_rm(struct ht_struct *hs, struct ll_node **node);

extern int ht_rmall(struct ht_struct *hs,
		void (*free_data)(void *), void (*free_key)(void *));

#define HT_INIT_STRUCT(hs) \
	(hs) = (struct ht_struct *)zalloc(sizeof(struct ht_struct));

#define HT_INIT_TABLE(hs, _size) \
(hs)->size = (_size);						\
(hs)->table = (struct ll_node **)			\
zalloc	((hs)->size * sizeof(void *));

#define HT_INIT_HASHFN(hs, fn) \
	(hs)->hash_fn = (fn);

#endif
