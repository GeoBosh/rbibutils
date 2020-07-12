/*
 * fields.c
 *
 * Copyright (c) Chris Putnam 2003-2020
 *
 * Source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "fields.h"

#define FIELDS_MIN_ALLOC (20)

/* private helper macros to access fields
 *
 * These skip all of the error checking and used flag manipulation
 * of the fields functions, so they are faster. However, they should
 * only be used in internal code that knows that the index is valid.
 */
#define _fields_tag(f,i)            &((f)->tag[(i)])
#define _fields_tag_char(f,i)       str_cstr( &((f)->tag[(i)]) )
#define _fields_tag_notempty(f,i)   str_has_value( &((f)->tag[(i)]) )
#define _fields_value(f,i)          &((f)->value[(i)])
#define _fields_value_char(f,i)     str_cstr( &((f)->value[(i)]) )
#define _fields_value_notempty(f,i) str_has_value( &((f)->value[(i)]) )
#define _fields_level(f,i)          (f)->level[(i)]

fields*
fields_new( void )
{
	fields *f = ( fields * ) malloc( sizeof( fields ) );
	if ( f ) fields_init( f );
	return f;
}

void
fields_init( fields *f )
{
	f->used  = f->level = NULL;
	f->tag   = f->value = NULL;
	f->max   = f->n     = 0;
}

void
fields_free( fields *f )
{
	int i;

	for ( i=0; i<f->max; ++i ) {
		str_free( _fields_tag( f, i ) );
		str_free( _fields_value( f, i ) );
	}
	if ( f->tag )   free( f->tag );
	if ( f->value ) free( f->value );
	if ( f->used )  free( f->used );
	if ( f->level ) free( f->level );

	fields_init( f );
}

void
fields_delete( fields *f )
{
	fields_free( f );
	free( f );
}

int
fields_remove( fields *f, int n )
{
	int i;
	if ( n<0 || n>= f->n ) return FIELDS_ERR_MEMERR;
	for ( i=n+1; i<f->n; ++i ) {
		str_strcpy( _fields_tag  ( f, i-1 ), _fields_tag  ( f, i ) );
		str_strcpy( _fields_value( f, i-1 ), _fields_value( f, i ) );
		f->used[i-1]  = f->used[i];
		f->level[i-1] = f->level[i];
	}
	f->n -= 1;
	return FIELDS_OK;
}

static void
initialize_new_tag_data_pairs( fields *f, int start, int end )
{
	int i;
	for ( i=start; i<end; ++i ) {
		str_init( _fields_tag( f, i ) );
		str_init( _fields_value( f, i ) );
	}
}

static int
fields_alloc( fields *f, int alloc )
{
	f->tag   = (str *) malloc( sizeof(str) * alloc );
	f->value = (str *) malloc( sizeof(str) * alloc );
	f->used  = (int *) calloc( alloc, sizeof(int) );
	f->level = (int *) calloc( alloc, sizeof(int) );
	if ( !f->tag || !f->value || !f->used || !f->level ){
		if ( f->tag )   free( f->tag );
		if ( f->value ) free( f->value );
		if ( f->used )  free( f->used );
		if ( f->level ) free( f->level );
		fields_init( f );
		return FIELDS_ERR_MEMERR;
	}

	initialize_new_tag_data_pairs( f, 0, alloc );

	f->max = alloc;
	f->n   = 0;

	return FIELDS_OK;
}

static int
fields_realloc( fields *f )
{
	int *newused, *newlevel;
	str *newtags, *newvalue;
	int alloc;

	alloc = f->max * 2;
	if ( alloc < f->max ) return FIELDS_ERR_MEMERR; /* integer overflow */

	newtags  = (str*) realloc( f->tag,   sizeof(str) * alloc );
	newvalue = (str*) realloc( f->value, sizeof(str) * alloc );
	newused  = (int*) realloc( f->used,  sizeof(int) * alloc );
	newlevel = (int*) realloc( f->level, sizeof(int) * alloc );

	/*
	 * ensure struct fields is consistent by reassigning pointers for any successful
	 * reallocation prior to failing on memory error
	 */
	if ( newtags )  f->tag   = newtags;
	if ( newvalue ) f->value = newvalue;
	if ( newused )  f->used  = newused;
	if ( newlevel ) f->level = newlevel;

	if ( !newtags || !newvalue || !newused || !newlevel )
		return FIELDS_ERR_MEMERR;

	initialize_new_tag_data_pairs( f, f->n, alloc );

	f->max = alloc;

	return FIELDS_OK;
}

