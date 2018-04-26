/* 
 * SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2017 Abinav Puthan Purayil
 * */

#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>

#include "qrand.h"
#include "util.h"
#include "hash.h"

/* Global flex variables */
extern int yylex();
extern char *yytext;
extern FILE *yyin;
extern size_t yyleng;

/* to be filled at first from cmdline info, acts like a global entity
 * in this file */
struct qrand_info {

	/* info from cmdline */
	size_t out_dup_lim; /* limit of duplicates from file p_qskip_old */
	size_t out_freq; /* o/p freq to stdout */

	/* file names */
	char *p_qbank;
	char *p_qskip_new;
	char *p_qskip_old; 

	/* arrays */
	char **qbank_arr; /* array of qs from file p_qbank */
	size_t qbank_nq;/* qbank_nq : # of qs in p_qbank */

	char **qskip_old_arr; /* ^^^ for qskip file p_qskip_old */
	size_t qskip_old_nq;
};

/* allocates n len sized allocation for arr[0],..arr[n] respectively */
static void alloc_strarr(char **arr, size_t n, size_t len)
{
	size_t i;

	for (i=0; i<n; i++) {
		arr[i] = (char *)umalloc(len);
	}
}

/* hash table for various qarrs */
static struct ht_struct *qarr_ht_struct;

/* all keys in hash are a stream of bytes stripped of white-spaces and
 * lower-cased to relax question search */

static struct ll_node **qarr_hash_find(const char *key)
{
	size_t i, j;
	char _key[QMAX_LENQ];

	if (!qarr_ht_struct) {
		return NULL;
	}

	/* strip white-space and force lower-case */
	for (i=0, j=0; key[i]; i++) {
		if (!isspace(key[i])) {
			_key[j++] = tolower(key[i]);
		}
	}
	_key[j] = 0;

	return ht_find(qarr_ht_struct, _key, j);
}

static void qarr_hash_add(const char *key)
{
	size_t i, j;
	char *_key = (char *)malloc(QMAX_LENQ);
	struct ll_node *new;

	if (!qarr_ht_struct) {
		HT_INIT_STRUCT(qarr_ht_struct);
		HT_INIT_TABLE(qarr_ht_struct, QMAX_NUMQ);
		qarr_ht_struct->hash_fn = hash_simfn;
	}

	/* strip white-space and force lower-case */
	for (i=0, j=0; key[i]; i++) {
		if (!isspace(key[i])) {
			_key[j++] = tolower(key[i]);
		}
	}
	_key[j] = 0;

	new = ht_add(qarr_ht_struct, _key, j);
	new->key = _key;
	new->nkey = j;
}

/* uniq[i] == 0 ? index i not used : index i is used.
 * Returns a random index < nuniq which is not "used" as per the
 * uniq arr of size nuniq, updates uniq before returning index.
 */
static size_t rand_uniq_index(uint8_t *uniq, size_t nuniq) {

	size_t index = 0, i;

	srand(getpid());

	/* try LOOP_MAX times to get a uniq rand # */
	for (i = 0; i < QMAX_LOOP; i++) {
		index = rand() % nuniq;

		if (uniq[index] == 0) {
			uniq[index] = 1;
			goto ret;
		}
		/* try again */
	}

	/* murphy's law */
	for (index = 0; index < nuniq; index++) {
		if (uniq[index] == 0) {
			uniq[index] = 1;
			goto ret;
		}
	}
ret:
	return index;
}

/* enforces (relaxed by -l) skip output to stdout and fd */
#define QRAND_OUT_QSKIP_RELAX	4 
#define QRAND_OUT_QSKIP_UPD	8 /* enforces strict skip output to fd_qrand */

/* 
 * o/p's random unique info->out_freq # of strings from info->qbank_arr of 
 * info->qbank_nq strings prettily to fd_pretty and in qrand format
 * to fd_qrand iff they don't equal -1 (ie -1 means ignore this fd)
 * Output is affected by the QRAND_OUT.* flags(see macros). 
 * */
