/* 
 * SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2017 Abinav Puthan Purayil
 * */

#ifndef QRANDH
#define QRANDH

#include "util.h"

#define UNDEF	-1

#define SIMQ	1 /* simple question */

#define QMAX_NUMQ	1024 /* max # of questions in i/p file */
#define QMAX_LENQ	512 /* max len of a question */
#define QMAX_LOOP	65535 /* max # of iter of rand() calls */

#define DEBUG_ERRSTR(str)	"Error: "__FILE__ ":%d " str "\n", __LINE__

#define QARR_INIT(q) \
	(q) = (char **)umalloc(sizeof(char *) * QMAX_NUMQ);	\
	alloc_strarr((q), QMAX_NUMQ, QMAX_LENQ);

/* do option flags set during getopt */
#define DOOPT_FREQ		1
#define DOOPT_QSKIP_NEW	2
#define DOOPT_QSKIP_OLD	4
#define DOOPT_DUP_LIM	8
#define DOOPT_QSKIP_UPD	16

/* does one "transaction", checked at qrand_decide() */
#define DOOPTSET_SIMPLE	(DOOPT_FREQ)
#define DOOPTSET_QSKIP_NEW \
	(DOOPT_FREQ | DOOPT_QSKIP_NEW)
#define DOOPTSET_QSKIP_OLD \
	(DOOPT_FREQ | DOOPT_QSKIP_OLD)

#endif