static int
ensure_space( fields *f )
{
	int status = FIELDS_OK;
	if ( f->max==0 )         status = fields_alloc( f, FIELDS_MIN_ALLOC );
	else if ( f->n==f->max ) status = fields_realloc( f );
	return status;
}

static int
is_duplicate_entry( fields *f, const char *tag, const char *value, int level )
{
	int i;

	for ( i=0; i<f->n; i++ ) {
		if ( _fields_level( f, i ) != level ) continue;
		if ( strcasecmp( _fields_tag_char( f, i ),   tag   ) ) continue;
		if ( strcasecmp( _fields_value_char( f, i ), value ) ) continue;
		return 1;
	}

	return 0;
}

int
_fields_add( fields *f, const char *tag, const char *value, int level, int mode )
{
	int n, status;

	/* Don't add incomplete entry */
	if ( !tag || !value ) return FIELDS_OK;

	/* Don't add duplicate entry if FIELDS_NO_DUPS */
	if ( mode == FIELDS_NO_DUPS && is_duplicate_entry( f, tag, value, level ) )
		return FIELDS_OK;

	status = ensure_space( f );
	if ( status!=FIELDS_OK ) return status;

	n = f->n;
	f->used[ n ]  = 0;
	f->level[ n ] = level;
	str_strcpyc( _fields_tag( f, n ),   tag   );
	str_strcpyc( _fields_value( f, n ), value );

	if ( str_memerr( &(f->tag[n]) ) || str_memerr( &(f->value[n] ) ) )
		return FIELDS_ERR_MEMERR;

	f->n++;

	return FIELDS_OK;
}

int
_fields_add_suffix( fields *f, const char *tag, const char *suffix, const char *value, int level, int mode )
{
	str newtag;
	int ret;

	str_init( &newtag );

	str_mergestrs( &newtag, tag, suffix, NULL );
	if ( str_memerr( &newtag ) )
		ret = FIELDS_ERR_MEMERR;
	else
		ret = _fields_add( f, str_cstr( &newtag ), value, level, mode );

	str_free( &newtag );

	return ret;
}

static fields*
fields_new_size( int alloc )
{
	int status;
	fields *f;

	f = ( fields * ) malloc( sizeof( fields ) );
	if ( f ) {
		fields_init( f );
		status = fields_alloc( f, alloc );
		if ( status!=FIELDS_OK ) {
			fields_delete( f );
			return NULL;
		}
	}
	return f;
}

fields *
fields_dupl( fields *in )
{
	int i, level, status;
	char *tag, *value;
	fields *out;

	out = fields_new_size( in->n );
	if ( !out ) return NULL;

	for ( i=0; i<in->n; ++i ) {
		tag   = _fields_tag_char( in, i );
		value = _fields_value_char( in, i );
		level = _fields_level( in, i );
		if ( tag && value ) {
			status = fields_add_can_dup( out, tag, value, level );
			if ( status!=FIELDS_OK ) {
				fields_delete( out );
				return NULL;
			}
		}
	}

	return out;
}

/* fields_match_level()
 *
 * returns 1 if level matched, 0 if not
 *
 * level==LEVEL_ANY is a special flag meaning any level can match
 */
int
fields_match_level( fields *f, int n, int level )
{
	if ( level==LEVEL_ANY ) return 1;
	if ( fields_level( f, n )==level ) return 1;
	return 0;
}

/* fields_match_tag()
 *
 * returns 1 if tag matches, 0 if not
 *
 */
int
fields_match_tag( fields *f, int n, const char *tag )
{
	if ( !strcmp( _fields_tag_char( f, n ), tag ) ) return 1;
	return 0;
}

int
fields_match_casetag( fields *f, int n, const char *tag )
{
	if ( !strcasecmp( _fields_tag_char( f, n ), tag ) ) return 1;
	return 0;
}

int
fields_match_tag_level( fields *f, int n, const char *tag, int level )
{
	if ( !fields_match_level( f, n, level ) ) return 0;
	return fields_match_tag( f, n, tag );
}

int
fields_match_casetag_level( fields *f, int n, const char *tag, int level )
{
	if ( !fields_match_level( f, n, level ) ) return 0;
	return fields_match_casetag( f, n, tag );
}

/* fields_find()
 *
 * Return position [0,f->n) for first match of the tag.
 * Return FIELDS_NOTFOUND if tag isn't found.
 */
int
fields_find( fields *f, const char *tag, int level )
{
	int i;

	for ( i=0; i<f->n; ++i ) {
		if ( !fields_match_casetag_level( f, i, tag, level ) )
			continue;
		if ( str_has_value( _fields_value( f, i ) ) ) return i;
		else {
			/* if there is no data for the tag, don't "find" it */
			/* and set "used" so noise is suppressed */
			f->used[i] = 1;
		}
	}

	return FIELDS_NOTFOUND;
}

