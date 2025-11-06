/*
 * common_bt_btd_out.h
 *
 * Copyright (c) Georgi N. Boshnakov 2022-2023
 *
 * Program and source code released under the GPL version 2
 *
 */

#ifndef COMMON_BT_BTD_OUT_H
#define COMMON_BT_BTD_OUT_H

#include <stdio.h>

#include "common_bt_blt_btd_out.h"


void append_people( fields *in, char *tag, char *ctag, char *atag,
	  char *bibtag, int level, fields *out, int format_opts, int latex_out, int *status );

int bibtexout_write( fields *out, FILE *fp, param *pm, unsigned long refnum );

#endif
