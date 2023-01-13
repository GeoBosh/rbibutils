/*
 * bibentrydirectout.c  (based on bibtexout.c)
 *
 * Copyright (c) Georgi N. Boshnakov 2020-2023
 *
 * Program and source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "str.h"
#include "strsearch.h"
#include "utf8.h"
#include "xml.h"
#include "fields.h"
#include "generic.h"
#include "name.h"
#include "title.h"
#include "type.h"
#include "url.h"
#include "bibformats.h"

#include "common_bt_blt_btd_out.h"
#include "common_beout.h"

/*****************************************************
 PUBLIC: int bibentrydirectout_initparams()
*****************************************************/

static int  bibentrydirectout_write( fields *in, FILE *fp, param *p, unsigned long refnum );
static int  bibentrydirectout_assemble( fields *in, fields *out, param *pm, unsigned long refnum );

int
bibentrydirectout_initparams( param *pm, const char *progname )
{
	pm->writeformat      = BIBL_BIBENTRYOUT;
	pm->format_opts      = 0;
	pm->charsetout       = BIBL_CHARSET_DEFAULT;
	pm->charsetout_src   = BIBL_SRC_DEFAULT;
	pm->latexout         = 1;
	pm->utf8out          = BIBL_CHARSET_UTF8_DEFAULT;
	pm->utf8bom          = BIBL_CHARSET_BOM_DEFAULT;
	pm->xmlout           = BIBL_XMLOUT_FALSE;
	pm->nosplittitle     = 0;
	pm->verbose          = 0;
	pm->addcount         = 0;
	pm->singlerefperfile = 0;

	pm->headerf   = bibentrydirectout_writeheader; // generic_writeheader;
	pm->footerf   = bibentrydirectout_writefooter; // NULL;
	pm->assemblef = bibentrydirectout_assemble;
	pm->writef    = bibentrydirectout_write;

	if ( !pm->progname ) {
		if ( !progname ) pm->progname = NULL;
		else {
			pm->progname = strdup( progname );
			if ( !pm->progname ) return BIBL_ERR_MEMERR;
		}
	}

	return BIBL_OK;
}

/*****************************************************
 PUBLIC: int bibentrydirectout_assemble()
*****************************************************/

