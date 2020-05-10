/*
 * bibl.h
 *
 * Copyright (c) Chris Putnam 2005-2020
 *
 * Source code released under the GPL version 2
 *
 */
#ifndef BIBL_H
#define BIBL_H

#include <stdio.h>
#include "str.h"
#include "fields.h"
#include "reftypes.h"

typedef struct {
	long n;
	long max;
	fields **ref;
} bibl;

void bibl_init( bibl *b );
int  bibl_addref( bibl *b, fields *ref );
void bibl_free( bibl *b );
int  bibl_copy( bibl *bout, bibl *bin );
long bibl_findref( bibl *bin, const char *citekey );

#endif

