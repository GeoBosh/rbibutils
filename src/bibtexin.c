/*
 * bibtexin.c
 *
 * Copyright (c) Chris Putnam 2003-2020
 * Copyright (c) Georgi N. Boshnakov 2020
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

static slist find    = { 0, 0, 0, NULL };
static slist replace = { 0, 0, 0, NULL };

extern variants bibtex_all[];
extern int bibtex_nall;

/*****************************************************
 PUBLIC: void bibtexin_initparams()
*****************************************************/

static int bibtexin_convertf( fields *bibin, fields *info, int reftype, param *p );
static int bibtexin_processf( fields *bibin, const char *data, const char *filename, long nref, param *p );
static int bibtexin_cleanf( bibl *bin, param *p );
static int bibtexin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, str *line, str *reference, int *fcharset );
static int bibtexin_typef( fields *bibin, const char *filename, int nrefs, param *p );

int
bibtexin_initparams( param *pm, const char *progname )
{
	pm->readformat       = BIBL_BIBTEXIN;
	pm->charsetin        = BIBL_CHARSET_DEFAULT;
	pm->charsetin_src    = BIBL_SRC_DEFAULT;
	pm->latexin          = 1;
	pm->xmlin            = 0;
	pm->utf8in           = 0;
	pm->nosplittitle     = 0;
	pm->verbose          = 0;
	pm->addcount         = 0;
	pm->output_raw       = 0;

	pm->readf    = bibtexin_readf;
	pm->processf = bibtexin_processf;
	pm->cleanf   = bibtexin_cleanf;
	pm->typef    = bibtexin_typef;
	pm->convertf = bibtexin_convertf;
	pm->all      = bibtex_all;
	pm->nall     = bibtex_nall;

	slist_init( &(pm->asis) );
	slist_init( &(pm->corps) );

 	// TODO: these probably should be made parameters, as the others above;
	//       note that 'find' and 'replace' work in tandem, so both need to be cleared.
	slist_free( &find );
	slist_free( &replace );

	if ( !progname ) pm->progname = NULL;
	else {
		pm->progname = strdup( progname );
		if ( pm->progname==NULL ) return BIBL_ERR_MEMERR;
	}

	return BIBL_OK;
}

/*****************************************************
 PUBLIC: int bibtexin_readf()
*****************************************************/

/*
 * readf can "read too far", so we store this information in line, thus
 * the next new text is in line, either from having read too far or
 * from the next chunk obtained via str_fget()
 *
 * return 1 on success, 0 on error/end-of-file
 *
 */
static int
readmore( FILE *fp, char *buf, int bufsize, int *bufpos, str *line )
{
	if ( line->len ) return 1;
	else return str_fget( fp, buf, bufsize, bufpos, line );
}

/*
 * readf()
 *
 * returns zero if cannot get reference and hit end of-file
 * returns 1 if last reference in file, 2 if reference within file
 */
static int
bibtexin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, str *line, str *reference, int *fcharset )
{
	int haveref = 0;
	const char *p;
	*fcharset = CHARSET_UNKNOWN;
	while ( haveref!=2 && readmore( fp, buf, bufsize, bufpos, line ) ) {
		if ( line->len == 0 ) continue; /* blank line */
		p = &(line->data[0]);
		/* Recognize UTF8 BOM */
		if ( line->len > 2 && 
				(unsigned char)(p[0])==0xEF &&
				(unsigned char)(p[1])==0xBB &&
				(unsigned char)(p[2])==0xBF ) {
			*fcharset = CHARSET_UNICODE;
			p += 3;
		}
		p = skip_ws( p );
		if ( *p == '%' ) { /* commented out line */
			str_empty( line );
			continue;
		}
		if ( *p == '@' ) haveref++;
		if ( haveref && haveref<2 ) {
			str_strcatc( reference, p );
			str_addchar( reference, '\n' );
			str_empty( line );
		} else if ( !haveref ) str_empty( line );
	
	}
	return haveref;
}

/*****************************************************
 PUBLIC: int bibtexin_processf()
*****************************************************/

typedef struct loc {
	const char *progname;
	const char *filename;
	long nref;
} loc;

/* process_bibtextype()
 *
 * extract 'article', 'book', etc. from:
 *
 * @article{...}
 * @book(...)
 *
 * return pointer after '{' or '(' character
 */
static const char*
process_bibtextype( const char *p, str *type )
{
	str tmp;

	str_init( &tmp );

	if ( *p=='@' ) p++;
	p = skip_ws( p );

	p = str_cpytodelim( &tmp, p, "{( \t\r\n", 0 );
	p = skip_ws( p );

	if ( *p=='{' || *p=='(' ) p++;
	p = skip_ws( p );

	if ( str_has_value( &tmp ) ) str_strcpy( type, &tmp );
	else str_empty( type );

	str_free( &tmp );

	return p;
}

char *dummy_id = "dummyid";