static void
append_simple_quoted_tag( fields *in, char *intag, char *outtag, fields *out, int *status )
{
	int n, fstatus;
	str qtag;

	str_init( &qtag);
 
	n = fields_find( in, intag, LEVEL_ANY );
	if ( n!=FIELDS_NOTFOUND ) {
		fields_set_used( in, n );

		str_strcatc( &qtag, "\"" );
		str_strcatc( &qtag, outtag );
		str_strcatc( &qtag, "\"" );
		
		fstatus = fields_add( out, qtag.data, fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}

	str_free( &qtag );

}

// Georgi  new 2023-01-03; TODO: currently recognises bibtex types only
static int
bibentryout_type( char *fld_val )
{
  int len = strlen(fld_val);

  switch(len) {
  case  4:
    if ( !strcmp( fld_val, "Book" ) )  return TYPE_BOOK;
    if ( !strcmp( fld_val, "Misc" ) )  return TYPE_MISC;
    break;

  case  6:
    if ( !strcmp( fld_val, "Inbook" ) )  return TYPE_INBOOK;
    if ( !strcmp( fld_val, "Manual" ) )  return TYPE_MANUAL;
    break;

  case  7:
    if ( !strcmp( fld_val, "Article" ) ) return TYPE_ARTICLE;
    break;

  case  9:
    if ( !strcmp( fld_val, "PhdThesis" ) )  return TYPE_PHDTHESIS;
    break;

  case 10:
    if ( !strcmp( fld_val, "TechReport" ) )  return TYPE_REPORT;
    if ( !strcmp( fld_val, "Collection" ) )  return TYPE_COLLECTION;
    if ( !strcmp( fld_val, "Electronic" ) )  return TYPE_ELECTRONIC;
    break;

  case 11:
    if ( !strcmp( fld_val, "Proceedings" ) ) return TYPE_PROCEEDINGS;
    if ( !strcmp( fld_val, "Unpublished" ) )  return TYPE_UNPUBLISHED;
    break;

  case 12:
    if ( !strcmp( fld_val, "InCollection" ) ) return TYPE_INCOLLECTION;
    break;

  case 13:
    if ( !strcmp( fld_val, "InProceedings" ) )  return TYPE_INPROCEEDINGS;
    if ( !strcmp( fld_val, "MastersThesis" ) )  return TYPE_MASTERSTHESIS;
    if ( !strcmp( fld_val, "DiplomaThesis" ) )  return TYPE_DIPLOMATHESIS;
    break;

  default:
    // TODO: this is for Rdpack which erroneously accepted it in a package
    //       remove support for this in a future version of rbibutils?
    if ( !strcmp( fld_val, "online" ) )  return TYPE_MISC;
    break;
  }
  return TYPE_UNKNOWN; // 0
}

static int
bibentrydirectout_assemble( fields *in, fields *out, param *pm, unsigned long refnum )
{
  int type, status = BIBL_OK;

  // // Georgi; for testing
  // fields_report_stderr(in);

  int n, fstatus;
  char *fld_val;
  n = fields_find( in, "INTERNAL_TYPE", LEVEL_ANY );

  // REprintf("\nassemble: INTERNAL_TYPE = %d\n", n);
  // REprintf("\nassemble: FIELDS_NOTFOUND = %d\n", FIELDS_NOTFOUND);
	
  // append_type        ( type, out, &status );
  if ( n!=FIELDS_NOTFOUND ) {
    fields_set_used( in, n );
    fld_val = fields_value( in, n, FIELDS_CHRP );

    type = bibentryout_type(fld_val);
      
    // REprintf("(bibentrydirectout_assemble): fld_val=%s\n", fld_val);
    //  REprintf("type = %d\n\n", type);
      
    if ( strcmp( fld_val, "online" ) )
      fstatus = fields_add( out, "bibtype", fld_val, LEVEL_MAIN );
    else   // TODO: this is temporary patch!
      fstatus = fields_add( out, "bibtype", "Misc", LEVEL_MAIN );

  } else {
    type = TYPE_MISC; // default to Misc; TODO: issue a message?
    fstatus = fields_add( out, "bibtype", "Misc", LEVEL_MAIN );
  }
  if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;

     // append_citekey     ( in, out, pm->format_opts, &status );
     append_simple      ( in, "REFNUM", "refnum", out, &status );
	
     append_people_be      ( in, "AUTHOR",     "AUTHOR:CORP",     "AUTHOR:ASIS",     "author", LEVEL_MAIN, out, pm->format_opts, pm->latexout, &status );
     append_people_be      ( in, "EDITOR",     "EDITOR:CORP",     "EDITOR:ASIS",     "editor", LEVEL_ANY, out, pm->format_opts, pm->latexout, &status );
     append_people_be      ( in, "TRANSLATOR", "TRANSLATOR:CORP", "TRANSLATOR:ASIS", "translator", LEVEL_ANY, out, pm->format_opts, pm->latexout, &status );
     append_titles      ( in, type, out, pm->format_opts, &status );
     append_date        ( in, out, &status );
     append_simple      ( in, "EDITION",            "edition",   out, &status );

     append_simple      ( in, "INSTITUTION",        "institution", out, &status );
     append_simple      ( in, "PUBLISHER",          "publisher", out, &status );


     append_simple      ( in, "ADDRESS",            "address",   out, &status );
     append_simple      ( in, "VOLUME",             "volume",    out, &status );
     append_issue_number( in, out, &status );
     append_pages       ( in, out, pm->format_opts, &status );
     append_keywords    ( in, out, &status );
     append_simple      ( in, "CONTENTS",           "contents",  out, &status );
     append_simple      ( in, "ABSTRACT",           "abstract",  out, &status );
     append_simple      ( in, "LOCATION",           "location",  out, &status );
     append_simple      ( in, "DEGREEGRANTOR",      "school",    out, &status );
     append_simple      ( in, "DEGREEGRANTOR:ASIS", "school",    out, &status );
     append_simple      ( in, "DEGREEGRANTOR:CORP", "school",    out, &status );
     append_simpleall   ( in, "NOTES",              "note",      out, &status );
     append_simpleall   ( in, "ANNOTE",             "annote",    out, &status );
     append_simple      ( in, "ISBN",               "isbn",      out, &status );
     append_simple      ( in, "ISSN",               "issn",      out, &status );
     append_simple      ( in, "MRNUMBER",           "mrnumber",  out, &status );
     append_simple      ( in, "CODEN",              "coden",     out, &status );
     append_simple      ( in, "DOI",                "doi",       out, &status );
     append_urls        ( in, out, &status );
     append_fileattach  ( in, out, &status );
     append_arxiv       ( in, out, &status );
     append_simple      ( in, "EPRINTCLASS",        "primaryClass", out, &status );
     append_isi         ( in, out, &status );
     append_simple      ( in, "LANGUAGE",           "language",  out, &status );
     append_howpublished( in, out, &status );

     append_simple      ( in, "CHAPTER",           "chapter",  out, &status ); // Georgi

     // Georgi - some entries may have field 'key' (it is used by some bibtex styles)
     //       other = c(key = "mykey")
     append_key      ( in, "KEY",   "other"        ,  out, &status );

     int i, f_len;
     char * fld_tag;

     f_len = fields_num( in );
     for ( i=0; i<f_len; ++i ) {
	  if( !fields_used(in, i) ){
	       fld_tag = fields_tag( in, i, FIELDS_CHRP );
	       fld_val = fields_value( in, i, FIELDS_CHRP );

	       // for(int i = 0; str[i]; i++){
	       //   str[i] = tolower(str[i]);
	       // }

	       append_simple_quoted_tag( in, fld_tag, fld_tag, out, &status );
	  }
     }
	
     return status;
}

/*****************************************************
 PUBLIC: int bibentrydirectout_write()
*****************************************************/

static int
bibentrydirectout_write( fields *out, FILE *fp, param *pm, unsigned long refnum )
{
  int i, j, len; // nquotes, format_opts = pm->format_opts;
	char *tag, *value, ch;
	int not_person, not_other; // Georgi

	fprintf( fp, ",\n\n" ); // Georgi
	
	/* ...output type information "@article{" */
	value = ( char * ) fields_value( out, 0, FIELDS_CHRP );
	// if ( !(format_opts & BIBL_FORMAT_BIBOUT_UPPERCASE) ) fprintf( fp, "@%s{", value );
	// else {
	// 	len = (value) ? strlen( value ) : 0;
	// 	fprintf( fp, "@" );
	// 	for ( i=0; i<len; ++i )
	// 		fprintf( fp, "%c", toupper((unsigned char)value[i]) );
	// 	fprintf( fp, "{" );
	// }
	len = (value) ? strlen( value ) : 0;
	fprintf( fp, "  bibentry(bibtype = \"" );
	if(len > 0)
	    fprintf( fp, "%c", toupper((unsigned char)value[0]) );
	for (i=1; i<len; ++i )
		fprintf( fp, "%c", tolower((unsigned char)value[i]) );
	fprintf( fp, "\"" );

	/* ...output refnum "Smith2001" */
	value = ( char * ) fields_value( out, 1, FIELDS_CHRP );
	// fprintf( fp, "%s", value );
	fprintf( fp, ",\n      key = \"%s\"", value );

	/* ...rest of the references */
	for ( j=2; j<out->n; ++j ) {
	        // nquotes = 0;
		tag   = ( char * ) fields_tag( out, j, FIELDS_CHRP );
		value = ( char * ) fields_value( out, j, FIELDS_CHRP );
		fprintf( fp, ",\n      " );

		// if ( format_opts & BIBL_FORMAT_BIBOUT_WHITESPACE ) fprintf( fp, "  " );
		// if ( !(format_opts & BIBL_FORMAT_BIBOUT_UPPERCASE ) ) fprintf( fp, "%s", tag );
		// else {
		// 	len = strlen( tag );
		// 	for ( i=0; i<len; ++i )
		// 		fprintf( fp, "%c", toupper((unsigned char)tag[i]) );
		// }
		fprintf( fp, "%s", tag );
		
		// if ( format_opts & BIBL_FORMAT_BIBOUT_WHITESPACE ) fprintf( fp, " = \t" );
		// else fprintf( fp, "=" );
		fprintf( fp, " = " );

		// if ( format_opts & BIBL_FORMAT_BIBOUT_BRACKETS ) fprintf( fp, "{" );
		// else fprintf( fp, "\"" );
		not_person = strcmp( tag, "author" ) && strcmp( tag, "editor" ) 
		  && strcmp( tag, "translator" );  // TODO: are there others?

		//
		not_other = strcmp( tag, "other" );
		
		if ( not_person && not_other ) fprintf( fp, "\"" );

		len = strlen( value );
		for ( i=0; i<len; ++i ) {
			ch = value[i];
			// if ( ch!='\"' ) fprintf( fp, "%c", ch );
			// else {
			// 	if ( format_opts & BIBL_FORMAT_BIBOUT_BRACKETS || ( i>0 && value[i-1]=='\\' ) )
			// 		fprintf( fp, "\"" );
			// 	else {
			// 		if ( nquotes % 2 == 0 )
			// 			fprintf( fp, "``" );
			// 		else    fprintf( fp, "\'\'" );
			// 		nquotes++;
			// 	}
			// }
			
			if ( ch == '\\' ) {
			  fprintf( fp, "%c%c", ch, ch );
			}
			else if ( ch == '\"' &&
				  ( (not_person && not_other) || (i>0 && value[i-1]=='\\') ))
			  fprintf( fp, "\\%c", ch );
			  
			else		       fprintf( fp, "%c"  , ch );
		}

		// if ( format_opts & BIBL_FORMAT_BIBOUT_BRACKETS ) fprintf( fp, "}" );
		// else fprintf( fp, "\"" );
		if ( not_person && not_other )
		  fprintf( fp, "\"" );
		
	}

	/* ...finish reference */
	// if ( format_opts & BIBL_FORMAT_BIBOUT_FINALCOMMA ) fprintf( fp, "," );
	// fprintf( fp, "\n}\n\n" );
	fprintf( fp, " )" );

	fflush( fp );

	return BIBL_OK;
}