int
fields_maxlevel( fields *f )
{
	int i, max = 0;

	if ( f->n ) {
		max = _fields_level( f, 0 );
		for ( i=1; i<f->n; ++i ) {
			if ( _fields_level( f, i ) > max )
				max = _fields_level( f, i );
		}
	}

	return max;
}

void
fields_clear_used( fields *f )
{
	int i;

	for ( i=0; i<f->n; ++i )
		f->used[i] = 0;
}

void
fields_set_used( fields *f, int n )
{
	if ( n >= 0 && n < f->n )
		f->used[n] = 1;
}

/* fields_replace_or_add()
 *
 * return FIELDS_OK on success, else FIELDS_ERR_MEMERR
 */
int
fields_replace_or_add( fields *f, const char *tag, const char *value, int level )
{
	int n;

	n = fields_find( f, tag, level );
	if ( n==FIELDS_NOTFOUND ) {
		return fields_add( f, tag, value, level );
	}
	else {
		str_strcpyc( _fields_value( f, n ), value );
		if ( str_memerr( _fields_value( f, n ) ) ) return FIELDS_ERR_MEMERR;
		return FIELDS_OK;
	}
}

char *fields_null_value = "\0";

int
fields_used( fields *f, int n )
{
	if ( n >= 0 && n < f->n ) return f->used[n];
	else return 0;
}

int
fields_no_tag( fields *f, int n )
{
	if ( n >= 0 && n < f->n ) {
		if ( _fields_tag_notempty( f, n ) ) return 0;
	}
	return 1;
}

int
fields_no_value( fields *f, int n )
{
	if ( n >= 0 && n < f->n ) {
		if ( _fields_value_notempty( f, n ) ) return 0;
	}
	return 1;
}

int
fields_has_value( fields *f, int n )
{
	if ( n >= 0 && n < f->n ) return _fields_value_notempty( f, n );
	return 0;
}

int
fields_num( fields *f )
{
	return f->n;
}

/*
 * #define FIELDS_CHRP       
 * #define FIELDS_STRP       
 * #define FIELDS_CHRP_NOLEN
 * #define FIELDS_STRP_NOLEN
 * 
 * If the length of the tagged value is zero and the mode is
 * FIELDS_STRP_NOLEN or FIELDS_CHRP_NOLEN, return a pointer to
 * a static null string as the data field could be new due to
 * the way str handles initialized strings with no data.
 *
 */

void *
fields_value( fields *f, int n, int mode )
{
	intptr_t retn;

	if ( n<0 || n>= f->n ) return NULL;

	if ( mode & FIELDS_SETUSE_FLAG )
		fields_set_used( f, n );

	if ( mode & FIELDS_STRP_FLAG ) {
		return ( void * ) _fields_value( f, n );
	}
	else if ( mode & FIELDS_POSP_FLAG ) {
		retn = n;               /* avoid compiler warning */
		return ( void * ) retn; /* Rather pointless -- the user provided "n" */
	}
	else {
		if ( str_has_value( _fields_value( f, n ) ) )
			return _fields_value_char( f, n );
		else
			return fields_null_value;
	}
}

void *
fields_tag( fields *f, int n, int mode )
{
	intptr_t retn;

	if ( n<0 || n>= f->n ) return NULL;

	if ( mode & FIELDS_STRP_FLAG ) {
		return ( void * ) _fields_tag( f, n );
	}
	else if ( mode & FIELDS_POSP_FLAG ) {
		retn = n;               /* avoid compiler warning */
		return ( void * ) retn; /* Rather pointless -- the user provided "n" */
	}
	else {
		if ( str_has_value( _fields_tag( f, n ) ) )
			return _fields_tag_char( f, n );
		else
			return fields_null_value;
	}
}

int
fields_level( fields *f, int n )
{
	if ( n<0 || n>= f->n ) return 0;
	return _fields_level( f, n );
}

void *
fields_findv( fields *f, int level, int mode, const char *tag )
{
	int i, found = FIELDS_NOTFOUND;

	for ( i=0; i<f->n; ++i ) {

		if ( !fields_match_level( f, i, level ) ) continue;
		if ( !fields_match_casetag( f, i, tag ) ) continue;

		if ( _fields_value_notempty( f, i ) ) {
			found = i;
			break;
		} else {
			if ( mode & FIELDS_NOLENOK_FLAG ) {
				return (void *) fields_null_value;
			} else if ( mode & FIELDS_SETUSE_FLAG ) {
				f->used[i] = 1; /* Suppress "noise" of unused */
			}
		}
	}

	if ( found==FIELDS_NOTFOUND ) return NULL;
	else return fields_value( f, found, mode );
}