static const char *
process_bibtexid( const char *p, str *id )
{
	const char *start_p = p;
	str tmp;

	str_init( &tmp );
	p = str_cpytodelim( &tmp, p, ",", 1 );

	if ( str_has_value( &tmp ) ) {
		if ( strchr( tmp.data, '=' ) ) {
			/* Endnote writes bibtex files w/o fields, try to
			 * distinguish via presence of an equal sign.... if
			 * it's there, assume that it's a tag/data pair instead
			 * and roll back.
			 */
			p = start_p;
			str_empty( id );
		} else {
			str_strcpy( id, &tmp );
		}
	} else {
	  // Georgi was: str_empty( id );
	  str_strcpyc( id, dummy_id );
	}

	str_free( &tmp );
	return skip_ws( p );
}

/* bibtex_tag()
 *
 * returns NULL on memory error, else position after tag+whitespace
 */
static const char *
bibtex_tag( const char *p, str *tag )
{
	p = str_cpytodelim( tag, p, "= \t\r\n", 0 );
	if ( str_memerr( tag ) ) return NULL;
	return skip_ws( p );
}

static int
quotation_mark_is_escaped( int nbraces, const char *p, const char *startp )
{
	if ( nbraces!=0 ) return 1;
	if ( p!=startp && *(p-1)=='\\' ) return 1;
	return 0;
}

static int
brace_is_escaped( int nquotes, const char *p, const char *startp )
{
	if ( nquotes!=0 ) return 1;
	if ( p!=startp && *(p-1)=='\\' ) return 1;
	return 0;
}

static int
char_is_escaped( int nquotes, int nbraces )
{
	if ( nquotes!=0 || nbraces!=0 ) return 1;
	return 0;
}

static int
add_token( slist *tokens, str *token )
{
	int status;

	if ( str_memerr( token ) ) return BIBL_ERR_MEMERR;

	status = slist_add( tokens, token );
	if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;

	str_empty( token );

	return BIBL_OK;
}

static const char *
bibtex_data( const char *p, slist *tokens, loc *currloc )
{
	int nbraces = 0, nquotes = 0;
	const char *startp = p;
	int status;
	str token;

	str_init( &token );

	while ( p && *p ) {

		/* ...have we reached end-of-data? */
		if ( nquotes==0 && nbraces==0 ) {
			if ( *p==',' || *p=='=' || *p=='}' || *p==')' ) goto out;
		}

		if ( *p=='\"' ) {
			str_addchar( &token, *p );
			if ( !quotation_mark_is_escaped( nbraces, p, startp ) ) {
				nquotes = !nquotes;
				if ( nquotes==0 ) {
					status = add_token( tokens, &token );
					if ( status!=BIBL_OK ) { p=NULL; goto out0; }
				}
			}
		}

		else if ( *p=='{' ) {
			str_addchar( &token, *p );
			if ( !brace_is_escaped( nquotes, p, startp ) ) {
				nbraces++;
			}
		}

		else if ( *p=='}' ) {
			str_addchar( &token, *p );
			if ( !brace_is_escaped( nquotes, p, startp ) ) {
				nbraces--;
				if ( nbraces==0 ) {
					status = add_token( tokens, &token );
					if ( status!=BIBL_OK ) { p=NULL; goto out0; }
				}
				if ( nbraces<0 ) {
					goto out;
				}
			}
		}

		else if ( *p=='#' ) {
			if ( char_is_escaped( nquotes, nbraces ) ) {
				str_addchar( &token, *p );
			}
			/* ...this is a bibtex string concatentation token */
			else {
				if ( str_has_value( &token ) ) {
					status = add_token( tokens, &token );
					if ( status!=BIBL_OK ) { p=NULL; goto out0; }
				}
				status = slist_addc( tokens, "#" );
				if ( status!=SLIST_OK ) { p=NULL; goto out0; }
			}
		}

		/* ...add escaped white-space and non-white-space to current token */
		else if ( !is_ws( *p ) || char_is_escaped( nquotes, nbraces ) ) {
			/* always add non-whitespace characters */
			if ( !is_ws( *p ) ) {
				str_addchar( &token, *p );
			}
			/* only add whitespace if token is non-empty; convert CR/LF to space */
			else if ( token.len!=0 ) {
				if ( *p!='\n' && *p!='\r' )
					str_addchar( &token, *p );
				else {
					str_addchar( &token, ' ' );
					while ( is_ws( *(p+1) ) ) p++;
				}
			}
		}

		/* ...unescaped white-space marks the end of a token */
		else if ( is_ws( *p ) ) {
			if ( token.len ) {
				status = add_token( tokens, &token );
				if ( status!=BIBL_OK ) { p=NULL; goto out0; }
			}
		}

		p++;
	}
out:
	if ( nbraces!=0 ) {
		REprintf( "%s: Mismatch in number of braces in file %s reference %ld.\n", currloc->progname, currloc->filename, currloc->nref );
	}
	if ( nquotes!=0 ) {
		REprintf( "%s: Mismatch in number of quotes in file %s reference %ld.\n", currloc->progname, currloc->filename, currloc->nref );
	}
	if ( str_has_value( &token ) ) {
		if ( str_memerr( &token ) ) { p = NULL; goto out; }
		status = slist_add( tokens, &token );
		if ( status!=SLIST_OK ) p = NULL;
	}
out0:
	str_free( &token );
	return p;
}

