/*
 * common_bt_btd_blt.c
 *
 * Copyright (c) Chris Putnam 2003-2020
 * Copyright (c) Georgi N. Boshnakov 2020-2023
 *
 * File created by taking out parts from bibtexin.c, bibtexdirectin.c and biblatexin.c that
 * are common to all three of them.
 *
 * Program and source code released under the GPL version 2
 *
 */
#ifndef COMMON_BT_BTD_BLT_H
#define COMMON_BT_BTD_BLT_H

// #include "fields.h"
#include "bibutils.h"
#include "charsets.h"
#include "is_ws.h"
#include "str.h"

#include "common_bt_btd_blt.h"

typedef struct loc {
	const char *progname;
	const char *filename;
	long nref;
} loc;

#define KEEP_QUOTES  (0)
#define STRIP_QUOTES (1)

#define NOT_ESCAPED    (0)
#define ESCAPED_QUOTES (1)
#define ESCAPED_BRACES (2)


int bibtexin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, str *line, str *reference, int *fcharset );

const char *process_bibtexid( const char *p, str *id );

const char *process_bibtexline( const char *p, str *tag, str *data, uchar stripquotes, loc *currloc );

const char*process_bibtextype( const char *p, str *type );

int process_string( const char *p, loc *currloc );
int bibtexin_crossref( bibl *bin, param *p );
int bibtexin_typef( fields *bibin, const char *filename, int nrefs, param *p );

#endif