static void qrand_out(struct qrand_info *info, int fd_pretty, int fd_qrand, 
		int flags)
{
	size_t i, index, dup_lim;
	char out_qrand[QMAX_LENQ + 4], out_pretty[QMAX_LENQ + 4];
	uint8_t uniq[info->qbank_nq];

	memset(uniq, 0, info->qbank_nq * sizeof(uniq[0]));

	for (i = 0, dup_lim = info->out_dup_lim; i < info->out_freq; i++) {
		index = rand_uniq_index(uniq, info->qbank_nq);

		if (flags & QRAND_OUT_QSKIP_RELAX) {
			if (qarr_hash_find(info->qbank_arr[index])) {
				if (!dup_lim) {
					i--;
					continue;
				}
				dup_lim--;
			}
		}

		if (fd_pretty != -1) {
			sprintf(out_pretty, "Q%d)%s\n\n", i, info->qbank_arr[index]);
			write(fd_pretty, out_pretty, strlen(out_pretty));
		}

		if (flags & QRAND_OUT_QSKIP_UPD) {
			if (qarr_hash_find(info->qbank_arr[index]))
				continue;
		}

		if (fd_qrand != -1) {
			sprintf(out_qrand, "<q%sq>\n", info->qbank_arr[index]);
			write(fd_qrand, out_qrand, strlen(out_qrand));
		}

	}
}

/* fills qarr with qs from file p_file, returns # of qs added */
static size_t qarr_fill(const char *p_file, char **qarr)
{
	FILE *f_qbank;
	size_t i = 0, nq = 0;
	int tok;

	if (!(f_qbank = fopen(p_file, "r"))) {
		perror("open");
		err_exit(EXIT_FAILURE,"");
	}

	yyin = f_qbank;
	while ((tok=yylex())) {
		if (tok == SIMQ) {
			nq++;
			ustrcpy(qarr[i++], yytext, yyleng, 1);
		}
	}

	fclose(f_qbank);
	return nq;
}

static int qrand_decide(struct qrand_info *info, int doopt_flags)
{
	int fd, qrand_out_flags = 0, maxqs, freq;
	size_t i;

	/* just pretty print random qs to stdout */
	if (doopt_flags == DOOPTSET_SIMPLE) {
		qrand_out(info, STDOUT_FILENO, -1, qrand_out_flags);

	/* pretty print to stdout and log in qrand_format to 
	 * the _new_ file info->p_qskip_new */
	} else if (doopt_flags == DOOPTSET_QSKIP_NEW) {
		if ((fd = creat(info->p_qskip_new, 0644)) == -1) {
			perror("creat");
			err_exit(EXIT_FAILURE, "");
		}

		qrand_out(info, STDOUT_FILENO, fd, qrand_out_flags);

	/* 
	 * pretty print to stdout by forcing ONLY AT MAX info->out_dup_lim
	 * duplicates with reference to info->p_qskip_old. If -u then 
	 * the output is skip_updated to p_qskip_old. 
	 * */

	/* ignore the DOOPT_QSKIP_UPD for the moment since both update and
	 * non-update version shares a preamble */
	} else if ((doopt_flags & ~DOOPT_QSKIP_UPD) == DOOPTSET_QSKIP_OLD) {
		QARR_INIT(info->qskip_old_arr);
		info->qskip_old_nq = 
			qarr_fill(info->p_qskip_old, info->qskip_old_arr);

		/* max we can offer */
		maxqs = (info->qbank_nq - info->qskip_old_nq) + info->out_dup_lim;

		/* what they asked for */
		freq = info->out_freq;

		if (freq > maxqs) {
			/* they asked for more than what we can offer */
			err_exit(EXIT_FAILURE, "Error : Try a smaller freq or "
				   "higher duplicate limit\n");
		}
		for (i = 0; i < info->qskip_old_nq; i++) {
			qarr_hash_add(info->qskip_old_arr[i]);
		}

		qrand_out_flags |= QRAND_OUT_QSKIP_RELAX;

		if (doopt_flags & DOOPT_QSKIP_UPD) {
			/* update version of DOOPTSET_QSKIP_OLD */
			if ((fd = open(info->p_qskip_old, O_APPEND | O_RDWR)) == -1) {
				perror("open");
				err_exit(EXIT_FAILURE, "");
			}
			qrand_out_flags |= QRAND_OUT_QSKIP_UPD;
			qrand_out(info, STDOUT_FILENO, fd, qrand_out_flags);

		} else {
			/* no update version of DOOPTSET_QSKIP_OLD */
			qrand_out(info, STDOUT_FILENO, -1, qrand_out_flags);
		}

	/* Erroneous option combination */
	} else {
		return -1;
	}

	return 0;
}