#define NOT_ESCAPED    (0)
#define ESCAPED_QUOTES (1)
#define ESCAPED_BRACES (2)

static int
token_is_escaped( str *s )
{
	if ( s->data[0]=='\"' && s->data[s->len-1]=='\"' ) return ESCAPED_QUOTES;
	if ( s->data[0]=='{'  && s->data[s->len-1]=='}'  ) return ESCAPED_BRACES;
	return NOT_ESCAPED;
}

/* replace_strings()
 *
 * do bibtex string replacement for data tokens
 */
static int
replace_strings( slist *tokens )
{
	int i, n;
	str *s;

	for ( i=0; i<tokens->n; ++i ) {

		s = slist_str( tokens, i );

		/* ...skip if token is protected by quotation marks or braces */
		if ( token_is_escaped( s ) ) continue;

		/* ...skip if token is string concatentation symbol */
		if ( !str_strcmpc( s, "#" ) ) continue;

		n = slist_find( &find, s );
		if ( slist_wasnotfound( &find, n ) ) continue;

		str_strcpy( s, slist_str( &replace, n ) );
		if ( str_memerr( s ) ) return BIBL_ERR_MEMERR;

	}

	return BIBL_OK;
}

static int
string_concatenate( slist *tokens, loc *currloc )
{
	int i, status, esc_s, esc_t;
	str *s, *t;

	i = 0;
	while ( i < tokens->n ) {

		s = slist_str( tokens, i );
		if ( str_strcmpc( s, "#" ) ) {
			i++;
			continue;
		}

		if ( i==0 || i==tokens->n-1 ) {
			REprintf( "%s: Warning: Stray string concatenation ('#' character) in file %s reference %ld\n",
					currloc->progname, currloc->filename, currloc->nref );
			status = slist_remove( tokens, i );
			if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;
			continue;
		}

		s = slist_str( tokens, i-1 );
		t = slist_str( tokens, i+1 );

		esc_s = token_is_escaped( s );
		esc_t = token_is_escaped( t );

		if ( esc_s != NOT_ESCAPED ) str_trimend( s, 1 );
		if ( esc_t != NOT_ESCAPED ) str_trimbegin( t, 1 );
		if ( esc_s != esc_t ) {
			if ( esc_s == NOT_ESCAPED ) {
				if ( esc_t == ESCAPED_QUOTES ) str_prepend( s, "\"" );
				else                           str_prepend( s, "{" );
			}
			else {
				if ( esc_t != NOT_ESCAPED ) str_trimend( t, 1 );
				if ( esc_s == ESCAPED_QUOTES ) str_addchar( t, '\"' );
				else                           str_addchar( t, '}' );
			}
		}

		str_strcat( s, t );
		if ( str_memerr( s ) ) return BIBL_ERR_MEMERR;

		/* ...remove concatenated string t */
		status = slist_remove( tokens, i+1 );
		if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;

		/* ...remove concatentation token '#' */
		status = slist_remove( tokens, i );
		if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;

	}

	return BIBL_OK;
}

#define KEEP_QUOTES  (0)
#define STRIP_QUOTES (1)

static int
merge_tokens_into_data( str *data, slist *tokens, int stripquotes )
{
	int i, esc_s;
	str *s;

	for ( i=0; i<tokens->n; i++ ) {

		s     = slist_str( tokens, i );
		esc_s = token_is_escaped( s );

		if ( ( esc_s == ESCAPED_BRACES ) ||
		     ( stripquotes == STRIP_QUOTES && esc_s == ESCAPED_QUOTES ) ) {
			str_trimbegin( s, 1 );
			str_trimend( s, 1 );
		}

		str_strcat( data, s );

	}

	if ( str_memerr( data ) ) return BIBL_ERR_MEMERR;
	else return BIBL_OK;
}

/* return NULL on memory error */
static const char *
process_bibtexline( const char *p, str *tag, str *data, uchar stripquotes, loc *currloc )
{
	slist tokens;
	int status;

	str_empty( data );

	slist_init( &tokens );

	p = bibtex_tag( skip_ws( p ), tag );
	if ( p ) {
		if ( str_is_empty( tag ) ) {
			p = skip_line( p );
			goto out;
		}
	}

	if ( p && *p=='=' ) {
		p = bibtex_data( p+1, &tokens, currloc );
	}

	if ( p ) {
		status = replace_strings( &tokens );
		if ( status!=BIBL_OK ) p = NULL;
	}

	if ( p ) {
		status = string_concatenate( &tokens, currloc );
		if ( status!=BIBL_OK ) p = NULL;
	}

	if ( p ) {
		status = merge_tokens_into_data( data, &tokens, stripquotes );
		if ( status!=BIBL_OK ) p = NULL;
	}

out:
	slist_free( &tokens );
	return p;
}

/* process_ref()
 *
 */
static int
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

