/*
 * biblatexin.c
 *
 * Copyright (c) Chris Putnam 2008-2020
 * Copyright (c) Johannes Wilm 2010-2020
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
#include "strsearch.h"
#include "str.h"
#include "utf8.h"
#include "str_conv.h"
#include "fields.h"
#include "latex_parse.h"
#include "slist.h"
#include "name.h"
#include "reftypes.h"
#include "bibformats.h"
#include "generic.h"

extern variants biblatex_all[];
extern int biblatex_nall;

static slist find    = { 0, 0, 0, NULL };
static slist replace = { 0, 0, 0, NULL };

/*****************************************************
 PUBLIC: void biblatexin_initparams()
*****************************************************/

static int  biblatexin_convertf( fields *bibin, fields *info, int reftype, param *p );
static int  biblatexin_processf( fields *bibin, const char *data, const char *filename, long nref, param *p );
static int  biblatexin_cleanf( bibl *bin, param *p );
static int  biblatexin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, str *line, str *reference, int *fcharset );
static int  biblatexin_typef( fields *bibin, const char *filename, int nrefs, param *p );

int
biblatexin_initparams( param *pm, const char *progname )
{
	pm->readformat       = BIBL_BIBLATEXIN;
	pm->charsetin        = BIBL_CHARSET_DEFAULT;
	pm->charsetin_src    = BIBL_SRC_DEFAULT;
	pm->latexin          = 1;
	pm->xmlin            = 0;
	pm->utf8in           = 0;
	pm->nosplittitle     = 0;
	pm->verbose          = 0;
	pm->addcount         = 0;
	pm->output_raw       = 0;

	pm->readf    = biblatexin_readf;
	pm->processf = biblatexin_processf;
	pm->cleanf   = biblatexin_cleanf;
	pm->typef    = biblatexin_typef;
	pm->convertf = biblatexin_convertf;
	pm->all      = biblatex_all;
	pm->nall     = biblatex_nall;

	slist_init( &(pm->asis) );
	slist_init( &(pm->corps) );

 	// TODO: these probably should be made parameters, as the others above;
	//       note that 'find' and 'replace' work in tandem, so both need to be cleared.
	slist_free( &find );
	slist_free( &replace );

	if ( !progname ) pm->progname = NULL;
	else {
		pm->progname = strdup( progname );
		if ( !pm->progname ) return BIBL_ERR_MEMERR;
	}

	return BIBL_OK;
}

/*****************************************************
 PUBLIC: int biblatexin_readf()
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
biblatexin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, str *line, str *reference, int *fcharset )
{
	int haveref = 0;
	const char *p;
	while ( haveref!=2 && readmore( fp, buf, bufsize, bufpos, line ) ) {
		if ( line->len == 0 ) continue; /* blank line */
		p = &(line->data[0]);
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
	*fcharset = CHARSET_UNKNOWN;
	return haveref;
}

/*****************************************************
 PUBLIC: int biblatexin_processf()
*****************************************************/

static const char *
process_biblatextype( const char *p, str *type )
{
	str tmp;
	str_init( &tmp );

	if ( *p=='@' ) p++;
	p = str_cpytodelim( &tmp, p, "{( \t\r\n", 0 );
	p = skip_ws( p );
	if ( *p=='{' || *p=='(' ) p++;
	p = skip_ws( p );

	if ( str_has_value( &tmp ) ) str_strcpy( type, &tmp );
	else str_empty( type );

	str_free( &tmp );
	return p;
}

