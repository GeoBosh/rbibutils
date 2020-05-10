/*
 * bibl.c
 *
 * Copyright (c) Chris Putnam 2005-2020
 *
 * Source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bibdefs.h"
#include "bibl.h"

void
bibl_init( bibl *b )
{
	b->n   = b->max = 0L;
	b->ref = NULL;
}

static int
bibl_alloc( bibl * b )
{
	int alloc = 50;

	b->ref = ( fields ** ) malloc( sizeof( fields* ) * alloc );
	if ( !b->ref ) return BIBL_ERR_MEMERR;

	b->max = alloc;

	return BIBL_OK;
}

static int
bibl_realloc( bibl * b )
{
	long alloc = b->max * 2;
	fields **more;

	more = ( fields ** ) realloc( b->ref, sizeof( fields* ) * alloc );
	if ( !more ) return BIBL_ERR_MEMERR;

	b->ref = more;
	b->max = alloc;

	return BIBL_OK;
}

int
bibl_addref( bibl *b, fields *ref )
{
	int status = BIBL_OK;

	if ( b->max==0 )
		status = bibl_alloc( b );
	else if ( b->n >= b->max )
		status = bibl_realloc( b );

	if ( status==BIBL_OK ) {
		b->ref[ b->n ] = ref;
		b->n++;
	}
	return status;
}

void
bibl_free( bibl *b )
{
	long i;

	for ( i=0; i<b->n; ++i )
		fields_delete( b->ref[i] );

	free( b->ref );

	bibl_init( b );
}

/* bibl_copy()
 *
 * returns BIBL_OK on success, BIBL_ERR_MEMERR on failure
 */
int
bibl_copy( bibl *bout, bibl *bin )
{
	fields *ref;
	int status;
	long i;

	for ( i=0; i<bin->n; ++i ) {

		ref = fields_dupl( bin->ref[i] );
		if ( !ref ) return BIBL_ERR_MEMERR;

		status = bibl_addref( bout, ref );
		if ( status!=BIBL_OK ) return status;

	}

	return BIBL_OK;
}

/* bibl_findref()
 *
 * returns position of reference matching citekey, else -1
 */
long
bibl_findref( bibl *bin, const char *citekey )
{
	long i;
	int n;

	for ( i=0; i<bin->n; ++i ) {

		n = fields_find( bin->ref[i], "refnum", LEVEL_ANY );
		if ( n==FIELDS_NOTFOUND ) continue;

		if ( !strcmp( fields_value( bin->ref[i], n, FIELDS_CHRP_NOUSE ), citekey ) ) return i;

	}

	return -1;
}