/* process_string()
 *
 * Handle lines like:
 *
 * '@STRING{TL = {Tetrahedron Lett.}}'
 *
 * p should point to just after '@STRING'
 *
 * In BibTeX, if a string is defined several times, the last one is kept.
 *
 */
static int
process_string( const char *p, loc *currloc )
{
	int n, status = BIBL_OK;
	str s1, s2, *t;

	strs_init( &s1, &s2, NULL );

	while ( *p && *p!='{' && *p!='(' ) p++;
	if ( *p=='{' || *p=='(' ) p++;

	p = process_bibtexline( skip_ws( p ), &s1, &s2, KEEP_QUOTES, currloc );
	if ( p==NULL ) { status = BIBL_ERR_MEMERR; goto out; }

	if ( str_has_value( &s2 ) ) {
		str_findreplace( &s2, "\\ ", " " );
	} else {
		str_strcpyc( &s2, "" );
	}

	if ( str_has_value( &s1 ) ) {
		n = slist_find( &find, &s1 );
		if ( n==-1 ) {
			status = slist_add_ret( &find,    &s1, BIBL_OK, BIBL_ERR_MEMERR );
			if ( status!=BIBL_OK ) goto out;
			status = slist_add_ret( &replace, &s2, BIBL_OK, BIBL_ERR_MEMERR );
			if ( status!=BIBL_OK ) goto out;
		} else {
			t = slist_set( &replace, n, &s2 );
			if ( t==NULL ) { status = BIBL_ERR_MEMERR; goto out; }
		}
	}

out:
	strs_free( &s1, &s2, NULL );
	return status;
}

/* bibtexin_processf()
 *
 * Handle '@STRING', '@reftype', and ignore '@COMMENT'
 *                                   Georgi: also ignore @PREAMBLE
 */
static int
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

/*****************************************************
 PUBLIC: void bibtexin_cleanf()
*****************************************************/

static int
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

static int
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

	
	for ( i=0; i<tokens->n; ++i ) {

		s = slist_str( tokens, i );

		// Georgi: removing since changes latex characters to unicode
		//         in names, see comments in bibtexin_cleanref() (in bibtexin.c
		//    TODO: check if this causes bad side effects, ideally correct
		//
		// Reinstating this, bad side effects
		status = bibtex_cleanvalue( s );
		if ( status!=BIBL_OK ) return status;

		// !!! Georgi: conversion is here!
		// !!!
		// REprintf("\ns before str_convert: %s\n", s->data);
		  ok = str_convert( s, pm->charsetin,  1, pm->utf8in,  pm->xmlin,
				    // Georgi: change arg. latexout to 1
				    // TODO: make it argument to this function?
				    //       it should depend on --no-latex
				    // v1.3 - restoring latexout to 0
				    pm->charsetout, 0, pm->utf8out, pm->xmlout );
		  // REprintf("s after str_convert: %s\n", s->data);
		if ( !ok ) return BIBL_ERR_MEMERR;

	}

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

	return BIBL_OK;
}

/* Keep looking up tag values--we can reallocate when we add new names here */
static int
bibtexin_person( fields *bibin, int m, param *pm )
{
	int status, match = 0;
	slist tokens;
// REprintf("bibtexin_person!\n");

	status = bibtex_matches_asis_or_corps( bibin, m, pm, &match );
	if ( status!=BIBL_OK || match==1 ) return status;

	slist_init( &tokens );

	status = bibtex_person_tokenize( bibin, m, pm, &tokens );
	if ( status!=BIBL_OK ) goto out;

	status = bibtex_person_add_names( bibin, m, &tokens );
	if ( status!=BIBL_OK ) goto out;

out:
	slist_free( &tokens );
	return status;

}