static const char *
process_biblatexid( const char *p, str *id )
{
	const char *start_p = p;
	str tmp;

	str_init( &tmp );
	p = str_cpytodelim( &tmp, p, ",", 1 );

	if ( str_has_value( &tmp ) ) {
		if ( strchr( tmp.data, '=' ) ) {
			/* Endnote writes biblatex files w/o fields, try to
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
		str_empty( id );
	}

	str_free( &tmp );
	return skip_ws( p );
}

static const char *
biblatex_tag( const char *p, str *tag )
{
	p = str_cpytodelim( tag, skip_ws( p ), "= \t\r\n", 0 );
	return skip_ws( p );
}

static const char *
biblatex_data( const char *p, fields *bibin, slist *tokens, long nref, param *pm )
{
	unsigned int nbracket = 0, nquotes = 0;
	const char *startp = p;
	int status;
	str tok;

	str_init( &tok );
	while ( p && *p ) {
		if ( !nquotes && !nbracket ) {
			if ( *p==',' || *p=='=' || *p=='}' || *p==')' )
				goto out;
		}
		if ( *p=='\"' && nbracket==0 && ( p==startp || *(p-1)!='\\' ) ) {
			nquotes = !nquotes;
			str_addchar( &tok, *p );
			if ( !nquotes ) {
				status = slist_add( tokens, &tok );
				if ( status!=SLIST_OK ) { p = NULL; goto outerr; }
				str_empty( &tok );
			}
		} else if ( *p=='#' && !nquotes && !nbracket ) {
			if ( str_has_value( &tok ) ) {
				status = slist_add( tokens, &tok );
				if ( status!=SLIST_OK ) { p = NULL; goto outerr; }
			}
			str_strcpyc( &tok, "#" );
			status = slist_add( tokens, &tok );
			if ( status!=SLIST_OK ) { p = NULL; goto outerr; }
			str_empty( &tok );
		} else if ( *p=='{' && !nquotes && ( p==startp || *(p-1)!='\\' ) ) {
			nbracket++;
			str_addchar( &tok, *p );
		} else if ( *p=='}' && !nquotes && ( p==startp || *(p-1)!='\\' ) ) {
			nbracket--;
			str_addchar( &tok, *p );
			if ( nbracket==0 ) {
				status = slist_add( tokens, &tok );
				if ( status!=SLIST_OK ) { p = NULL; goto outerr; }
				str_empty( &tok );
			}
		} else if ( !is_ws( *p ) || nquotes || nbracket ) {
			if ( !is_ws( *p ) ) str_addchar( &tok, *p );
			else {
				if ( tok.len!=0 && *p!='\n' && *p!='\r' )
					str_addchar( &tok, *p );
				else if ( tok.len!=0 && (*p=='\n' || *p=='\r')) {
					str_addchar( &tok, ' ' );
					while ( is_ws( *(p+1) ) ) p++;
				}
			}
		} else if ( is_ws( *p ) ) {
			if ( str_has_value( &tok ) ) {
				status = slist_add( tokens, &tok );
				if ( status!=SLIST_OK ) { p = NULL; goto outerr; }
				str_empty( &tok );
			}
		}
		p++;
	}
out:
	if ( nbracket!=0 ) {
		REprintf( "%s: Mismatch in number of brackets in reference %ld\n", pm->progname, nref );
	}
	if ( nquotes!=0 ) {
		REprintf( "%s: Mismatch in number of quotes in reference %ld\n", pm->progname, nref );
	}
	if ( str_has_value( &tok ) ) {
		status = slist_add( tokens, &tok );
		if ( status!=SLIST_OK ) p = NULL;
	}
outerr:
	str_free( &tok );
	return p;
}

/* replace_strings()
 *
 * do string replacement -- only if unprotected by quotation marks or curly brackets
 */
static void
replace_strings( slist *tokens, fields *bibin, long nref, param *pm )
{
	int i, n, ok;
	char *q;
	str *s;
	i = 0;
	while ( i < tokens->n ) {
		s = slist_str( tokens, i );
		if ( !strcmp( s->data, "#" ) ) {
		} else if ( s->data[0]!='\"' && s->data[0]!='{' ) {
			n = slist_find( &find, s );
			if ( n!=-1 ) {
				str_strcpy( s, slist_str( &replace, n ) );
			} else {
				q = s->data;
				ok = 1;
				while ( *q && ok ) {
					if ( !isdigit( *q ) ) ok = 0;
					q++;
				}
				if ( !ok ) {
					REprintf( "%s: Warning: Non-numeric "
					   "BibTeX elements should be in quotations or "
					   "curly brackets in reference %ld\n", pm->progname, nref );
				}
			}
		}
		i++;
	}
}

static int
string_concatenate( slist *tokens, fields *bibin, long nref, param *pm )
{
	int i, status;
	str *s, *t;
	i = 0;
	while ( i < tokens->n ) {
		s = slist_str( tokens, i );
		if ( !strcmp( str_cstr( s ), "#" ) ) {
			if ( i==0 || i==tokens->n-1 ) {
				REprintf( "%s: Warning: Stray string concatenation "
					"('#' character) in reference %ld\n", pm->progname, nref );
				status = slist_remove( tokens, i );
				if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;
				continue;
			}
			s = slist_str( tokens, i-1 );
			if ( s->data[0]!='\"' && s->data[s->len-1]!='\"' )
				REprintf( "%s: Warning: String concentation should "
					"be used in context of quotations marks in reference %ld\n", pm->progname, nref );
			t = slist_str( tokens, i+1 );
			if ( t->data[0]!='\"' && t->data[t->len-1]!='\"' )
				REprintf( "%s: Warning: String concentation should "
					"be used in context of quotations marks in reference %ld\n", pm->progname, nref );
			if ( ( s->data[s->len-1]=='\"' && t->data[0]=='\"') || (s->data[s->len-1]=='}' && t->data[0]=='{') ) {
				str_trimend( s, 1 );
				str_trimbegin( t, 1 );
				str_strcat( s, t );
			} else {
				str_strcat( s, t );
			}
			status = slist_remove( tokens, i );
			if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;
			status = slist_remove( tokens, i );
			if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;
		} else i++;
	}
	return BIBL_OK;
}

static const char *
process_biblatexline( const char *p, str *tag, str *data, uchar stripquotes, long nref, param *pm )
{
	int i, status;
	slist tokens;
	str *s;

	str_empty( data );

	p = biblatex_tag( p, tag );
	if ( str_is_empty( tag ) ) {
		/* ...skip this line */
		while ( *p && *p!='\n' && *p!='\r' ) p++;
		while ( *p=='\n' || *p=='\r' ) p++;
		return p;
	}

	slist_init( &tokens );

	if ( *p=='=' ) p = biblatex_data( p+1, NULL, &tokens, nref, pm );

	replace_strings( &tokens, NULL, nref, pm );

	status = string_concatenate( &tokens, NULL, nref, pm );
	if ( status!=BIBL_OK ) {
		p = NULL;
		goto out;
	}

	for ( i=0; i<tokens.n; i++ ) {
		s = slist_str( &tokens, i );
		if ( ( stripquotes && s->data[0]=='\"' && s->data[s->len-1]=='\"' ) ||
		     ( s->data[0]=='{' && s->data[s->len-1]=='}' ) ) {
			str_trimbegin( s, 1 );
			str_trimend( s, 1 );
		}
		str_strcat( data, slist_str( &tokens, i ) );
	}
out:
	slist_free( &tokens );
	return p;
}

static int
process_cite( fields *bibin, const char *p, const char *filename, long nref, param *pm )
{
	int fstatus, status = BIBL_OK;
	str type, id, tag, data;

	strs_init( &type, &id, &tag, &data, NULL );

	p = process_biblatextype( p, &type );
	p = process_biblatexid( p, &id );

	if ( str_is_empty( &type ) || str_is_empty( &id ) ) goto out;

	fstatus = fields_add( bibin, "INTERNAL_TYPE", str_cstr( &type ), 0 );
	if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }

	fstatus = fields_add( bibin, "REFNUM", str_cstr( &id ), 0 );
	if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }

	while ( *p ) {
		p = process_biblatexline( p, &tag, &data, 1, nref, pm );
		if ( !p ) { status = BIBL_ERR_MEMERR; goto out; }
		/* no anonymous or empty fields allowed */
		if ( str_has_value( &tag ) && str_has_value( &data ) ) {
			fstatus = fields_add( bibin, str_cstr( &tag ), str_cstr( &data ), 0 );
			if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }
		}
		strs_empty( &tag, &data, NULL );
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
process_string( const char *p, long nref, param *pm )
{
	int n, status = BIBL_OK;
	str s1, s2, *s;
	strs_init( &s1, &s2, NULL );
	while ( *p && *p!='{' && *p!='(' ) p++;
	if ( *p=='{' || *p=='(' ) p++;
	(void) process_biblatexline( skip_ws( p ), &s1, &s2, 0, nref, pm );
	if ( str_has_value( &s2 ) ) {
		str_findreplace( &s2, "\\ ", " " );
		if ( str_memerr( &s2 ) ) { status = BIBL_ERR_MEMERR; goto out; }
	} else {
		str_strcpyc( &s2, "" );
	}
	if ( str_has_value( &s1 ) ) {
		n = slist_find( &find, &s1 );
		if ( n==-1 ) {
			status = slist_add_ret( &find, &s1, BIBL_OK, BIBL_ERR_MEMERR );
			if ( status!=BIBL_OK ) goto out;
			status = slist_add_ret( &replace, &s2, BIBL_OK, BIBL_ERR_MEMERR );
			if ( status!=BIBL_OK ) goto out;
		} else {
			if ( str_has_value( &s2 ) ) s = slist_set( &replace, n, &s2 );
			else s = slist_setc( &replace, n, "" );
			if ( s==NULL ) { status = BIBL_ERR_MEMERR; goto out; }
		}
	}
out:
	strs_free( &s1, &s2, NULL );
	return status;
}

static int
biblatexin_processf( fields *bibin, const char *data, const char *filename, long nref, param *p )
{
	if ( !strncasecmp( data, "@STRING", 7 ) ) {
		process_string( data+7, nref, p );
		return 0;
        } else {
		process_cite( bibin, data, filename, nref, p );
		return 1;
	}
}

/*****************************************************
 PUBLIC: void biblatexin_cleanf()
*****************************************************/

static int
is_url_tag( str *tag )
{
	if ( str_has_value( tag ) ) {
		if ( !strcasecmp( str_cstr( tag ), "url" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "file" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "doi" ) ) return 1;
	}
	return 0;
}

static int
is_name_tag( str *tag )
{
	if ( str_has_value( tag ) ) {
		if ( !strcasecmp( str_cstr( tag ), "author" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "editor" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "editorb" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "editorc" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "director" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "producer" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "execproducer" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "writer" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "redactor" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "annotator" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "commentator" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "translator" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "foreword" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "afterword" ) ) return 1;
		if ( !strcasecmp( str_cstr( tag ), "introduction" ) ) return 1;
	}
	return 0;
}

static int
biblatexin_cleanvalue( str *tag, str *value, fields *bibin, param *p )
{
	int status = BIBL_OK;
	str parsed;

	if ( str_is_empty( value ) ) return BIBL_OK;

	/* protect url from undergoing any parsing */
	if ( is_url_tag( tag ) ) return BIBL_OK;

	/* delay names from undergoing any parsing */
	if ( is_name_tag( tag ) ) return BIBL_OK;

	str_init( &parsed );

	status = latex_parse( value, &parsed );
	if ( status!=BIBL_OK ) goto out;

	str_strcpy( value, &parsed );
	if ( str_memerr( value ) ) status = BIBL_ERR_MEMERR;

out:
	str_free( &parsed );
	return status;
}

static void
biblatexin_nocrossref( bibl *bin, long i, int n, param *p )
{
	int n1 = fields_find( bin->ref[i], "REFNUM", LEVEL_ANY );
	if ( p->progname ) REprintf( "%s: ", p->progname );
	REprintf( "Cannot find cross-reference '%s'", (char*)fields_value( bin->ref[i], n, FIELDS_CHRP_NOUSE ) );
	if ( n1!=FIELDS_NOTFOUND )
		REprintf( " for reference '%s'", (char*)fields_value( bin->ref[i], n1, FIELDS_CHRP_NOUSE ) );
	REprintf( "\n" );
}

static int
biblatexin_crossref_oneref( fields *ref, fields *cross )
{
	int j, nl, ntype, fstatus;
	char *type, *nt, *nd;
	ntype = fields_find( ref, "INTERNAL_TYPE", LEVEL_ANY );
	type = ( char * ) fields_value( ref, ntype, FIELDS_CHRP_NOUSE );
	for ( j=0; j<cross->n; ++j ) {
		nt = ( char * ) fields_tag( cross, j, FIELDS_CHRP_NOUSE );
		if ( !strcasecmp( nt, "INTERNAL_TYPE" ) ) continue;
		if ( !strcasecmp( nt, "REFNUM" ) ) continue;
		if ( !strcasecmp( nt, "TITLE" ) ) {
			if ( !strcasecmp( type, "Inproceedings" ) ||
			     !strcasecmp( type, "Incollection" ) )
				nt = "booktitle";
		}
		nd = ( char * ) fields_value( cross, j, FIELDS_CHRP_NOUSE );
		nl = fields_level( cross, j ) + 1;
		fstatus = fields_add( ref, nt, nd, nl );
		if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
	}
	return BIBL_OK;
}

static int
biblatexin_crossref( bibl *bin, param *p )
{
	int n, ncross, status = BIBL_OK;
	fields *ref, *cross;
	long i;
        for ( i=0; i<bin->n; ++i ) {
		ref = bin->ref[i];
		n = fields_find( ref, "CROSSREF", LEVEL_ANY );
		if ( n==FIELDS_NOTFOUND ) continue;
		fields_set_used( ref, n );
		ncross = bibl_findref( bin, (char*) fields_value(ref,n, FIELDS_CHRP_NOUSE) );
		if ( ncross==-1 ) {
			biblatexin_nocrossref( bin, i, n, p );
			continue;
		}
		cross = bin->ref[ncross];
		status = biblatexin_crossref_oneref( ref, cross );
		if ( status!=BIBL_OK ) return status;
	}
	return status;
}

static int
biblatexin_cleanref( fields *bibin, param *p )
{
	int i, n, status;
	str *t, *d;
	n = fields_num( bibin );
	for ( i=0; i<n; ++i ) {
		t = fields_tag( bibin, i, FIELDS_STRP_NOUSE );
		d = fields_value( bibin, i, FIELDS_STRP_NOUSE );
		status = biblatexin_cleanvalue( t, d, bibin, p );
		if ( status!=BIBL_OK ) return status;
		if ( !strsearch( str_cstr( t ), "AUTHORS" ) ) {
			str_findreplace( d, "\n", " " );
			str_findreplace( d, "\r", " " );
		}
		else if ( !strsearch( str_cstr( t ), "ABSTRACT" ) ||
		     !strsearch( str_cstr( t ), "SUMMARY" ) || 
		     !strsearch( str_cstr( t ), "NOTE" ) ) {
			str_findreplace( d, "\n", "" );
			str_findreplace( d, "\r", "" );
		}
	}
	return BIBL_OK;
}

static int
biblatexin_cleanf( bibl *bin, param *p )
{
	int status;
	long i;
        for ( i=0; i<bin->n; ++i ) {
		status = biblatexin_cleanref( bin->ref[i], p );
		if ( status!=BIBL_OK ) return status;
	}
	status = biblatexin_crossref( bin, p );
	return status;
}

/*****************************************************
 PUBLIC: void biblatexin_typef()
*****************************************************/

static int
biblatexin_typef( fields *bibin, const char *filename, int nrefs, param *p )
{
	int ntypename, nrefname, is_default;
	char *refname = "", *typename="";

	ntypename = fields_find( bibin, "INTERNAL_TYPE", LEVEL_MAIN );
	nrefname  = fields_find( bibin, "REFNUM",        LEVEL_MAIN );
	if ( nrefname!=FIELDS_NOTFOUND )  refname  = fields_value( bibin, nrefname,  FIELDS_CHRP_NOUSE );
        if ( ntypename!=FIELDS_NOTFOUND ) typename = fields_value( bibin, ntypename, FIELDS_CHRP_NOUSE );

	return get_reftype( typename, nrefs, p->progname, p->all, p->nall, refname, &is_default, REFTYPE_CHATTY );
}

/*****************************************************
 PUBLIC: int biblatexin_convertf(), returns BIBL_OK or BIBL_ERR_MEMERR
*****************************************************/

/* get_title_elements()
 *
 * find all of the biblatex title elements for the current level
 *    internal "TITLE"      -> "title", "booktitle", "maintitle"
 *    internal "SUBTITLE"   -> "subtitle", "booksubtitle", "mainsubtitle"
 *    internal "TITLEADDON" -> "titleaddon", "booktitleaddon", "maintitleaddon"
 *
 * place in ttl, subttl, and ttladdon strings
 *
 * return 1 if an element is found, 0 if not
 */
static int
get_title_elements( fields *bibin, int currlevel, int reftype, variants *all, int nall, str *ttl, str *subttl, str *ttladdon )
{
	int nfields, process, level, i;
	str *t, *d;
	char *newtag;

	strs_empty( ttl, subttl, ttladdon, NULL );

	nfields = fields_num( bibin );

	for ( i=0; i<nfields; ++i ) {

		/* ...skip already used titles */
		if ( fields_used( bibin, i ) ) continue;

		/* ...skip empty elements */
		t = fields_tag  ( bibin, i, FIELDS_STRP_NOUSE );
		d = fields_value( bibin, i, FIELDS_STRP_NOUSE );
		if ( d->len == 0 ) continue;

		if ( !translate_oldtag( t->data, reftype, all, nall, &process, &level, &newtag ) )
			continue;
		if ( process != TITLE ) continue;
		if ( level != currlevel ) continue;

		fields_set_used( bibin, i );

		if ( !strcasecmp( newtag, "TITLE" ) ) {
			if ( str_has_value( ttl ) ) str_addchar( ttl, ' ' );
			str_strcat( ttl, d );
		} else if ( !strcasecmp( newtag, "SUBTITLE" ) ) {
			if ( str_has_value( subttl ) ) str_addchar( subttl, ' ' );
			str_strcat( subttl, d );
		} else if ( !strcasecmp( newtag, "TITLEADDON" ) ) {
			if ( str_has_value( ttladdon ) ) str_addchar( ttladdon, ' ' );
			str_strcat( ttladdon, d );
		}
	}

	return ( ttl->len>0 || subttl->len > 0 || ttladdon->len > 0 );
}

/* attach_subtitle()
 *
 * Add subtitle (if exists) to the title
 */
static void
attach_subtitle( str *title, str *subtitle )
{
	if ( str_has_value( subtitle ) ) {
		if ( str_has_value( title ) ) {
			if ( title->data[title->len-1]!=':' && title->data[title->len-1]!='?' )
				str_addchar( title, ':' );
			str_addchar( title, ' ' );
		}
		str_strcat( title, subtitle );
	}
}

/* attach_addon()
 *
 * Add titleaddon (if exists) to the title.
 */
static void
attach_addon( str *title, str *addon )
{
	if ( str_has_value( addon ) ) {
		if ( str_has_value( title ) ) {
			if ( title->data[title->len-1]!='.' )
				str_addchar( title, '.' );
			str_addchar( title, ' ' );
		}
		str_strcat( title, addon );
	}
}

static int
process_combined_title( fields *info, str *ttl, str *subttl, str *ttladdon, int currlevel )
{
	int fstatus, status = BIBL_OK;
	str combined;

	str_init( &combined );

	str_strcpy( &combined, ttl );
	attach_subtitle( &combined, subttl );
	attach_addon( &combined, ttladdon );

	if ( str_memerr( &combined ) ) {
		status = BIBL_ERR_MEMERR;
		goto out;
	}

	fstatus = fields_add( info, "TITLE", str_cstr( &combined ), currlevel );
	if ( fstatus==FIELDS_OK ) status = BIBL_ERR_MEMERR;
out:
	str_free( &combined );
	return status;
}

static int
process_separated_title( fields *info, str *ttl, str *subttl, str *ttladdon, int currlevel )
{
	int fstatus;
	if ( str_has_value( ttladdon ) ) {
		if ( subttl->len ) attach_addon( subttl, ttladdon );
		else attach_addon( ttl, ttladdon );
	}
	if ( str_has_value( ttl ) ) {
		fstatus = fields_add( info, "TITLE", str_cstr( ttl ), currlevel );
		if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
	}
	if ( str_has_value( subttl ) ) {
		fstatus = fields_add( info, "SUBTITLE", str_cstr( subttl ), currlevel );
		if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
	}
	return BIBL_OK;
}

static int
process_title_all( fields *bibin, fields *info, int reftype, param *p )
{
	int currlevel, found, status = BIBL_OK;
	str ttl, subttl, ttladdon;
	strs_init( &ttl, &subttl, &ttladdon, NULL );
	for ( currlevel = 0; currlevel<LEVEL_SERIES+2; currlevel++ ) {
		found = get_title_elements( bibin, currlevel, reftype, p->all, p->nall, &ttl, &subttl, &ttladdon );
		if ( !found ) continue;
		if ( p->nosplittitle )
			status = process_combined_title( info, &ttl, &subttl, &ttladdon, currlevel );
		else
			status = process_separated_title( info, &ttl, &subttl, &ttladdon, currlevel );
		if ( status!=BIBL_OK ) goto out;
	}
out:
	strs_free( &ttl, &subttl, &ttladdon, NULL );
	return status;
}


static int
biblatex_matches_list( fields *info, char *tag, char *suffix, str *data, int level, slist *names, int *match )
{
	int i, fstatus, status = BIBL_OK;
	str newtag;

	*match = 0;
	if ( names->n==0 ) return status;

	str_init( &newtag );

	for ( i=0; i<names->n; ++i ) {
		if ( strcmp( str_cstr( data ), slist_cstr( names, i ) ) ) continue;
		str_initstrc( &newtag, tag );
		str_strcatc( &newtag, suffix );
		fstatus = fields_add( info, str_cstr( &newtag ), str_cstr( data ), level );
		if ( fstatus!=FIELDS_OK ) {
			status = BIBL_ERR_MEMERR;
			goto out;
		}
		*match = 1;
		goto out;
	}

out:
	str_free( &newtag );
	return status;
}

static int
biblatex_names( fields *info, char *tag, str *data, int level, slist *asis, slist *corps )
{
	int begin, end, ok, n, etal, i, match, status = BIBL_OK;
	slist tokens;
	str parsed, *s;

	/* If we match the asis or corps list add and bail. */
	status = biblatex_matches_list( info, tag, ":ASIS", data, level, asis, &match );
	if ( match==1 || status!=BIBL_OK ) return status;
	status = biblatex_matches_list( info, tag, ":CORP", data, level, corps, &match );
	if ( match==1 || status!=BIBL_OK ) return status;

	slist_init( &tokens );
	str_init( &parsed );

	status = latex_tokenize( &tokens, data );
	if ( status!=BIBL_OK ) goto out;

	for ( i=0; i<tokens.n; ++i ) {
		status = latex_parse( slist_str( &tokens, i ), &parsed );
		if ( status!=BIBL_OK ) goto out;
		s = slist_set( &tokens, i, &parsed );
		if ( !s ) { status=BIBL_ERR_MEMERR; goto out; }
	}

	etal = name_findetal( &tokens );

	begin = 0;
	n = tokens.n - etal;
	while ( begin < n ) {

		end = begin + 1;

		while ( end < n && strcasecmp( slist_cstr( &tokens, end ), "and" ) )
			end++;

		if ( end - begin == 1 ) {
			ok = name_addsingleelement( info, tag, slist_cstr( &tokens, begin ), level, 0 );
			if ( !ok ) { status = BIBL_ERR_MEMERR; goto out; }
		} else {
			ok = name_addmultielement( info, tag, &tokens, begin, end, level );
			if ( !ok ) { status = BIBL_ERR_MEMERR; goto out; }
		}

		begin = end + 1;

		/* Handle repeated 'and' errors */
		while ( begin < n && !strcasecmp( slist_cstr( &tokens, begin ), "and" ) )
			begin++;

	}

	if ( etal ) {
		ok = name_addsingleelement( info, tag, "et al.", level, 0 );
		if ( !ok ) status = BIBL_ERR_MEMERR;
	}

out:
	str_free( &parsed );
	slist_free( &tokens );
	return status;
}

static int
biblatexin_bltsubtype( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int fstatus1, fstatus2;

	if ( !strcasecmp( str_cstr( invalue ), "magazine" ) ) {
		fstatus1 = fields_add( bibout, "GENRE:BIBUTILS", "magazine article", LEVEL_MAIN );
		fstatus2 = fields_add( bibout, "GENRE:BIBUTILS", "magazine",         LEVEL_HOST );
		if ( fstatus1!=FIELDS_OK || fstatus2!=FIELDS_OK ) return BIBL_ERR_MEMERR;
	}

	else if ( !strcasecmp( str_cstr( invalue ), "newspaper" ) ) {
		fstatus1 = fields_add( bibout, "GENRE:BIBUTILS", "newspaper article", LEVEL_MAIN );
		fstatus2 = fields_add( bibout, "GENRE:MARC",     "newspaper",         LEVEL_HOST );
		if ( fstatus1!=FIELDS_OK || fstatus2!=FIELDS_OK ) return BIBL_ERR_MEMERR;
	}

	return BIBL_OK;
}

/* biblatex drops school field if institution is present */
static int
biblatexin_bltschool( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int fstatus;
	if ( fields_find( bibin, "institution", LEVEL_ANY ) != FIELDS_NOTFOUND )
		return BIBL_OK;
	else {
		fstatus = fields_add( bibout, outtag, str_cstr( invalue ), level );
		if ( fstatus==FIELDS_OK ) return BIBL_OK;
		else return BIBL_ERR_MEMERR;
	}
}

static int
biblatexin_bltthesistype( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	char *p = invalue->data;
	int fstatus = FIELDS_OK;
	/* type in the @thesis is used to distinguish Ph.D. and Master's thesis */
	if ( !strncasecmp( p, "phdthesis", 9 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Ph.D. thesis", level );
	} else if ( !strncasecmp( p, "mastersthesis", 13 ) || !strncasecmp( p, "masterthesis", 12 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Masters thesis", level );
	} else if ( !strncasecmp( p, "mathesis", 8 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Masters thesis", level );
	} else if ( !strncasecmp( p, "diploma", 7 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Diploma thesis", level );
	} else if ( !strncasecmp( p, "habilitation", 12 ) ) {
		fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Habilitation thesis", level );
	}
	if ( fstatus==FIELDS_OK ) return BIBL_OK;
	else return BIBL_ERR_MEMERR;
}

static int
biblatexin_bteprint( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int neprint, netype, fstatus;
	char *eprint = NULL, *etype = NULL;

	neprint = fields_find( bibin, "eprint",     LEVEL_ANY );
	netype  = fields_find( bibin, "eprinttype", LEVEL_ANY );

	if ( neprint!=FIELDS_NOTFOUND ) eprint = fields_value( bibin, neprint, FIELDS_CHRP );
	if ( netype!=FIELDS_NOTFOUND )  etype =  fields_value( bibin, netype,  FIELDS_CHRP );

	if ( eprint && etype ) {
		if ( !strncasecmp( etype, "arxiv", 5 ) ) {
			fstatus = fields_add( bibout, "ARXIV", eprint, level );
			if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		} else if ( !strncasecmp( etype, "jstor", 5 ) ) {
			fstatus = fields_add( bibout, "JSTOR", eprint, level );
			if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		} else if ( !strncasecmp( etype, "pubmed", 6 ) ) {
			fstatus = fields_add( bibout, "PMID", eprint, level );
			if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		} else if ( !strncasecmp( etype, "medline", 7 ) ) {
			fstatus = fields_add( bibout, "MEDLINE", eprint, level );
			if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		} else {
			fstatus = fields_add( bibout, "EPRINT", eprint, level );
			if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
			fstatus = fields_add( bibout, "EPRINTTYPE", etype, level );
			if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		}
		fields_set_used( bibin, neprint );
		fields_set_used( bibin, netype );
	} else if ( eprint ) {
		fstatus = fields_add( bibout, "EPRINT", eprint, level );
		if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		fields_set_used( bibin, neprint );
	} else if ( etype ) {
		fstatus = fields_add( bibout, "EPRINTTYPE", etype, level );
		if ( fstatus!=FIELDS_OK ) return BIBL_ERR_MEMERR;
		fields_set_used( bibin, netype );
	}
	return BIBL_OK;
}

static int
biblatexin_btgenre( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	if ( fields_add( bibout, "GENRE:BIBUTILS", str_cstr( invalue ), level ) == FIELDS_OK ) return BIBL_OK;
	else return BIBL_ERR_MEMERR;
}

/* biblatexin_howpublished()
 *
 *    howpublished={},
 *
 * Normally indicates the manner in which something was
 * published in lieu of a formal publisher, so typically
 * 'howpublished' and 'publisher' will never be in the
 * same reference.
 *
 * Occasionally, people put Diploma thesis information
 * into this field, so check for that first.
 */
static int
biblatexin_howpublished( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	int fstatus;

        if ( !strncasecmp( str_cstr( invalue ), "Diplom", 6 ) )
                fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Diploma thesis", level );
        else if ( !strncasecmp( str_cstr( invalue ), "Habilitation", 13 ) )
                fstatus = fields_replace_or_add( bibout, "GENRE:BIBUTILS", "Habilitation thesis", level );
        else
		fstatus = fields_add( bibout, "PUBLISHER", str_cstr( invalue ), level );

	if ( fstatus==FIELDS_OK ) return BIBL_OK;
	else return BIBL_ERR_MEMERR;
}

/*
 * biblatex has multiple editor fields "editor", "editora", "editorb", "editorc",
 * each of which can be modified from a type of "EDITOR" via "editortype",
 * "editoratype", "editorbtype", "editorctype".
 *
 * Defined types:
 *     "editor"
 *     "collaborator"
 *     "compiler"
 *     "redactor"
 *
 *     "reviser" ?
 *     "founder" ?
 *     "continuator" ?
 *
 *  bibtex-chicago
 *
 *     "director"
 *     "producer"
 *     "conductor"
 *     "none" (for performer)
 */
static int
biblatexin_blteditor( fields *bibin, int m, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	char *editor_fields[] = { "editor", "editora", "editorb", "editorc" };
	char *editor_types[]  = { "editortype", "editoratype", "editorbtype", "editorctype" };
	int i, n = 0, ntype, neditors = sizeof( editor_fields ) / sizeof( editor_fields[0] );
	char *type, *usetag = "EDITOR";
	for ( i=1; i<neditors; ++i )
		if ( !strcasecmp( intag->data, editor_fields[i] ) ) n = i;
	ntype = fields_find( bibin, editor_types[n], LEVEL_ANY );
	if ( ntype!=FIELDS_NOTFOUND ) {
		type = fields_value( bibin, ntype, FIELDS_CHRP_NOUSE );
		if ( !strcasecmp( type, "collaborator" ) )  usetag = "COLLABORATOR";
		else if ( !strcasecmp( type, "compiler" ) ) usetag = "COMPILER";
		else if ( !strcasecmp( type, "redactor" ) ) usetag = "REDACTOR";
		else if ( !strcasecmp( type, "director" ) ) usetag = "DIRECTOR";
		else if ( !strcasecmp( type, "producer" ) ) usetag = "PRODUCER";
		else if ( !strcasecmp( type, "none" ) )     usetag = "PERFORMER";
	}
	return biblatex_names( bibout, usetag, invalue, level, &(pm->asis), &(pm->corps) );
}

static int
biblatexin_person( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	return biblatex_names( bibout, outtag, invalue, level, &(pm->asis), &(pm->corps) );
}

static void
biblatexin_notag( param *p, char *tag )
{
	if ( p->verbose && strcmp( tag, "INTERNAL_TYPE" ) ) {
		if ( p->progname ) REprintf( "%s: ", p->progname );
		REprintf( " Cannot find tag '%s'\n", tag );
	}
}

static int
biblatexin_convertf( fields *bibin, fields *bibout, int reftype, param *p )
{
	static int (*convertfns[NUM_REFTYPES])(fields *, int, str *, str *, int, param *, char *, fields *) = {
                // 2020-06-18 Georgi: deal with:
		//   warning: ISO C forbids specifying range of elements to initialize [-Wpedantic]
	        //
		// [ 0 ... NUM_REFTYPES-1 ] = generic_null,
		// [ SIMPLE          ] = generic_simple,
		// [ PAGES           ] = generic_pages,
		// [ NOTES           ] = generic_notes,
		// [ PERSON          ] = biblatexin_person,
		// [ BLT_EDITOR      ] = biblatexin_blteditor,
		// [ HOWPUBLISHED    ] = biblatexin_howpublished,
		// [ URL             ] = generic_url,
		// [ GENRE           ] = biblatexin_btgenre,
		// [ BT_EPRINT       ] = biblatexin_bteprint,
		// [ BLT_THESIS_TYPE ] = biblatexin_bltthesistype,
		// [ BLT_SCHOOL      ] = biblatexin_bltschool,
		// [ BLT_SUBTYPE     ] = biblatexin_bltsubtype,
		// [ BLT_SKIP        ] = generic_skip,
		// [ TITLE           ] = generic_null,    /* delay processing until later */
		
                [ ALWAYS           ] = generic_null,  // (0)
		[ DEFAULT          ] = generic_null,  // (1)
		[ SKIP             ] = generic_null,  // (2)
		[ SIMPLE           ] = generic_simple,  // (3) 
		[ TYPE             ] = generic_null,  // (4) 
		[ PERSON           ] = biblatexin_person,  // (5) 
		[ DATE             ] = generic_null,  // (6) 
		[ PAGES            ] = generic_pages,  // (7) 
		[ SERIALNO         ] = generic_null,  // (8) 
		[ TITLE            ] = generic_null,  // (9) 
		[ NOTES            ] = generic_notes,  // (10)
		[ DOI              ] = generic_null,  // (11)
		[ HOWPUBLISHED     ] = biblatexin_howpublished,  // (12)
		[ LINKEDFILE       ] = generic_null,  // (13)
		[ KEYWORD          ] = generic_null,  // (14)
		[ URL              ] = generic_url,  // (15)
		[ GENRE            ] = biblatexin_btgenre,  // (16)
		[ BT_SENTE         ] = generic_null,  // (17) /* Bibtex 'Sente' */
		[ BT_EPRINT        ] = biblatexin_bteprint,  // (18) /* Bibtex 'Eprint' */
		[ BT_ORG           ] = generic_null,  // (19) /* Bibtex Organization */
		[ BLT_THESIS_TYPE  ] = biblatexin_bltthesistype,  // (20) /* Biblatex Thesis Type */
		[ BLT_SCHOOL       ] = biblatexin_bltschool,  // (21) /* Biblatex School */
		[ BLT_EDITOR       ] = biblatexin_blteditor, // (22) /* Biblatex Editor */
		[ BLT_SUBTYPE      ] = biblatexin_bltsubtype,  // (23) /* Biblatex entrysubtype */
		[ BLT_SKIP         ] = generic_skip,  // (24) /* Biblatex Skip Entry */
		[ EPRINT           ] = generic_null,  // (25)   /* delay processing until later */
	};

	int process, level, i, nfields, status = BIBL_OK;
	str *intag, *invalue;
	char *outtag;

	nfields = fields_num( bibin );
	for ( i=0; i<nfields; ++i ) {

               /* skip ones already "used" such as successful crossref */
                if ( fields_used( bibin, i ) ) continue;

		/* skip ones with no data or no tags (e.g. don't match ALWAYS/DEFAULT entries) */
		intag   = fields_tag  ( bibin, i, FIELDS_STRP_NOUSE );
		invalue = fields_value( bibin, i, FIELDS_STRP_NOUSE );
		if ( str_is_empty( intag ) || str_is_empty( invalue ) ) continue;

		if ( !translate_oldtag( intag->data, reftype, p->all, p->nall, &process, &level, &outtag ) ) {
			biblatexin_notag( p, intag->data );
			continue;
		}

		status = convertfns[ process ]( bibin, i, intag, invalue, level, p, outtag, bibout );
		if ( status!=BIBL_OK ) return status;

		if ( convertfns[ process ] != generic_null )
			fields_set_used( bibin, i );

	}

	status = process_title_all( bibin, bibout, reftype, p );

	if ( status==BIBL_OK && p->verbose ) fields_report_stdout( bibout );

	return status;
}

