/*
 * common_bt_btd.c
 *
 * Copyright (c) Chris Putnam 2003-2020
 * Copyright (c) Georgi N. Boshnakov 2020-2023
 *
 * File created by taking out common parts from bibtexin.c and bibtexdirectin.c.
 *
 * Program and source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "is_ws.h"
#include "intlist.h"
#include "str.h"
#include "utf8.h"
#include "str_conv.h"
#include "fields.h"
#include "slist.h"
#include "name.h"
#include "title.h"
#include "url.h"
#include "reftypes.h"
#include "latex_parse.h"
#include "bibformats.h"
#include "generic.h"

#include "R.h"

#include "common_bt_btd_blt.h"

extern slist find;
extern slist replace;

/* process_ref()
 *
 */
int
process_ref( fields *bibin, const char *p, loc *currloc )
{
	int fstatus, status = BIBL_OK;
	str type, id, tag, data;

	strs_init( &type, &id, &tag, &data, NULL );

	p = process_bibtextype( p, &type );
	p = process_bibtexid( p, &id );

	if ( str_is_empty( &type ) || str_is_empty( &id ) ) goto out;

	fstatus = fields_add( bibin, "INTERNAL_TYPE", str_cstr( &type ), LEVEL_MAIN );
	if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }

	fstatus = fields_add( bibin, "REFNUM", str_cstr( &id ), LEVEL_MAIN );
	if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }

	while ( *p ) {

		p = process_bibtexline( p, &tag, &data, STRIP_QUOTES, currloc );
		if ( p==NULL ) { status = BIBL_ERR_MEMERR; goto out; }

		if ( !str_has_value( &tag ) || !str_has_value( &data ) ) continue;

		fstatus = fields_add( bibin, str_cstr( &tag ), str_cstr( &data ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }

	}
out:
	strs_free( &type, &id, &tag, &data, NULL );
	return status;
}

/* bibtexdirectin_processf()
 *
 * Handle '@STRING', '@reftype', and ignore '@COMMENT'
 *                                   Georgi: also ignore @PREAMBLE
 */
int
bibtexin_processf( fields *bibin, const char *data, const char *filename, long nref, param *pm )
{
	loc currloc;

	currloc.progname = pm->progname;
	currloc.filename = filename;
	currloc.nref     = nref;

	if ( !strncasecmp( data, "@STRING", 7 ) ) {
		process_string( data+7, &currloc );
		return 0;
	} else if ( !strncasecmp( data, "@COMMENT", 8 ) || !strncasecmp( data, "@PREAMBLE", 9 )) {
	  // Georgi: added @PREAMBLE
	  //    todo: It could make sense to keep it for output to bibtex (or TeX related)
	  
		/* Not sure if these are real Bibtex, but not references */
		return 0;
	} else {
		process_ref( bibin, data, &currloc );
		return 1;
	}
}

int
is_url_tag( str *tag )
{
	if ( str_has_value( tag ) ) {
		if ( !strcasecmp( str_cstr( tag ), "url" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "file" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "doi" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "sentelink" ) ) return 1;
	}
	return 0;
}

int
is_name_tag( str *tag )
{
	if ( str_has_value( tag ) ) {
		if ( !strcasecmp( str_cstr( tag ), "author" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "editor" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "translator" ) ) return 1;
	}
	return 0;
}

static int
bibtex_cleanvalue( str *value )
{
	int status;
	str parsed;

	str_init( &parsed );
// REprintf("before clean: %s\n", value->data);

	status = latex_parse( value, &parsed );
	if ( status!=BIBL_OK ) goto out;

	str_strcpy( value, &parsed );
	if ( str_memerr( value ) ) status = BIBL_ERR_MEMERR;

// REprintf("after clean: %s\n", value->data);
	
out:
	str_free( &parsed );
	return status;
}

static int
bibtex_matches_list( fields *bibout, char *tag, char *suffix, str *data, int level, slist *names, int *match )
{
	int n, fstatus;
	str mergedtag;

	*match = 0;

	n = slist_find( names, data );
	if ( slist_wasfound( names, n ) ) {
		str_initstrsc( &mergedtag, tag, suffix, NULL );
		fstatus = fields_add( bibout, str_cstr( &mergedtag ), str_cstr( data ), level );
		str_free( &mergedtag );
		if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		*match = 1;
	}

	return BIBL_OK;
}

static int
bibtex_matches_asis_or_corps( fields *bibin, int m, param *pm, int *match )
{
	int status;

	status = bibtex_matches_list( bibin, fields_tag( bibin, m, FIELDS_STRP ), ":ASIS", fields_value( bibin, m, FIELDS_STRP ), LEVEL_MAIN, &(pm->asis), match );
	if ( *match==1 || status!=BIBL_OK ) return status;

	status = bibtex_matches_list( bibin, fields_tag( bibin, m, FIELDS_STRP ), ":CORP", fields_value( bibin, m, FIELDS_STRP ), LEVEL_MAIN, &(pm->corps), match );
	if ( *match==1 || status!=BIBL_OK ) return status;

	return BIBL_OK;
}

/* We need to:
 *    (1) break names into LaTeX tokens (e.g. respect "{van der Hoff}" as a single name element)
 *    (2) clean the values by removing brackets and things
 *    (3) convert the character set before any name processing happens (else things like "\"O" get split up)
 */