static int
bibtexin_cleanref( fields *bibin, param *pm )
{
	int i, n, fstatus, status = BIBL_OK;
	str *tag, *value;
	intlist toremove;

	intlist_init( &toremove );

	n = fields_num( bibin );
// REprintf("n = %d\n", n);

	  // REprintf("n = %d\n" , n);
	  // for(i = 0; i < n; i++) {
	  //   REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
	  // }
	  

 
	for ( i=0; i<n; ++i ) {

		tag = fields_tag( bibin, i, FIELDS_STRP_NOUSE );
// REprintf("\ntag = %s\n", tag->data);
		if ( is_url_tag( tag ) ) continue; /* protect url from parsing */

		/* Georgi:  protecting names, otherwise havoc ensues if the input is 
                            in a different encoding; 
                       TODO: test side effects of doing this.
		   delay names from undergoing any parsing */
		/* 2020-09-26: but names need parsing since there may be more than one!
                       Commenting out to process properly names fields

                       TODO: return to this and check again!
                            I commented this out because of encodings - do tests!
                       Amendment: run the nex two lines  but only if tag is not names tag
                        (actually, moved them to the else part)
		 */
		// if ( is_name_tag( tag ) ) return BIBL_OK;
		// if ( !is_name_tag( tag ) ){
		value = fields_value( bibin, i, FIELDS_STRP_NOUSE );
		if ( str_is_empty( value ) ) continue;

		// }
		if ( is_name_tag( tag ) ) {
			status = bibtexin_person( bibin, i, pm );
// REprintf("i = %d\n", i);
// REprintf("value = %s\n", (bibin->value[i]).data);
 
			if ( status!=BIBL_OK ) goto out;

			fstatus = intlist_add( &toremove, i );
			if ( fstatus!=INTLIST_OK ) { status = BIBL_ERR_MEMERR; goto out; }
// REprintf("nout = %d\n" , fields_num( bibin ));
                        // goto out;
		}

		// else {
                // //         // REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
		// // 
                //          value = fields_value( bibin, i, FIELDS_STRP_NOUSE );
		//          if ( str_is_empty( value ) ) continue;
		// // 
                // //         // Georgi: bibtex_cleanvalue() drops $, {, }, for now just skip it
                // //         //   TODO: fix bibtex_cleanvalue() to not do that when not necessary 
		// // 	// // REprintf("i = %d, value = %s\n", i, value->data);
		// //         // status = bibtex_cleanvalue( value );
		// // 	// // REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
		// // 	// if ( status!=BIBL_OK ) goto out;
		// }

	}


	// int nout = fields_num( bibin );
	// if(nout > n) {
	//   REprintf("nout = %d\n" , nout);
	//   for(i = 0; i < nout; i++) {
	//     REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
	//   }
	//   
	// }

	
	for ( i=toremove.n-1; i>=0; i-- ) {
		fstatus = fields_remove( bibin, intlist_get( &toremove, i ) );
		if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }
	}

	
out:

	intlist_free( &toremove );

	
	// nout = fields_num( bibin );
	// if(nout > n) {
	//   REprintf("nout = %d\n" , nout);
	//   for(i = 0; i < nout; i++) {
	//     REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
	//   }
	//   
	// }
	
	return status;
}

static void
bibtexin_nocrossref( bibl *bin, long i, int n, param *p )
{
	int n1 = fields_find( bin->ref[i], "REFNUM", LEVEL_ANY );
	if ( p->progname ) REprintf( "%s: ", p->progname );
	REprintf( "Cannot find cross-reference '%s'", (char*) fields_value( bin->ref[i], n, FIELDS_CHRP_NOUSE ) );
	if ( n1!=FIELDS_NOTFOUND ) REprintf( " for reference '%s'\n", (char*) fields_value( bin->ref[i], n1, FIELDS_CHRP_NOUSE ) );
	REprintf( "\n" );
}

static int
bibtexin_crossref_oneref( fields *bibref, fields *bibcross )
{
	int i, n, newlevel, ntype, fstatus;
	char *type, *newtag, *newvalue;

	ntype = fields_find( bibref, "INTERNAL_TYPE", LEVEL_ANY );
	type = ( char * ) fields_value( bibref, ntype, FIELDS_CHRP_NOUSE );

	n = fields_num( bibcross );

	for ( i=0; i<n; ++i ) {

		newtag = ( char * ) fields_tag( bibcross, i, FIELDS_CHRP_NOUSE );
		if ( !strcasecmp( newtag, "INTERNAL_TYPE" ) ) continue;
		if ( !strcasecmp( newtag, "REFNUM" ) ) continue;
		if ( !strcasecmp( newtag, "TITLE" ) ) {
			if ( !strcasecmp( type, "Inproceedings" ) ||
			     !strcasecmp( type, "Incollection" ) )
				newtag = "booktitle";
		}

		newvalue = ( char * ) fields_value( bibcross, i, FIELDS_CHRP_NOUSE );

		newlevel = fields_level( bibcross, i ) + 1;

		fstatus = fields_add( bibref, newtag, newvalue, newlevel );
		if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
	}

	return BIBL_OK;
}

static int
bibtexin_crossref( bibl *bin, param *p )
{
	int i, n, ncross, status = BIBL_OK;
	fields *bibref, *bibcross;

	for ( i=0; i<bin->n; ++i ) {
		bibref = bin->ref[i];
		n = fields_find( bibref, "CROSSREF", LEVEL_ANY );
		if ( n==FIELDS_NOTFOUND ) continue;
		fields_set_used( bibref, n );
		ncross = bibl_findref( bin, (char*) fields_value( bibref, n, FIELDS_CHRP_NOUSE ) );
		if ( ncross==-1 ) {
			bibtexin_nocrossref( bin, i, n, p );
			continue;
		}
		bibcross = bin->ref[ncross];
		status = bibtexin_crossref_oneref( bibref, bibcross );
		if ( status!=BIBL_OK ) goto out;
	}
out:
	return status;
}

static int
bibtexin_cleanf( bibl *bin, param *p )
{
	int status;
	long i;

        for ( i=0; i<bin->n; ++i ) {
		status = bibtexin_cleanref( bin->ref[i], p );
		if ( status!=BIBL_OK ) return status;
	}
	status = bibtexin_crossref( bin, p );
	return status;
}

/*****************************************************
 PUBLIC: int bibtexin_typef()
*****************************************************/