void *
fields_findv_firstof( fields *f, int level, int mode, ... )
{
	char *tag, *value;
	va_list argp;

	va_start( argp, mode );
	while ( ( tag = ( char * ) va_arg( argp, char * ) ) ) {
		value = fields_findv( f, level, mode, tag );
		if ( value ) {
			va_end( argp );
			return value;
		}
	}
	va_end( argp );

	return NULL;
}

static int
fields_findv_each_add( fields *f, int mode, int n, vplist *a )
{
	int status;
	void *v;

	v = fields_value( f, n, mode );
	if ( !v ) return FIELDS_OK;

	status = vplist_add( a, v );

	if ( status==VPLIST_OK ) return FIELDS_OK;
	else return FIELDS_ERR_MEMERR;
}

int
fields_findv_each( fields *f, int level, int mode, vplist *a, const char *tag )
{
	int i, status;

	for ( i=0; i<f->n; ++i ) {

		if ( !fields_match_level( f, i, level ) ) continue;
		if ( !fields_match_casetag( f, i, tag ) ) continue;

		if ( _fields_value_notempty( f, i ) ) {
			status = fields_findv_each_add( f, mode, i, a );
			if ( status!=FIELDS_OK ) return status;
		} else {
			if ( mode & FIELDS_NOLENOK_FLAG ) {
				status = fields_findv_each_add( f, mode, i, a );
				if ( status!=FIELDS_OK ) return status;
			} else {
				f->used[i] = 1; /* Suppress "noise" of unused */
			}
		}

	}

	return FIELDS_OK;
}

static int
fields_build_tags( va_list argp, vplist *tags )
{
	int status;
	char *tag;

	while ( ( tag = ( char * ) va_arg( argp, char * ) ) ) {
		status = vplist_add( tags, tag );
		if ( status!=VPLIST_OK ) return FIELDS_ERR_MEMERR;
	}

	return FIELDS_OK;
}

static int
fields_match_casetags( fields *f, int n, vplist *tags )
{
	int i;

	for ( i=0; i<tags->n; ++i )
		if ( fields_match_casetag( f, n, vplist_get( tags, i ) ) ) return 1;

	return 0;
}

int
fields_findv_eachof( fields *f, int level, int mode, vplist *a, ... )
{
	int i, status;
	va_list argp;
	vplist tags;

	vplist_init( &tags );

	/* build list of tags to search for */
	va_start( argp, a );
	status = fields_build_tags( argp, &tags );
	va_end( argp );
	if ( status!=FIELDS_OK ) goto out;

	/* search list */
	for ( i=0; i<f->n; ++i ) {

		if ( !fields_match_level( f, i, level ) ) continue;
		if ( !fields_match_casetags( f, i, &tags ) ) continue;

		if ( _fields_value_notempty( f, i ) || ( mode & FIELDS_NOLENOK_FLAG ) ) {
			status = fields_findv_each_add( f, mode, i, a );
			if ( status!=FIELDS_OK ) goto out;
		} else {
			f->used[i] = 1; /* Suppress "noise" of unused */
		}

	}

out:
	vplist_free( &tags );
	return status;
}

void
fields_report( fields *f, FILE *fp )
{
	int i, n;
	n = fields_num( f );
	fprintf( fp, "# NUM   level = LEVEL   'TAG' = 'VALUE'\n" );
	for ( i=0; i<n; ++i ) {
		REprintf( "%d\tlevel = %d\t'%s' = '%s'\n",
			i+1,
			_fields_level( f, i ),
			_fields_tag_char( f, i ),
			_fields_value_char( f, i )
		);
	}
}

// Georgi - as above, stderr only
void
fields_report_stderr( fields *f)
{
	int i, n;
	n = fields_num( f );
	REprintf( "# NUM   level = LEVEL   'TAG' = 'VALUE'\n" );
	for ( i=0; i<n; ++i ) {
		REprintf( "%d\tlevel = %d\t'%s' = '%s'\n",
			i+1,
			_fields_level( f, i ),
			_fields_tag_char( f, i ),
			_fields_value_char( f, i )
		);
	}
}

// Georgi - as above, stdout only
void
fields_report_stdout( fields *f)
{
	int i, n;
	n = fields_num( f );
	Rprintf( "# NUM   level = LEVEL   'TAG' = 'VALUE'\n" );
	for ( i=0; i<n; ++i ) {
		Rprintf( "%d\tlevel = %d\t'%s' = '%s'\n",
			i+1,
			_fields_level( f, i ),
			_fields_tag_char( f, i ),
			_fields_value_char( f, i )
		);
	}
}

