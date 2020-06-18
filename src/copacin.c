/*
 * copacin.c
 *
 * Copyright (c) Chris Putnam 2004-2020
 *
 * Program and source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "is_ws.h"
#include "str.h"
#include "str_conv.h"
#include "slist.h"
#include "name.h"
#include "fields.h"
#include "reftypes.h"
#include "bibformats.h"
#include "generic.h"

extern variants copac_all[];
extern int copac_nall;

/*****************************************************
 PUBLIC: void copacin_initparams()
*****************************************************/

static int copacin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, str *line, str *reference, int *fcharset );
static int copacin_processf( fields *bibin, const char *p, const char *filename, long nref, param *pm );
static int copacin_convertf( fields *bibin, fields *info, int reftype, param *pm );

int
copacin_initparams( param *pm, const char *progname )
{
	pm->readformat       = BIBL_COPACIN;
	pm->charsetin        = BIBL_CHARSET_DEFAULT;
	pm->charsetin_src    = BIBL_SRC_DEFAULT;
	pm->latexin          = 0;
	pm->xmlin            = 0;
	pm->utf8in           = 0;
	pm->nosplittitle     = 0;
	pm->verbose          = 0;
	pm->addcount         = 0;
	pm->output_raw       = 0;

	pm->readf    = copacin_readf;
	pm->processf = copacin_processf;
	pm->cleanf   = NULL;
	pm->typef    = NULL;
	pm->convertf = copacin_convertf;
	pm->all      = copac_all;
	pm->nall     = copac_nall;

	slist_init( &(pm->asis) );
	slist_init( &(pm->corps) );

	if ( !progname ) pm->progname = NULL;
	else {
		pm->progname = strdup( progname );
		if ( !pm->progname ) return BIBL_ERR_MEMERR;
	}

	return BIBL_OK;
}

/*****************************************************
 PUBLIC: int copacin_readf()
*****************************************************/

/* Endnote-Refer/Copac tag definition:
    character 1 = alphabetic character
    character 2 = alphabetic character
    character 3 = dash
    character 4 = space
*/
static int
copacin_istag( const char *buf )
{
	if (! ((buf[0]>='A' && buf[0]<='Z')) || (buf[0]>='a' && buf[0]<='z') )
		return 0;
	if (! ((buf[1]>='A' && buf[1]<='Z')) || (buf[1]>='a' && buf[1]<='z') )
		return 0;
	if (buf[2]!='-' ) return 0;
	if (buf[3]!=' ' ) return 0;
	return 1; 
}
static int
readmore( FILE *fp, char *buf, int bufsize, int *bufpos, str *line )
{
	if ( line->len ) return 1;
	else return str_fget( fp, buf, bufsize, bufpos, line );
}

static int
copacin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, str *line, str *reference, int *fcharset )
{
	int haveref = 0, inref=0;
	char *p;
	*fcharset = CHARSET_UNKNOWN;
	while ( !haveref && readmore( fp, buf, bufsize, bufpos, line ) ) {
		/* blank line separates */
		if ( line->data==NULL ) continue;
		if ( inref && line->len==0 ) haveref=1; 
		p = &(line->data[0]);
		/* Recognize UTF8 BOM */
		if ( line->len > 2 &&
				(unsigned char)(p[0])==0xEF &&
				(unsigned char)(p[1])==0xBB &&
				(unsigned char)(p[2])==0xBF ) {
			*fcharset = CHARSET_UNICODE;
			p += 3;
		}
		if ( copacin_istag( p ) ) {
			if ( inref ) str_addchar( reference, '\n' );
			str_strcatc( reference, p );
			inref = 1;
		} else if ( inref ) {
			/* copac puts tag only on 1st line */
			if ( *p ) p++;
			if ( *p ) p++;
			if ( *p ) p++;
			if ( *p ) {
				str_addchar( reference, ' ' );
				str_strcatc( reference, p );
			}
		}
		str_empty( line );
	}
	return haveref;
}

/*****************************************************
 PUBLIC: int copacin_processf()
*****************************************************/

static const char*
copacin_addfield( const char *p, str *tag, str *value )
{
	int i;

	str_empty( tag );
	str_empty( value );

	i = 0;
	while ( i<3 && *p ) {
		str_addchar( tag, *p );
		p++;
		i++;
	}

	while ( *p==' ' || *p=='\t' ) p++;

	while ( *p && *p!='\r' && *p!='\n' ) {
		str_addchar( value, *p );
		p++;
	}

	str_trimendingws( value );

	while ( *p=='\n' || *p=='\r' ) p++;

	return p;
}

static const char *
copacin_nextline( const char *p )
{
	while ( *p && *p!='\n' && *p!='\r') p++;
	while ( *p=='\n' || *p=='\r' ) p++;
	return p;
}

static int
copacin_processf( fields *copacin, const char *p, const char *filename, long nref, param *pm )
{
	int status, ret = 1;
	str tag, value;

	str_init( &tag );
	str_init( &value );

	while ( *p ) {

		p = skip_ws( p );

		if ( copacin_istag( p ) ) {
			p = copacin_addfield( p, &tag, &value );
			/* don't add empty strings */
			if ( str_has_value( &tag ) && str_has_value( &value ) ) {
				status = fields_add( copacin, str_cstr( &tag ), str_cstr( &value ), LEVEL_MAIN );
				if ( status!=FIELDS_OK ) {
					ret = 0;
					goto out;
				}
			}
		}

		else {
			p = copacin_nextline( p );
		}
	}

out:
	str_free( &tag );
	str_free( &value );

	return ret;
}

