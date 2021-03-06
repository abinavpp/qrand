/* 
 * SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2017 Abinav Puthan Purayil
 * */

%{
	#include <stdio.h>
	#include "qrand.h"
%}

BEGIN_SIMQ	<q
END_SIMQ	q>

%x simq

%%
	/* Inside questions */

{BEGIN_SIMQ}	{
	BEGIN(simq);
}

<simq>. yymore(); /* .* can match beyond q> */

<simq>\n yymore(); /* since . = sigma-'\n' */

<simq>{END_SIMQ}	{
	BEGIN(INITIAL); /* revert to normal state */
	yyleng -= 2; /* removes trailing q> */
	return SIMQ;
}

	/* Outside questions */

[ \t\n]*	;

.			;

%%