static int
bibtexin_typef( fields *bibin, const char *filename, int nrefs, param *p )
{
	int ntypename, nrefname, is_default;
	char *refname = "", *typename = "";

	ntypename = fields_find( bibin, "INTERNAL_TYPE", LEVEL_MAIN );
	nrefname  = fields_find( bibin, "REFNUM",        LEVEL_MAIN );
	if ( nrefname!=FIELDS_NOTFOUND )  refname  = fields_value( bibin, nrefname,  FIELDS_CHRP_NOUSE );
	if ( ntypename!=FIELDS_NOTFOUND ) typename = fields_value( bibin, ntypename, FIELDS_CHRP_NOUSE );

	return get_reftype( typename, nrefs, p->progname, p->all, p->nall, refname, &is_default, REFTYPE_CHATTY );
}

/*****************************************************
 PUBLIC: int bibtexin_convertf(), returns BIBL_OK or BIBL_ERR_MEMERR
*****************************************************/

/**** bibtexin_btorg ****/

/*
 * BibTeX uses 'organization' in lieu of publisher if that field is missing.
 * Otherwise output as
 * <name type="corporate">
 *    <namePart>The organization</namePart>
 *    <role>
 *       <roleTerm authority="marcrelator" type="text">organizer of meeting</roleTerm>
 *    </role>
 * </name>
 */

static int
bibtexin_btorg( fields *bibin, int m, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int n, fstatus;
	n = fields_find( bibin, "publisher", LEVEL_ANY );
	if ( n==FIELDS_NOTFOUND )
		fstatus = fields_add( bibout, "PUBLISHER", str_cstr( invalue ), level );
	else
		fstatus = fields_add( bibout, "ORGANIZER:CORP", str_cstr( invalue ), level );
	if ( fstatus==FIELDS_OK ) return BIBL_OK;
	else return BIBL_ERR_MEMERR;
}

/**** bibtexin_btsente() ****/

/*
 * sentelink = {file://localhost/full/path/to/file.pdf,Sente,PDF}
 *
 * Sente is an academic reference manager for MacOSX and Apple iPad.
 */

