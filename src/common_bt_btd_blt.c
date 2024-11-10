/*
 * common_bt_btd_blt.c
 *
 * Copyright (c) Chris Putnam 2003-2020
 * Copyright (c) Georgi N. Boshnakov 2020-2023
 *
 * File created by taking out parts from bibtexout.c, bibtexdirectout.c and
 * biblatexout.c that are common to all three of them.
 *
 * Program and source code released under the GPL version 2
 *
 */
#include "slist.h"

#include "common_bt_btd_blt.h"

slist find    = { 0, 0, 0, NULL };
slist replace = { 0, 0, 0, NULL };

static char *dummy_id = "dummyid";

/*****************************************************
 PUBLIC: int bibtexdirectin_readf()
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
int
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

const char *
process_bibtexid( const char *p, str *id )
{
	const char *start_p = p;
	str tmp;

	str_init( &tmp );
	p = str_cpytodelim( &tmp, p, ",", 1 );

	if ( str_has_value( &tmp ) ) {
		if ( strchr( tmp.data, '=' ) ) {
			/* Endnote writes bibtex/biblatex files w/o fields, try to
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

	// Georgi
	str_trimstartingws(id);
	str_trimendingws(id);
	
	// REprintf("id = %s, this should not be on new line\n", id->data);
 
	str_free( &tmp );
	return skip_ws( p );
}


// @string

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

static int
token_is_escaped( str *s )
{
	if ( s->data[0]=='\"' && s->data[s->len-1]=='\"' ) return ESCAPED_QUOTES;
	if ( s->data[0]=='{'  && s->data[s->len-1]=='}'  ) return ESCAPED_BRACES;
	return NOT_ESCAPED;
}

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
					if ( status!=BIBL_OK ) { p=NULL; goto outerr; }
				}
			}
		} else if ( *p=='{' ) {
			str_addchar( &token, *p );
			if ( !brace_is_escaped( nquotes, p, startp ) ) {
				nbraces++;
			}
		} else if ( *p=='}' ) {
			str_addchar( &token, *p );
			if ( !brace_is_escaped( nquotes, p, startp ) ) {
				nbraces--;
				if ( nbraces==0 ) {
					status = add_token( tokens, &token );
					if ( status!=BIBL_OK ) { p=NULL; goto outerr; }
				}
				if ( nbraces<0 ) {
					goto out;
				}
			}
		} else if ( *p=='#' ) {
			if ( char_is_escaped( nquotes, nbraces ) ) {
				str_addchar( &token, *p );
			}
			/* ...this is a bibtex string concatentation token */
			else {
				if ( str_has_value( &token ) ) {
					status = add_token( tokens, &token );
					if ( status!=BIBL_OK ) { p=NULL; goto outerr; }
				}
				status = slist_addc( tokens, "#" );
				if ( status!=SLIST_OK ) { p=NULL; goto outerr; }
			}
		} else if ( !is_ws( *p ) || char_is_escaped( nquotes, nbraces ) ) {
		        /* ...add escaped white-space and non-white-space to current token */

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
                          if ( status!=BIBL_OK ) { p=NULL; goto outerr; }
			}
		}

		p++;
	}
out:
	if ( nbraces!=0 ) {
	  REprintf( "%s: Mismatch in number of braces in file %s in reference %ld.\n", currloc->progname, currloc->filename, currloc->nref );
	}
	if ( nquotes!=0 ) {
	  REprintf( "%s: Mismatch in number of quotes in file %s in reference %ld.\n", currloc->progname, currloc->filename, currloc->nref );
	}
	if ( str_has_value( &token ) ) {
		if ( str_memerr( &token ) ) { p = NULL; goto out; }
		status = slist_add( tokens, &token );
		if ( status!=SLIST_OK ) p = NULL;
	}
outerr:
	str_free( &token );
	return p;
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

/* return NULL on memory error */
const char *
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

/* process_bibtextype()
 *
 * extract 'article', 'book', etc. from:
 *
 * @article{...}
 * @book(...)
 *
 * return pointer after '{' or '(' character
 */
const char*
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
int
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
			if ( str_has_value( &s2 ) ) t = slist_set( &replace, n, &s2 );
			else t = slist_setc( &replace, n, "" );
			if ( t==NULL ) { status = BIBL_ERR_MEMERR; goto out; }
		}
	}
out:
	strs_free( &s1, &s2, NULL );
	return status;
}

static void
bibtexin_nocrossref( bibl *bin, long i, int n, param *p )
{
	int n1 = fields_find( bin->ref[i], "REFNUM", LEVEL_ANY );
	if ( p->progname ) REprintf( "%s: ", p->progname );
	REprintf( "Cannot find cross-reference '%s'", (char*)fields_value( bin->ref[i], n, FIELDS_CHRP_NOUSE ) );
	if ( n1!=FIELDS_NOTFOUND )
	  REprintf( " for reference '%s'", (char*)fields_value( bin->ref[i], n1, FIELDS_CHRP_NOUSE ) );
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

int
bibtexin_crossref( bibl *bin, param *p )
{
	int n, ncross, status = BIBL_OK;
	fields *bibref, *bibcross;
	long i;

	for ( i=0; i<bin->n; ++i ) {
		bibref = bin->ref[i];
		n = fields_find( bibref, "CROSSREF", LEVEL_ANY );
		if ( n==FIELDS_NOTFOUND ) continue;
		fields_set_used( bibref, n );
		ncross = bibl_findref( bin, (char*) fields_value(bibref, n, FIELDS_CHRP_NOUSE) );
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

/*****************************************************
 PUBLIC: int bibtexdirectin_typef()
*****************************************************/

int
bibtexin_typef( fields *bibin, const char *filename, int nrefs, param *p )
{
	int ntypename, nrefname, is_default;
	char *refname = "", *typename = "";
	
// REprintf("(bibtexin_typef:)\n");
// fields_report_stderr( bibin );  // Testing only
 
	ntypename = fields_find( bibin, "INTERNAL_TYPE", LEVEL_MAIN );
	nrefname  = fields_find( bibin, "REFNUM",        LEVEL_MAIN );
	if ( nrefname!=FIELDS_NOTFOUND )  refname  = fields_value( bibin, nrefname,  FIELDS_CHRP_NOUSE );
	if ( ntypename!=FIELDS_NOTFOUND ) typename = fields_value( bibin, ntypename, FIELDS_CHRP_NOUSE );
// REprintf("(bibtexin_typef) typename = %s\n", typename);
// REprintf("(bibtexin_typef) refname = %s\n", typename);

	return get_reftype( typename, nrefs, p->progname, p->all, p->nall, refname, &is_default, REFTYPE_CHATTY );
}