static void usage()
{
	printf
		("qrand v0.1\n"
		 "usage : qrand [opt opt_arg]+ qbank_file\n"
		 "Note : qrand always needs qbank_file as the main argument. "
		 "qbank_file is read-only, but qskip files are NOT!\n"
		 "qbank/qskip files input to qrand must be in the qrand_format "
		 "ie. questions are in the format <q[.\\n]*q>\n\n"


		 "opt and opt_arg:\n"
		 "-f <freq>\t\t\t(mandatory)# of qs to be written to stdout. "
		 "<freq> = -1 to know # of qs in qbank\n"

		 "-n <qskip_new>\t\t\t(only with -f) The new "
		 "qskip file to generated in qrand_format\n"

		 "-o <qskip_old>\t\t\tExisiting qskip file to be referenced "
		 "for skip output constrained by relaxation_level or duplicate_limit(-l)\n"

		 "-l <dup_lim>\t\t\t(optional with -o)Max # of duplicates allowed (relaxation level) "
		 "in reference to  the qskip_old file. No -l means <dup_lim> = 0\n"

		 "-u \t\t\t\t(optional with -o)updates <qskip_old> arg of -o\n\n"


		 "Examples : \n"
		 "$ qrand -f 9 qbank.txt # 9 rand uniq qs from qbank.txt\n"
		 "$ qrand -f 9 -n qskip.txt qbank.txt # same as above, "
		 "but also writes o/p to qskip.txt in qrand_format\n"
		 "$ qrand -f 9 -o qskip.txt -l 2 qbank.txt # forces qs NOT in "
		 "qskip.txt relaxed by max of 2 duplicates in reference to qskip.txt\n"
		 "$ qrand -f 9 -u -o qskip.txt -l 2 qbank.txt # same as above, "
		 "skip_updates qskip.txt as well\n\n"


		 "skip_updating qskip.txt means append generated qs to qskip.txt "
		 "iff they are not in the existing qskip.txt.\n"
		 "Checking for duplicates is case-insensitve and "
		 "white_space-insensitive, ie <q define foo q> and "
		 "<q Define\tFOO q> are the same \nBUT NOT <q define foo q> and "
		 "<q define foo ? q>\n");
}

int main(int argc, char *argv[])
{
	int cmd_opt, doopt_flags = 0;
	struct qrand_info info = {0};

	while ((cmd_opt = 
		getopt_long(argc, argv, "f:un:o:l:h", NULL, NULL)) != -1) {
		switch (cmd_opt) {
		case 'f':
			doopt_flags |= DOOPT_FREQ;
			info.out_freq = atoi(optarg);
			break;

		case 'u':
			doopt_flags |= DOOPT_QSKIP_UPD;
			break;

		case 'n':
			doopt_flags |= DOOPT_QSKIP_NEW;
			info.p_qskip_new = optarg;
			break;

		case 'o':
			doopt_flags |= DOOPT_QSKIP_OLD;
			info.p_qskip_old = optarg;
			break;

		case 'l':
			/* no -l means info.out_dup_lim = 0 */
			info.out_dup_lim = atoi(optarg);
			break;

		case 'h':
			usage();
			goto exit;

		case '?':
			fprintf(stderr, "Error : Invalid format see qrand -h\n");
			goto err;

		default:
			err_exit(EXIT_FAILURE, DEBUG_ERRSTR("Invalid option"));
		}
	}

	if (argc != (optind + 1)) {
		fprintf(stderr, "Error : requires qbank argument! see qrand -h\n");
		goto err;
	}

	info.p_qbank = argv[optind];
	QARR_INIT(info.qbank_arr);
	info.qbank_nq = qarr_fill(info.p_qbank, info.qbank_arr);

	if (info.out_freq > info.qbank_nq) {
		fprintf(stderr, "Error : Not enough questions in qbank"
			   	"(try -f <= %d)\n", info.qbank_nq);
		goto err;
	}

	if (qrand_decide(&info, doopt_flags) == -1) {
		fprintf(stderr, "Error : Invalid format see qrand -h\n");
		goto err;
	}

exit:
	return EXIT_SUCCESS;

err:
	return EXIT_FAILURE;
}