static int
bibtexin_btsente( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int fstatus, status = BIBL_OK;
	str link;

	str_init( &link );
	str_cpytodelim( &link, skip_ws( invalue->data ), ",", 0 );
	str_trimendingws( &link );
	if ( str_memerr( &link ) ) status = BIBL_ERR_MEMERR;

	if ( status==BIBL_OK && link.len ) {
		fstatus = fields_add( bibout, "FILEATTACH", str_cstr( &link ), level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}

	str_free( &link );
	return status;
}

/**** bibtexin_linkedfile() ****/

static int
count_colons( char *p )
{
	int n = 0;
	while ( *p ) {
		if ( *p==':' ) n++;
		p++;
	}
	return n;
}

static int
first_colon( char *p )
{
	int n = 0;
	while ( p[n] && p[n]!=':' ) n++;
	return n;
}

static int
last_colon( char *p )
{
	int n = strlen( p ) - 1;
	while ( n>0 && p[n]!=':' ) n--;
	return n;
}

/*
 * file={Description:/full/path/to/file.pdf:PDF}
 */
static int
bibtexin_linkedfile( fields *bibin, int m, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int fstatus, status = BIBL_OK;
	char *p = str_cstr( invalue );
	int i, n, n1, n2;
	str link;

	n = count_colons( p );
	if ( n > 1 ) {
		/* A DOS file can contain a colon ":C:/....pdf:PDF" */
		/* Extract after 1st and up to last colons */
		n1 = first_colon( p ) + 1;
		n2 = last_colon( p );
		str_init( &link );
		for ( i=n1; i<n2; ++i ) {
			str_addchar( &link, p[i] );
		}
		str_trimstartingws( &link );
		str_trimendingws( &link );
		if ( str_memerr( &link ) ) {
			status = BIBL_ERR_MEMERR;
			goto out;
		}
		if ( link.len ) {
			fstatus = fields_add( bibout, "FILEATTACH", str_cstr( &link ), level );
			if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
		}
out:
		str_free( &link );
	} else {
		/* This field isn't formatted properly, so just copy directly */
		fstatus = fields_add( bibout, "FILEATTACH", p, level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}
	return status;

}

/**** bibtexin_howpublished() ****/

/*    howpublished={},
 *
 * Normally indicates the manner in which something was
 * published in lieu of a formal publisher, so typically
 * 'howpublished' and 'publisher' will never be in the
 * same reference.
 *
 * Occassionally, people put Diploma thesis information
 * into the field, so check that first.
 *
 * Returns BIBL_OK or BIBL_ERR_MEMERR
 */

static int
bibtexin_howpublished( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int fstatus, status = BIBL_OK;
	if ( !strncasecmp( str_cstr( invalue ), "Diplom", 6 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Diploma thesis", level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}
	else if ( !strncasecmp( str_cstr( invalue ), "HSabilitation", 13 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Habilitation thesis", level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}
	else if ( !strncasecmp( str_cstr( invalue ), "Licentiate", 10 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Licentiate thesis", level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}
	else if ( is_embedded_link( str_cstr( invalue ) ) ) {
		status =  urls_split_and_add( str_cstr( invalue ), bibout, level );
	}
	else {
		fstatus = fields_add( bibout, "PUBLISHER", str_cstr( invalue ), level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}
	return status;
}

/**** bibtexin_eprint() ****/

/* Try to capture situations like
 *
 * eprint="1605.02026",
 * archivePrefix="arXiv",
 *
 * or
 *
 * eprint="13211131",
 * eprinttype="medline",
 *
 * If we don't know anything, concatenate archivePrefix:eprint
 * and push into URL. (Could be wrong)
 *
 * If no info, just push eprint into URL. (Could be wrong)
 */
static int
process_eprint_with_prefix( fields *bibout, char *prefix, str *value, int level )
{
	int fstatus, status = BIBL_OK;
	str merge;

	if ( !strcmp( prefix, "arXiv" ) ) {
		fstatus = fields_add( bibout, "ARXIV", str_cstr( value ), level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}

	else if ( !strcmp( prefix, "jstor" ) ) {
		fstatus = fields_add( bibout, "JSTOR", str_cstr( value ), level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}

	else if ( !strcmp( prefix, "medline" ) ) {
		fstatus = fields_add( bibout, "MEDLINE", str_cstr( value ), level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}

	else if ( !strcmp( prefix, "pubmed" ) ) {
		fstatus = fields_add( bibout, "PMID", str_cstr( value ), level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}

	/* ...if this is unknown prefix, merge prefix & eprint */
	else {
		str_init( &merge );
		str_mergestrs( &merge, prefix, ":", str_cstr( value ), NULL );
		fstatus = fields_add( bibout, "URL", str_cstr( &merge ), level );
		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
		str_free( &merge );
	}

	return status;
}
static int
process_eprint_without_prefix( fields *bibout, str *value, int level )
{
	int fstatus;

	/* ...no archivePrefix, need to handle just 'eprint' tag */
	fstatus = fields_add( bibout, "URL", str_cstr( value ), level );

	if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
	else return BIBL_OK;
}

static int
bibtexin_eprint( fields *bibin, int m, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	char *prefix;
	int n;

	/* ...do we have an archivePrefix too? */
	n = fields_find( bibin, "ARCHIVEPREFIX", level );
	if ( n==FIELDS_NOTFOUND ) n = fields_find( bibin, "EPRINTTYPE", level );
	if ( n!=FIELDS_NOTFOUND ) {
		prefix = fields_value( bibin, n, FIELDS_CHRP );
		return process_eprint_with_prefix( bibout, prefix, invalue, level );
	}

	/* ...no we don't */
	return process_eprint_without_prefix( bibout, invalue, level );
}

/**** bibtexin_keyword() ****/

/* Split keywords="" with semicolons.
 * Commas are also frequently used, but will break
 * entries like:
 *       keywords="Microscopy, Confocal"
 * Returns BIBL_OK or BIBL_ERR_MEMERR
 */

static int
bibtexin_keyword( fields *bibin, int m, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int fstatus, status = BIBL_OK;
	const char *p;
	str keyword;

	p = str_cstr( invalue );
	str_init( &keyword );

	while ( *p ) {
		p = str_cpytodelim( &keyword, skip_ws( p ), ";", 1 );
		str_trimendingws( &keyword );
		if ( str_memerr( &keyword ) ) {
			status = BIBL_ERR_MEMERR;
			goto out;
		}
		if ( keyword.len ) {
			fstatus = fields_add( bibout, "KEYWORD", str_cstr( &keyword ), level );
			if ( fstatus!=FIELDS_OK ) {
				status = BIBL_ERR_MEMERR;
				goto out;
			}
		}
	}
out:
	str_free( &keyword );
	return status;
}

/**** bibtexin_title() ****/

/* bibtexin_titleinbook_isbooktitle()
 *
 * Normally, the title field of inbook refers to the book.  The
 * section in a @inbook reference is untitled.  If it's titled,
 * the @incollection should be used.  For example, in:
 *
 * @inbook{
 *    title="xxx"
 * }
 *
 * the booktitle is "xxx".
 *
 * However, @inbook is frequently abused (and treated like
 * @incollection) so that title and booktitle are present
 * and title is now 'supposed' to refer to the section.  For example:
 *
 * @inbook{
 *     title="yyy",
 *     booktitle="xxx"
 * }
 *
 * Therefore report whether or not booktitle is present as well
 * as title in @inbook references.  If not, then make 'title'
 * correspond to the title of the book, not the section.
 *
 */
static int
bibtexin_titleinbook_isbooktitle( fields *bibin, char *intag )
{
	int n;

	/* ...look only at 'title="xxx"' elements */
	if ( strcasecmp( intag, "TITLE" ) ) return 0;

	/* ...look only at '@inbook' references */
	n = fields_find( bibin, "INTERNAL_TYPE", LEVEL_ANY );
	if ( n==FIELDS_NOTFOUND ) return 0;
	if ( strcasecmp( fields_value( bibin, n, FIELDS_CHRP ), "INBOOK" ) ) return 0;

	/* ...look to see if 'booktitle="yyy"' exists */
	n = fields_find( bibin, "BOOKTITLE", LEVEL_ANY );
	if ( n==FIELDS_NOTFOUND ) return 0;
	else return 1;
}

static int
bibtexin_title( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int ok;

	if ( bibtexin_titleinbook_isbooktitle( bibin, intag->data ) ) level=LEVEL_MAIN;
	ok = title_process( bibout, "TITLE", invalue->data, level, pm->nosplittitle );
	if ( ok ) return BIBL_OK;
	else return BIBL_ERR_MEMERR;
}

static void
bibtexin_notag( param *p, char *tag )
{
	if ( p->verbose && strcmp( tag, "INTERNAL_TYPE" ) ) {
		if ( p->progname ) REprintf( "%s: ", p->progname );
		REprintf( "Cannot find tag '%s'\n", tag );
	}
}

static int
bibtexin_convertf( fields *bibin, fields *bibout, int reftype, param *p )
{
	static int (*convertfns[NUM_REFTYPES])(fields *, int, str *, str *, int, param *, char *, fields *) = {
		// [ 0 ... NUM_REFTYPES-1 ] = generic_null,
		// [ SIMPLE       ] = generic_simple,
		// [ TITLE        ] = bibtexin_title,
		// [ PERSON       ] = generic_simple,
		// [ PAGES        ] = generic_pages,
		// [ KEYWORD      ] = bibtexin_keyword,
		// [ EPRINT       ] = bibtexin_eprint,
		// [ HOWPUBLISHED ] = bibtexin_howpublished,
		// [ LINKEDFILE   ] = bibtexin_linkedfile,
		// [ NOTES        ] = generic_notes,
		// [ GENRE        ] = generic_genre,
		// [ BT_SENTE     ] = bibtexin_btsente,
		// [ BT_ORG       ] = bibtexin_btorg,
		// [ URL          ] = generic_url

                [ ALWAYS           ] = generic_null,  // (0)
		[ DEFAULT          ] = generic_null,  // (1)
		[ SKIP             ] = generic_null,  // (2)
		[ SIMPLE           ] = generic_simple,  // (3) 
		[ TYPE             ] = generic_null,  // (4) 
		[ PERSON           ] = generic_simple, // (5) 
		[ DATE             ] = generic_null,  // (6) 
		[ PAGES            ] = generic_pages,  // (7) 
		[ SERIALNO         ] = generic_null,  // (8) 
		[ TITLE            ] = bibtexin_title, // (9) 
		[ NOTES            ] = generic_notes,  // (10)
		[ DOI              ] = generic_null,  // (11)
		[ HOWPUBLISHED     ] = bibtexin_howpublished,  // (12)
		[ LINKEDFILE       ] = bibtexin_linkedfile,  // (13)
		[ KEYWORD          ] = bibtexin_keyword, // (14)
		[ URL              ] = generic_url,  // (15)
		[ GENRE            ] = generic_genre, // (16)
		[ BT_SENTE         ] = bibtexin_btsente,  // (17) /* Bibtex 'Sente' */
		[ BT_EPRINT        ] = generic_null,  // (18) /* Bibtex 'Eprint' */
		[ BT_ORG           ] = bibtexin_btorg, // (19) /* Bibtex Organization */
		[ BLT_THESIS_TYPE  ] = generic_null, // (20) /* Biblatex Thesis Type */
		[ BLT_SCHOOL       ] = generic_null,  // (21) /* Biblatex School */
		[ BLT_EDITOR       ] = generic_null, // (22) /* Biblatex Editor */
		[ BLT_SUBTYPE      ] = generic_null,  // (23) /* Biblatex entrysubtype */
		[ BLT_SKIP         ] = generic_skip,  // (24) /* Biblatex Skip Entry */
		[ EPRINT           ] = bibtexin_eprint // (25)
	};

	int process, level, i, nfields, status = BIBL_OK;
	str *intag, *invalue;
	char *outtag;

	nfields = fields_num( bibin );
	for ( i=0; i<nfields; ++i ) {

		if ( fields_used( bibin, i ) )   continue; /* e.g. successful crossref */
		if ( fields_no_tag( bibin, i ) )  continue;
		if ( fields_no_value( bibin, i ) ) continue;

		intag   = fields_tag( bibin, i, FIELDS_STRP );
		invalue = fields_value( bibin, i, FIELDS_STRP );

		if ( !translate_oldtag( str_cstr( intag ), reftype, p->all, p->nall, &process, &level, &outtag ) ) {
			bibtexin_notag( p, str_cstr( intag ) );
			continue;
		}

		status = convertfns[ process ] ( bibin, i, intag, invalue, level, p, outtag, bibout );
		if ( status!=BIBL_OK ) return status;
	}

	if ( status==BIBL_OK && p->verbose ) fields_report_stderr( bibout );

	return status;
}