/*****************************************************
 PUBLIC: int copacin_convertf(), returns BIBL_OK or BIBL_ERR_MEMERR
*****************************************************/

/* copac names appear to always start with last name first, but don't
 * always seem to have a comma after the name
 *
 * editors seem to be stuck in as authors with the tag "[Editor]" in it
 */
static int
copacin_person( fields *bibin, int n, str *intag, str *invalue, int level, param *pm, char *outtag, fields *bibout )
{
	char *usetag = outtag, editor[]="EDITOR";
	int comma = 0, i, ok, status;
	str usename, *s;
	slist tokens;

	if ( slist_find( &(pm->asis),  invalue ) !=-1  ||
	     slist_find( &(pm->corps), invalue ) !=-1 ) {
		ok = name_add( bibout, outtag, str_cstr( invalue ), level, &(pm->asis), &(pm->corps) );
		if ( ok ) return BIBL_OK;
		else return BIBL_ERR_MEMERR;
	}

	slist_init( &tokens );
	str_init( &usename );

	status = slist_tokenize( &tokens, invalue, " ", 1 );
	if ( status!=SLIST_OK ) return BIBL_ERR_MEMERR;

	for ( i=0; i<tokens.n; ++i ) {
		s = slist_str( &tokens, i );
		if ( !strcmp( str_cstr( s ), "[Editor]" ) ) {
			usetag = editor;
			str_empty( s );
		} else if ( s->len && s->data[s->len-1]==',' ) {
			comma++;
		}
	}

	if ( comma==0 && tokens.n ) {
		s = slist_str( &tokens, 0 );
		str_addchar( s, ',' );
	}

	for ( i=0; i<tokens.n; ++i ) {
		s = slist_str( &tokens, i );
		if ( str_is_empty( s ) ) continue;
		if ( i ) str_addchar( &usename, ' ' );
		str_strcat( &usename, s );
	}

	slist_free( &tokens );

	ok = name_add( bibout, usetag, str_cstr( &usename ), level, &(pm->asis), &(pm->corps) );

	str_free( &usename );

	if ( ok ) return BIBL_OK;
	else return BIBL_ERR_MEMERR;
}

static void
copacin_report_notag( param *p, char *tag )
{
	if ( p->verbose ) {
		if ( p->progname ) REprintf( "%s: ", p->progname );
		REprintf( "Cannot find tag '%s'\n", tag );
	}
}

static int
copacin_convertf( fields *bibin, fields *bibout, int reftype, param *p )
{
	static int (*convertfns[NUM_REFTYPES])(fields *, int, str *, str *, int, param *, char *, fields *) = {
		// [ 0 ... NUM_REFTYPES-1 ] = generic_null,
		// [ SIMPLE       ] = generic_simple,
		// [ TITLE        ] = generic_title,
		// [ NOTES        ] = generic_notes,
		// [ SERIALNO     ] = generic_serialno,
		// [ PERSON       ] = copacin_person

                [ ALWAYS           ] = generic_null,  // (0)
		[ DEFAULT          ] = generic_null,  // (1)
		[ SKIP             ] = generic_null,  // (2)
		[ SIMPLE           ] = generic_simple,  // (3) 
		[ TYPE             ] = generic_null,  // (4) 
		[ PERSON           ] = copacin_person,  // (5) 
		[ DATE             ] = generic_null,  // (6) 
		[ PAGES            ] = generic_null,  // (7) 
		[ SERIALNO         ] = generic_serialno,  // (8) 
		[ TITLE            ] = generic_title, // (9) 
		[ NOTES            ] = generic_notes, // (10)
		[ DOI              ] = generic_null,  // (11)
		[ HOWPUBLISHED     ] = generic_null,  // (12)
		[ LINKEDFILE       ] = generic_null,  // (13)
		[ KEYWORD          ] = generic_null,  // (14)
		[ URL              ] = generic_null,  // (15)
		[ GENRE            ] = generic_null,  // (16)
		[ BT_SENTE         ] = generic_null,  // (17) /* Bibtex 'Sente' */
		[ BT_EPRINT        ] = generic_null,  // (18) /* Bibtex 'Eprint' */
		[ BT_ORG           ] = generic_null,  // (19) /* Bibtex Organization */
		[ BLT_THESIS_TYPE  ] = generic_null,  // (20) /* Biblatex Thesis Type */
		[ BLT_SCHOOL       ] = generic_null,  // (21) /* Biblatex School */
		[ BLT_EDITOR       ] = generic_null,  // (22) /* Biblatex Editor */
		[ BLT_SUBTYPE      ] = generic_null,  // (23) /* Biblatex entrysubtype */
		[ BLT_SKIP         ] = generic_null,  // (24) /* Biblatex Skip Entry */
		[ EPRINT           ] = generic_null   // (25)

	};

	int  process, level, i, nfields, status = BIBL_OK;
	str *intag, *invalue;
	char *outtag;

	nfields = fields_num( bibin );
	for ( i=0; i<nfields; ++i ) {

		intag = fields_tag( bibin, i, FIELDS_STRP );

		if ( !translate_oldtag( str_cstr( intag ), reftype, p->all, p->nall, &process, &level, &outtag ) ) {
			copacin_report_notag( p, str_cstr( intag ) );
			continue;
		}

		invalue = fields_value( bibin, i, FIELDS_STRP );

		status = convertfns[ process ] ( bibin, i, intag, invalue, level, p, outtag, bibout );
		if ( status!=BIBL_OK ) return status;

	}

	return status;
}
