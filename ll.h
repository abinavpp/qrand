/* 
 * SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2017 Abinav Puthan Purayil
 * */

#ifndef _LLH
#define _LLH

struct ll_node {
	void *data;

	void *key;
	size_t nkey;

	struct ll_node *next;
};

extern struct ll_node *ll_add(struct ll_node **ref);

extern struct ll_node **ll_find(struct ll_node **head, 
		const void *key, size_t nkey);

extern struct ll_node *ll_repl(struct ll_node **head, 
		const void *key, size_t nkey);

extern int ll_rm(struct ll_node **ref);

extern int ll_rmall(struct ll_node **ref, 
		void (*free_data)(void *), void (*free_key)(void *));


#define LL_INIT_HEAD(head)	(head) = ll_add(NULL)

#define LL_ADD_KEY(node, _key, _nkey) \
	(node)->key = _key;		\
	(node)->nkey = (_nkey);

#define ll_for_ref(walk, from)	\
	for ((walk)=(from); (walk)&&*(walk); (walk)=&((*walk)->next))

#define ll_for_node(walk, from)	\
	for((walk)=(from); *(walk); (walk)=(walk)->next)
#endif
