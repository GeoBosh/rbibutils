/*
 * bibtexin.c
 *
 * Copyright (c) Chris Putnam 2003-2020
 * Copyright (c) Georgi N. Boshnakov 2020-2023
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

#include "common_bt_btd_blt.h"
#include "common_bt_btd.h"

extern slist find;
extern slist replace;

/*****************************************************
 PUBLIC: void bibtexin_initparams()
*****************************************************/

static int bibtexin_convertf( fields *bibin, fields *info, int reftype, param *p );
static int bibtexin_cleanf( bibl *bin, param *p );

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

	//    note that 'find' and 'replace' work in tandem, so both need to be cleared.
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
 PUBLIC: void bibtexin_cleanf()
*****************************************************/

static int
bibtexin_cleanref( fields *bibin, param *pm )
{
     int i, n, fstatus, status = BIBL_OK;
     str *tag, *value;
     intlist toremove;

     intlist_init( &toremove );

     n = fields_num( bibin );

     // REprintf("\nbibtexdirectin_cleanref (start): n = %d\n" , n);
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