static int
bibtex_person_tokenize( fields *bibin, int m, param *pm, slist *tokens )
{
  int i, ok, status;
  str *s;

  // REprintf("person!\n");
  status = latex_tokenize( tokens, fields_value( bibin, m, FIELDS_STRP ) );
  if ( status!=BIBL_OK ) return status;

  // REprintf("\nbibtex_person_tokenize: person!\n");
  // REprintf("number of tokens: %d\n", tokens->n);
  // for ( i=0; i<tokens->n; ++i )
  //     REprintf( "%s\n", slist_cstr( tokens, i ) );
  // REprintf("\nbibtex_person_tokenize: person! --------------\n");

  for ( i=0; i<tokens->n; ++i ) {

    s = slist_str( tokens, i );
    
    // Georgi: removing since changes latex characters to unicode
    //         in names, see comments in bibtexin_cleanref() (in bibtexin.c)
    //    TODO: check if this causes bad side effects, ideally correct
    //    NOTE: The biblatexin.c version of 'bibtex_cleanvalue' has additional arguments
    //          and doesn't modify names (and also url's).
    //
    // Reinstating this, bad side effects
    status = bibtex_cleanvalue( s );
    if ( status!=BIBL_OK ) return status;

    // !!! Georgi: conversion is here!
    // !!!
    // REprintf("\ns before str_convert: %s\n", s->data);
    // ok = str_convert( s, pm->charsetin,  1, pm->utf8in,  pm->xmlin,
    ok = str_convert( s, pm->charsetin,  pm->latexin, pm->utf8in,  pm->xmlin,
		      // Georgi: change arg. latexout to 1
		      // TODO: make it argument to this function?
		      //       it should depend on --no-latex
		      // v1.3 - restoring latexout to 0
		      pm->charsetout, 0, pm->utf8out, pm->xmlout );
    // REprintf("s after str_convert: %s\n", s->data);
    if ( !ok ) return BIBL_ERR_MEMERR;

  }

  // REprintf("\nbibtex_person_tokenize: person!\n");
  // REprintf("number of tokens: %d\n", tokens->n);
  // for ( i=0; i<tokens->n; ++i )
  //   REprintf( "%s\n", slist_cstr( tokens, i ) );
  // REprintf("\nbibtex_person_tokenize: person! --------------\n");
     
  return BIBL_OK;
}

/* We need to:
 *      (1) Build individual names
 *      (2) Add them to the end of fields *bibin -- because of this, we have to look up the tag/data every time
 *          because we can reallocate the raw data and make any pointers stale
 */
static int
bibtex_person_add_names( fields *bibin, int m, slist *tokens )
{
	int begin, end, ok, n, etal;

	etal = name_findetal( tokens );
// REprintf("person_add_names!\n");

	begin = 0;
	n = tokens->n - etal;
	while ( begin < n ) {

		end = begin + 1;

		while ( end < n && strcasecmp( slist_cstr( tokens, end ), "and" ) )
			end++;


		if ( end - begin == 1 ) {
			ok = name_addsingleelement( bibin, fields_tag( bibin,m,FIELDS_CHRP), slist_cstr( tokens, begin ), LEVEL_MAIN, NAME_ASIS );
			if ( !ok ) return BIBL_ERR_MEMERR;
		} else {
			ok = name_addmultielement( bibin, fields_tag(bibin,m,FIELDS_CHRP), tokens, begin, end, LEVEL_MAIN );
			if ( !ok ) return BIBL_ERR_MEMERR;
		}

		begin = end + 1;

		/* Handle repeated 'and' errors: authors="G. F. Author and and B. K. Author" */
		while ( begin < n && !strcasecmp( slist_cstr( tokens, begin ), "and" ) )
			begin++;

	}

	if ( etal ) {
		ok = name_addsingleelement( bibin, fields_tag(bibin,m,FIELDS_CHRP), "et al.", LEVEL_MAIN, NAME_ASIS );
		if ( !ok ) return BIBL_ERR_MEMERR;
	}

// REprintf("person_add_names!(end)\n");

	return BIBL_OK;
}

/* Keep looking up tag values--we can reallocate when we add new names here */
int
bibtexin_person( fields *bibin, int m, param *pm )
{
	int status, match = 0;
	slist tokens;
	// REprintf("\nbibtexdirectin_person!\n");

	status = bibtex_matches_asis_or_corps( bibin, m, pm, &match );
	if ( status!=BIBL_OK || match==1 ) return status;

	slist_init( &tokens );

	status = bibtex_person_tokenize( bibin, m, pm, &tokens );
	if ( status!=BIBL_OK ) goto out;

	// int nout = fields_num( bibin );
	// int i;
	// if(nout > 0) {
	//   REprintf("bibtexdirectin_person: nout = %d\n" , nout);
	//   for(i = 0; i < nout; i++) {
	//     REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
	//   }
	// }
	
	status = bibtex_person_add_names( bibin, m, &tokens );
	if ( status!=BIBL_OK ) goto out;

	// nout = fields_num( bibin );
	// if(nout > 0) {
	//   REprintf("bibtexdirectin_person (end): nout = %d\n" , nout);
	//   for(i = 0; i < nout; i++) {
	//     REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
	//   }
	// }
out:
	slist_free( &tokens );
	return status;
}
