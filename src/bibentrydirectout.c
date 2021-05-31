/*
 * bibentrydirectout.c  (based on bibtexout.c)
 *
 * Copyright (c) Georgi N. Boshnakov 2021
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

/*****************************************************
 PUBLIC: int bibentrydirectout_initparams()
*****************************************************/

static int  bibentrydirectout_write( fields *in, FILE *fp, param *p, unsigned long refnum );
static int  bibentrydirectout_assemble( fields *in, fields *out, param *pm, unsigned long refnum );


void bibentrydirectout_writeheader( FILE *outptr, param *pm )
{
  fprintf( outptr, "c( #" );
}

void bibentrydirectout_writefooter( FILE *outptr)
{
  fprintf( outptr, "\n)" );
}


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

enum {
	TYPE_UNKNOWN = 0,
	TYPE_ARTICLE,
	TYPE_INBOOK,
	TYPE_INPROCEEDINGS,
	TYPE_PROCEEDINGS,
	TYPE_INCOLLECTION,
	TYPE_COLLECTION,
	TYPE_BOOK,
	TYPE_PHDTHESIS,
	TYPE_MASTERSTHESIS,
	TYPE_DIPLOMATHESIS,
	TYPE_REPORT,
	TYPE_MANUAL,
	TYPE_UNPUBLISHED,
	TYPE_ELECTRONIC,
	TYPE_MISC,
	NUM_TYPES
};

// static int
// bibentrydirectout_type( fields *in, const char *progname, const char *filename, unsigned long refnum )
// {
// 	match_type genre_matches[] = {
// 		{ "periodical",             TYPE_ARTICLE,       LEVEL_ANY  },
// 		{ "academic journal",       TYPE_ARTICLE,       LEVEL_ANY  },
// 		{ "magazine",               TYPE_ARTICLE,       LEVEL_ANY  },
// 		{ "newspaper",              TYPE_ARTICLE,       LEVEL_ANY  },
// 		{ "article",                TYPE_ARTICLE,       LEVEL_ANY  },
// 		{ "instruction",            TYPE_MANUAL,        LEVEL_ANY  },
// 		{ "book",                   TYPE_BOOK,          LEVEL_MAIN },
// 		{ "book",                   TYPE_INBOOK,        LEVEL_ANY  },
// 		{ "book chapter",           TYPE_INBOOK,        LEVEL_ANY  },
// 		{ "unpublished",            TYPE_UNPUBLISHED,   LEVEL_ANY  },
// 		{ "manuscript",             TYPE_UNPUBLISHED,   LEVEL_ANY  },
// 		{ "conference publication", TYPE_PROCEEDINGS,   LEVEL_MAIN },
// 		{ "conference publication", TYPE_INPROCEEDINGS, LEVEL_ANY  },
// 		{ "collection",             TYPE_COLLECTION,    LEVEL_MAIN },
// 		{ "collection",             TYPE_INCOLLECTION,  LEVEL_ANY  },
// 		{ "report",                 TYPE_REPORT,        LEVEL_ANY  },
// 		{ "technical report",       TYPE_REPORT,        LEVEL_ANY  },
// 		{ "Masters thesis",         TYPE_MASTERSTHESIS, LEVEL_ANY  },
// 		{ "Diploma thesis",         TYPE_DIPLOMATHESIS, LEVEL_ANY  },
// 		{ "Ph.D. thesis",           TYPE_PHDTHESIS,     LEVEL_ANY  },
// 		{ "Licentiate thesis",      TYPE_PHDTHESIS,     LEVEL_ANY  },
// 		{ "thesis",                 TYPE_PHDTHESIS,     LEVEL_ANY  },
// 		{ "electronic",             TYPE_ELECTRONIC,    LEVEL_ANY  },
// 		{ "miscellaneous",          TYPE_MISC,          LEVEL_ANY  },
// 	};
// 	int ngenre_matches = sizeof( genre_matches ) / sizeof( genre_matches[0] );
// 
// 	match_type resource_matches[] = {
// 		{ "moving image",           TYPE_ELECTRONIC,    LEVEL_ANY  },
// 		{ "software, multimedia",   TYPE_ELECTRONIC,    LEVEL_ANY  },
// 	};
// 	int nresource_matches = sizeof( resource_matches ) /sizeof( resource_matches[0] );
// 
// 	match_type issuance_matches[] = {
// 		{ "monographic",            TYPE_BOOK,          LEVEL_MAIN },
// 		{ "monographic",            TYPE_INBOOK,        LEVEL_ANY  },
// 	};
// 	int nissuance_matches = sizeof( issuance_matches ) / sizeof( issuance_matches[0] );
// 
// 	int type, maxlevel, n;
// 
// 	type = type_from_mods_hints( in, TYPE_FROM_GENRE, genre_matches, ngenre_matches, TYPE_UNKNOWN );
// 	if ( type==TYPE_UNKNOWN ) type = type_from_mods_hints( in, TYPE_FROM_RESOURCE, resource_matches, nresource_matches, TYPE_UNKNOWN );
// 	if ( type==TYPE_UNKNOWN ) type = type_from_mods_hints( in, TYPE_FROM_ISSUANCE, issuance_matches, nissuance_matches, TYPE_UNKNOWN );
// 
// 	/* default to TYPE_MISC */
// 	if ( type==TYPE_UNKNOWN ) {
// 		maxlevel = fields_maxlevel( in );
// 		if ( maxlevel > 0 ) type = TYPE_MISC;
// 		else {
// 			if ( progname ) REprintf( "%s: ", progname );
// 			REprintf( "Cannot identify TYPE in reference %lu ", refnum+1 );
// 			n = fields_find( in, "REFNUM", LEVEL_ANY );
// 			if ( n!=FIELDS_NOTFOUND ) 
// 				REprintf( " %s", (char*) fields_value( in, n, FIELDS_CHRP ) );
// 			REprintf( " (defaulting to @Misc)\n" );
// 			type = TYPE_MISC;
// 		}
// 	}
// 	return type;
// }


// Georgi
//     TODO: consolidate with append_type, 
// static int
// is_TechReport_type( int type )
// {
// 	char *typenames[ NUM_TYPES ] = {
// 		[ TYPE_ARTICLE       ] = "Article",
// 		[ TYPE_INBOOK        ] = "Inbook",
// 		[ TYPE_PROCEEDINGS   ] = "Proceedings",
// 		[ TYPE_INPROCEEDINGS ] = "InProceedings",
// 		[ TYPE_BOOK          ] = "Book",
// 		[ TYPE_PHDTHESIS     ] = "PhdThesis",
// 		[ TYPE_MASTERSTHESIS ] = "MastersThesis",
// 		[ TYPE_DIPLOMATHESIS ] = "MastersThesis",
// 		[ TYPE_REPORT        ] = "TechReport",
// 		[ TYPE_MANUAL        ] = "Manual",
// 		[ TYPE_COLLECTION    ] = "Collection",
// 		[ TYPE_INCOLLECTION  ] = "InCollection",
// 		[ TYPE_UNPUBLISHED   ] = "Unpublished",
// 		[ TYPE_ELECTRONIC    ] = "Electronic",
// 		[ TYPE_MISC          ] = "Misc",
// 	};
// 	
// 	return( !strcmp(typenames[ type ], "TechReport") );
// }


// static void
// append_type( int type, fields *out, int *status )
// {
// 	char *typenames[ NUM_TYPES ] = {
// 		[ TYPE_ARTICLE       ] = "Article",
// 		[ TYPE_INBOOK        ] = "Inbook",
// 		[ TYPE_PROCEEDINGS   ] = "Proceedings",
// 		[ TYPE_INPROCEEDINGS ] = "InProceedings",
// 		[ TYPE_BOOK          ] = "Book",
// 		[ TYPE_PHDTHESIS     ] = "PhdThesis",
// 		[ TYPE_MASTERSTHESIS ] = "MastersThesis",
// 		[ TYPE_DIPLOMATHESIS ] = "MastersThesis",
// 		[ TYPE_REPORT        ] = "TechReport",
// 		[ TYPE_MANUAL        ] = "Manual",
// 		[ TYPE_COLLECTION    ] = "Collection",
// 		[ TYPE_INCOLLECTION  ] = "InCollection",
// 		[ TYPE_UNPUBLISHED   ] = "Unpublished",
// 		[ TYPE_ELECTRONIC    ] = "Electronic",
// 		[ TYPE_MISC          ] = "Misc",
// 	};
// 	int fstatus;
// 	char *s;
// 
// 	if ( type < 0 || type >= NUM_TYPES ) type = TYPE_MISC;
// 	s = typenames[ type ];
// 
// 	fstatus = fields_add( out, "TYPE", s, LEVEL_MAIN );
// 	if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
// }

// static void
// append_citekey( fields *in, fields *out, int format_opts, int *status )
// {
// 	int n, fstatus;
// 	str s;
// 	char *p;
// 
// 	n = fields_find( in, "REFNUM", LEVEL_ANY );
// 	if ( ( format_opts & BIBL_FORMAT_BIBOUT_DROPKEY ) || n==FIELDS_NOTFOUND ) {
// 		fstatus = fields_add( out, "REFNUM", "", LEVEL_MAIN );
// 		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
// 	}
// 
// 	else {
// 		str_init( &s );
// 		p = fields_value( in, n, FIELDS_CHRP );
// 		while ( p && *p && *p!='|' ) {
// 			if ( format_opts & BIBL_FORMAT_BIBOUT_STRICTKEY ) {
// 				if ( isdigit((unsigned char)*p) || (*p>='A' && *p<='Z') ||
// 				     (*p>='a' && *p<='z' ) ) {
// 					str_addchar( &s, *p );
// 				}
// 			}
// 			else {
// 				if ( *p!=' ' && *p!='\t' ) {
// 					str_addchar( &s, *p );
// 				}
// 			}
// 			p++;
// 		}
// 		if ( str_memerr( &s ) )  { *status = BIBL_ERR_MEMERR; str_free( &s ); return; }
// 		fstatus = fields_add( out, "REFNUM", str_cstr( &s ), LEVEL_MAIN );
// 		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
// 		str_free( &s );
// 	}
// }

static void
append_simple( fields *in, char *intag, char *outtag, fields *out, int *status )
{
	int n, fstatus;

	n = fields_find( in, intag, LEVEL_ANY );
	if ( n!=FIELDS_NOTFOUND ) {
		fields_set_used( in, n );
		fstatus = fields_add( out, outtag, fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}
}

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

static void
append_simpleall( fields *in, char *intag, char *outtag, fields *out, int *status )
{
	int i, fstatus;

	for ( i=0; i<in->n; ++i ) {
		if ( fields_match_tag( in, i, intag ) ) {
			fields_set_used( in, i );
			fstatus = fields_add( out, outtag, fields_value( in, i, FIELDS_CHRP ), LEVEL_MAIN );
			if ( fstatus!=FIELDS_OK ) {
				*status = BIBL_ERR_MEMERR;
				return;
			}
		}
	}
}

static void
append_keywords( fields *in, fields *out, int *status )
{
	str keywords, *word;
	vplist_index i;
	int fstatus;
	vplist a;

	str_init( &keywords );
	vplist_init( &a );

	fields_findv_each( in, LEVEL_ANY, FIELDS_STRP, &a, "KEYWORD" );

	if ( a.n ) {

		for ( i=0; i<a.n; ++i ) {
			word = vplist_get( &a, i );
			if ( i>0 ) str_strcatc( &keywords, "; " );
			str_strcat( &keywords, word );
		}

		if ( str_memerr( &keywords ) ) { *status = BIBL_ERR_MEMERR; goto out; }

		fstatus = fields_add( out, "keywords", str_cstr( &keywords ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) {
			*status = BIBL_ERR_MEMERR;
			goto out;
		}


	}

out:
	str_free( &keywords );
	vplist_free( &a );
}

static void
append_fileattach( fields *in, fields *out, int *status )
{
	char *tag, *value;
	int i, fstatus;
	str data;

	str_init( &data );

	for ( i=0; i<in->n; ++i ) {

		tag = fields_tag( in, i, FIELDS_CHRP );
		if ( strcasecmp( tag, "FILEATTACH" ) ) continue;

		value = fields_value( in, i, FIELDS_CHRP );
		str_strcpyc( &data, ":" );
		str_strcatc( &data, value );
		if ( strsearch( value, ".pdf" ) )
			str_strcatc( &data, ":PDF" );
		else if ( strsearch( value, ".html" ) )
			str_strcatc( &data, ":HTML" );
		else str_strcatc( &data, ":TYPE" );

		if ( str_memerr( &data ) ) {
			*status = BIBL_ERR_MEMERR;
			goto out;
		}

		fields_set_used( in, i );
		fstatus = fields_add( out, "file", str_cstr( &data ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) {
			*status = BIBL_ERR_MEMERR;
			goto out;
		}

		str_empty( &data );
	}
out:
	str_free( &data );
}



/* name_build_bibentry()  // replaces name_build_withcomma() for bibentry names
 *
 * reconstruct parsed names in format: 'family|given|given||suffix'
 * to 'family suffix, given given
 */
void
name_build_bibentry_direct( str *s, const char *p )
{
	const char *suffix, *stopat;
	int nseps = 0, nch;

	str_empty( s );

	suffix = strstr( p, "||" );
	if ( suffix ) stopat = suffix;
	else stopat = strchr( p, '\0' );

	// while ( p != stopat ) {
	// 	nch = 0;
	// 	if ( nseps==1 ) {
	// 		if ( suffix ) {
	// 			str_strcatc( s, " " );
	// 			str_strcatc( s, suffix+2 );
	// 		}
	// 		str_addchar( s, ',' );
	// 	}
	// 	if ( nseps ) str_addchar( s, ' ' );
	// 	while ( p!=stopat && *p!='|' ) {
	// 		str_addchar( s, *p++ );
	// 		nch++;
	// 	}
	// 	if ( p!=stopat && *p=='|' ) p++;
	// 	if ( nseps!=0 && nch==1 ) str_addchar( s, '.' );
	// 	nseps++;
	// 

	str_strcatc(s, "person(");
	
	 while ( p != stopat ) {
		nch = 0;
		if ( nseps==1 ) {
			if ( suffix ) {
				str_strcatc( s, " " );
				str_strcatc( s, suffix+2 );
			}
			str_addchar( s, '\"' );
			str_addchar( s, ',' );
		}

		if ( nseps ) str_addchar( s, ' ' );

		if ( nseps==0 ) {
		  str_strcatc( s, "family = \"");
		} else if ( nseps==1 ) {
		  str_strcatc( s, "given = c(\"");
		} else if ( nseps>1 ) {
		  str_strcatc( s, ", \"");
		}
		
		while ( p!=stopat && *p!='|' ) {
		  str_addchar( s, *p++ );
		  nch++;
		}
		// if ( nseps == 0 ) {
		//   str_strcatc( s, "\"" );
		// }
		// else
		if ( nseps >= 1 ) {
		  str_addchar( s, '\"' );
		}
		
		if ( p!=stopat && *p=='|' ) p++;
		// if ( nseps!=0 && nch==1 ) str_addchar( s, '.' );
		nseps++;
	}
	 if(nseps == 1) str_addchar( s, '\"' );
	 else str_strcatc( s, ")"); // closes given = c( ... )

	 str_strcatc( s, ")"); // closes person( ... )





}



static void
append_people( fields *in, char *tag, char *ctag, char *atag,
		char *bibtag, int level, fields *out, int format_opts, int latex_out, int *status )
{
	int i, npeople, person, corp, asis, fstatus;
	str allpeople, oneperson;

	strs_init( &allpeople, &oneperson, NULL );

	str_strcatc( &allpeople, "c(" );
	
	/* primary citation authors */
	npeople = 0;
	for ( i=0; i<in->n; ++i ) {
		if ( level!=LEVEL_ANY && in->level[i]!=level ) continue;
		person = ( strcasecmp( in->tag[i].data, tag ) == 0 );
		corp   = ( strcasecmp( in->tag[i].data, ctag ) == 0 );
		asis   = ( strcasecmp( in->tag[i].data, atag ) == 0 );
		if ( person || corp || asis ) {
			// if ( npeople>0 ) {
			// 	if ( format_opts & BIBL_FORMAT_BIBOUT_WHITESPACE )
			// 		str_strcatc( &allpeople, "\n\t\tand " );
			// 	else str_strcatc( &allpeople, "\nand " );
			// }
			// if ( corp ) {
			// 	if ( latex_out ) str_addchar( &allpeople, '{' );
			// 	str_strcat( &allpeople, fields_value( in, i, FIELDS_STRP ) );
			// 	if ( latex_out ) str_addchar( &allpeople, '}' );
			// } else if ( asis ) {
			// 	if ( latex_out ) str_addchar( &allpeople, '{' );
			// 	str_strcat( &allpeople, fields_value( in, i, FIELDS_STRP ) );
			// 	if ( latex_out ) str_addchar( &allpeople, '}' );
			// } else {
			// 	name_build_withcomma( &oneperson, fields_value( in, i, FIELDS_CHRP ) );
			// 	str_strcat( &allpeople, &oneperson );
			// }

			if ( npeople>0 ) {
			  str_strcatc( &allpeople, ",\n          " );
			}
			if ( corp ) {
			  // str_addchar( &allpeople, '{' );
			  str_strcatc( &allpeople, "person(family = \"");
			  str_strcat( &allpeople, fields_value( in, i, FIELDS_STRP ) );
			  str_strcatc( &allpeople, "\")");
			  // str_addchar( &allpeople, '}' );
			} else if ( asis ) {
			  // str_addchar( &allpeople, '{' );
			  str_strcatc( &allpeople, "person(family = \"");
			  str_strcat( &allpeople, fields_value( in, i, FIELDS_STRP ) );
			  str_strcatc( &allpeople, "\")");
			  // str_addchar( &allpeople, '}' );
			} else {
			  // name_build_withcomma( &oneperson, fields_value( in, i, FIELDS_CHRP ) );
			  name_build_bibentry_direct( &oneperson, fields_value( in, i, FIELDS_CHRP ) );
				str_strcat( &allpeople, &oneperson );
			}

			
			npeople++;
		}
	}

	str_strcatc( &allpeople, ")" );  
	
	if ( npeople ) {
		fstatus = fields_add( out, bibtag, allpeople.data, LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}

	strs_free( &allpeople, &oneperson, NULL );
}

static int
append_title_chosen( fields *in, char *bibtag, fields *out, int nmainttl, int nsubttl )
{
	str fulltitle, *mainttl = NULL, *subttl = NULL;
	int status, ret = BIBL_OK;

	str_init( &fulltitle );

	if ( nmainttl!=-1 ) {
		mainttl = fields_value( in, nmainttl, FIELDS_STRP );
		fields_set_used( in, nmainttl );
	}

	if ( nsubttl!=-1 ) {
		subttl = fields_value( in, nsubttl, FIELDS_STRP );
		fields_set_used( in, nsubttl );
	}

	title_combine( &fulltitle, mainttl, subttl );

	if ( str_memerr( &fulltitle ) ) {
		ret = BIBL_ERR_MEMERR;
		goto out;
	}

	if ( str_has_value( &fulltitle ) ) {
		status = fields_add( out, bibtag, str_cstr( &fulltitle ), LEVEL_MAIN );
		if ( status!=FIELDS_OK ) ret = BIBL_ERR_MEMERR;
	}

out:
	str_free( &fulltitle );
	return ret;
}

static int
append_title( fields *in, char *bibtag, int level, fields *out, int format_opts )
{
	int title, short_title, subtitle, short_subtitle, use_title, use_subtitle;

	title          = fields_find( in, "TITLE",         level );
	short_title    = fields_find( in, "SHORTTITLE",    level );
	subtitle       = fields_find( in, "SUBTITLE",      level );
	short_subtitle = fields_find( in, "SHORTSUBTITLE", level );

	if ( title==FIELDS_NOTFOUND || ( ( format_opts & BIBL_FORMAT_BIBOUT_SHORTTITLE ) && level==1 ) ) {
		use_title    = short_title;
		use_subtitle = short_subtitle;
	}

	else {
		use_title    = title;
		use_subtitle = subtitle;
	}

	return append_title_chosen( in, bibtag, out, use_title, use_subtitle );
}

static void
append_titles( fields *in, int type, fields *out, int format_opts, int *status )
{
	/* item=main level title */
	*status = append_title( in, "title", 0, out, format_opts );
	if ( *status!=BIBL_OK ) return;

	switch( type ) {

		case TYPE_ARTICLE:
		*status = append_title( in, "journal", 1, out, format_opts );
		break;

		case TYPE_INBOOK:
		*status = append_title( in, "bookTitle", 1, out, format_opts );
		if ( *status!=BIBL_OK ) return;
		*status = append_title( in, "series",    2, out, format_opts );
		break;

		case TYPE_INCOLLECTION:
		case TYPE_INPROCEEDINGS:
		*status = append_title( in, "booktitle", 1, out, format_opts );
		if ( *status!=BIBL_OK ) return;
		*status = append_title( in, "series",    2, out, format_opts );
		break;

		case TYPE_PHDTHESIS:
		case TYPE_MASTERSTHESIS:
		*status = append_title( in, "series", 1, out, format_opts );
		break;

		case TYPE_BOOK:
		case TYPE_REPORT:
		case TYPE_COLLECTION:
		case TYPE_PROCEEDINGS:
		*status = append_title( in, "series", 1, out, format_opts );
		if ( *status!=BIBL_OK ) return;
		*status = append_title( in, "series", 2, out, format_opts );

		// // (2020-12-27) added by Georgi
		// // TODO: not sure about this it always sets booktitle same as title.
		// //      Logical but iridia uses booktitle for abbreviated title
		// //      this looks like iridia convention; COMMENTING OUT
		// //
		// if ( *status!=BIBL_OK ) return;
		// *status = append_title( in, "booktitle", LEVEL_ANY, out, format_opts );

		break;

		default:
		/* do nothing */
		break;

	}
}

static int
find_date( fields *in, char *date_element )
{
	char date[100], partdate[100];
	int n;

	sprintf( date, "DATE:%s", date_element );
	n = fields_find( in, date, LEVEL_ANY );

	if ( n==FIELDS_NOTFOUND ) {
		sprintf( partdate, "PARTDATE:%s", date_element );
		n = fields_find( in, partdate, LEVEL_ANY );
	}

	return n;
}

static void
append_date( fields *in, fields *out, int *status )
{
	char *months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	int n, month, fstatus;

	n = find_date( in, "YEAR" );
	if ( n!=FIELDS_NOTFOUND ) {
		fields_set_used( in, n );
		fstatus = fields_add( out, "year", fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) {
			*status = BIBL_ERR_MEMERR;
			return;
		}
	}

	n = find_date( in, "MONTH" );
	if ( n!=-1 ) {
		fields_set_used( in, n );
		month = atoi( fields_value( in, n, FIELDS_CHRP ) );
		if ( month>0 && month<13 )
			fstatus = fields_add( out, "month", months[month-1], LEVEL_MAIN );
		else
			fstatus = fields_add( out, "month", fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) {
			*status = BIBL_ERR_MEMERR;
			return;
		}
	}

	n = find_date( in, "DAY" );
	if ( n!=-1 ) {
		fields_set_used( in, n );
		fstatus = fields_add( out, "day", fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) {
			*status = BIBL_ERR_MEMERR;
			return;
		}
	}

}

static void
append_arxiv( fields *in, fields *out, int *status )
{
	int n, fstatus1, fstatus2;
	str url;

	n = fields_find( in, "ARXIV", LEVEL_ANY );
	if ( n==FIELDS_NOTFOUND ) return;

	fields_set_used( in, n );

	/* ...write:
	 *     archivePrefix = "arXiv",
	 *     eprint = "#####",
	 * ...for arXiv references
	 */
	fstatus1 = fields_add( out, "archivePrefix", "arXiv", LEVEL_MAIN );
	fstatus2 = fields_add( out, "eprint", fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
	if ( fstatus1!=FIELDS_OK || fstatus2!=FIELDS_OK ) {
		*status = BIBL_ERR_MEMERR;
		return;
	}

	/* ...also write:
	 *     url = "http://arxiv.org/abs/####",
	 * ...to maximize compatibility
	 */
	str_init( &url );
	arxiv_to_url( in, n, "URL", &url );
	if ( str_has_value( &url ) ) {
		fstatus1 = fields_add( out, "url", str_cstr( &url ), LEVEL_MAIN );
		if ( fstatus1!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}
	str_free( &url );
}

static void
append_urls( fields *in, fields *out, int *status )
{
	int lstatus;
	slist types;

	lstatus = slist_init_valuesc( &types, "URL", "DOI", "PMID", "PMC", "JSTOR", NULL );
	if ( lstatus!=SLIST_OK ) {
		*status = BIBL_ERR_MEMERR;
		return;
	}

	*status = urls_merge_and_add( in, LEVEL_ANY, out, "url", LEVEL_MAIN, &types );

	slist_free( &types );
}

static void
append_isi( fields *in, fields *out, int *status )
{
	int n, fstatus;

	n = fields_find( in, "ISIREFNUM", LEVEL_ANY );
	if ( n==FIELDS_NOTFOUND ) return;

	fstatus = fields_add( out, "note", fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
	if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
}

static void
append_articlenumber( fields *in, fields *out, int *status )
{
	int n, fstatus;

	n = fields_find( in, "ARTICLENUMBER", LEVEL_ANY );
	if ( n==FIELDS_NOTFOUND ) return;

	fields_set_used( in, n );
	fstatus = fields_add( out, "pages", fields_value( in, n, FIELDS_CHRP ), LEVEL_MAIN );
	if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
}

static int
pages_build_pagestr( str *pages, fields *in, int sn, int en, int format_opts )
{
	/* ...append if starting page number is defined */
	if ( sn!=-1 ) {
		str_strcat( pages, fields_value( in, sn, FIELDS_STRP ) );
		fields_set_used( in, sn );
	}

	/* ...append dashes if both starting and ending page numbers are defined */
	if ( sn!=-1 && en!=-1 ) {
		if ( format_opts & BIBL_FORMAT_BIBOUT_SINGLEDASH )
			str_strcatc( pages, "-" );
		else
			str_strcatc( pages, "--" );
	}

	/* ...append ending page number is defined */
	if ( en!=-1 ) {
		str_strcat( pages, fields_value( in, en, FIELDS_STRP ) );
		fields_set_used( in, en );
	}

	if ( str_memerr( pages ) ) return BIBL_ERR_MEMERR;
	else return BIBL_OK;
}

static int
pages_are_defined( fields *in, int *sn, int *en )
{
	*sn = fields_find( in, "PAGES:START", LEVEL_ANY );
	*en = fields_find( in, "PAGES:STOP",  LEVEL_ANY );
	if ( *sn==FIELDS_NOTFOUND && *en==FIELDS_NOTFOUND ) return 0;
	else return 1;
}

static void
append_pages( fields *in, fields *out, int format_opts, int *status )
{
	int sn, en, fstatus;
	str pages;

	if ( !pages_are_defined( in, &sn, &en ) ) {
		append_articlenumber( in, out, status );
		return;
	}

	str_init( &pages );
	*status = pages_build_pagestr( &pages, in, sn, en, format_opts );
	if ( *status==BIBL_OK ) {
		fstatus = fields_add( out, "pages", str_cstr( &pages ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}
	str_free( &pages );
}

/*
 * from Tim Hicks:
 * I'm no expert on bibtex, but those who know more than I on our mailing 
 * list suggest that 'issue' isn't a recognised key for bibtex and 
 * therefore that bibutils should be aliasing IS to number at some point in 
 * the conversion.
 *
 * Therefore prefer outputting issue/number as number and only keep
 * a distinction if both issue and number are present for a particular
 * reference.
 */

static void
append_issue_number( fields *in, fields *out, int *status )
{
	char issue[] = "issue", number[] = "number", *use_issue = number;
	int nissue  = fields_find( in, "ISSUE",  LEVEL_ANY );
	int nnumber = fields_find( in, "NUMBER", LEVEL_ANY );
	int fstatus;

	if ( nissue!=FIELDS_NOTFOUND && nnumber!=FIELDS_NOTFOUND ) use_issue = issue;

	if ( nissue!=FIELDS_NOTFOUND ) {
		fields_set_used( in, nissue );
		fstatus = fields_add( out, use_issue, fields_value( in, nissue, FIELDS_CHRP ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) {
			*status = BIBL_ERR_MEMERR;
			return;
		}
	}

	if ( nnumber!=FIELDS_NOTFOUND ) {
		fields_set_used( in, nnumber );
		fstatus = fields_add( out, "number", fields_value( in, nnumber, FIELDS_CHRP ), LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) {
			*status = BIBL_ERR_MEMERR;
			return;
		}
	}
}

static void
append_howpublished( fields *in, fields *out, int *status )
{
	int n, fstatus;
	char *d;

	n = fields_find( in, "GENRE:BIBUTILS", LEVEL_ANY );
	if ( n==FIELDS_NOTFOUND ) return;

	d = fields_value( in, n, FIELDS_CHRP_NOUSE );
	if ( !strcmp( d, "Habilitation thesis" ) ) {
		fstatus = fields_add( out, "howpublised", d, LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}
	if ( !strcmp( d, "Licentiate thesis" ) ) {
		fstatus = fields_add( out, "howpublised", d, LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}
	if ( !strcmp( d, "Diploma thesis" ) ) {
		fstatus = fields_add( out, "howpublised", d, LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}
}

// Georgi
static void
append_key( fields *in, char *intag, char *outtag, fields *out, int *status )
{
        int n, fstatus;
        
        char  *value; // *tag
        str data;
        
        str_init( &data );
        
        n = fields_find( in, intag, LEVEL_ANY );
        if ( n!=FIELDS_NOTFOUND ) {
            fields_set_used( in, n );
            
            value = fields_value( in, n, FIELDS_CHRP );
	    str_strcatc( &data, "c(" );
            str_strcatc( &data, "key = \"" );
            str_strcatc( &data, value );
            str_strcatc( &data, "\")" );
                
            // fstatus = fields_add( out, outtag, str_cstr( &data ), LEVEL_MAIN );
            fstatus = fields_add( out, outtag, data.data, LEVEL_MAIN );
            if ( fstatus!=FIELDS_OK ) {
                *status = BIBL_ERR_MEMERR;
                goto out;
            }
        }

out:
	str_free( &data );

}

static int
bibentrydirectout_assemble( fields *in, fields *out, param *pm, unsigned long refnum )
{
	int type, status = BIBL_OK;

	// // Georgi; for testing
	// fields_report_stderr(in);

	// Determine type 
	//   type = bibentrydirectout_type( in, pm->progname, "", refnum );
	//
	
	int n, fstatus;
	char *fld_val;
	n = fields_find( in, "INTERNAL_TYPE", LEVEL_ANY );

 // REprintf("\nassemble: INTERNAL_TYPE = %d\n", n);
 // REprintf("\nassemble: FIELDS_NOTFOUND = %d\n", FIELDS_NOTFOUND);
	
	if ( n!=FIELDS_NOTFOUND ) {
		fields_set_used( in, n );
		fld_val = fields_value( in, n, FIELDS_CHRP );

		// TODO: this is absolutely temporary
		if ( !strcmp( fld_val, "Article" ) ) {
		  type = 1;
		}
		else if ( !strcmp( fld_val, "Inbook" ) ) {         
		  type = 2;
		}
		else if ( !strcmp( fld_val, "Proceedings" ) ) {    
		  type = 3;
		}
		else if ( !strcmp( fld_val, "InProceedings" ) ) {  
		  type = 4;
		}
		else if ( !strcmp( fld_val, "Book" ) ) {           
		  type = 5;
		}
		else if ( !strcmp( fld_val, "PhdThesis" ) ) {      
		  type = 6;
		}
		else if ( !strcmp( fld_val, "MastersThesis" ) ) {  
		  type = 7;
		}
		else if ( !strcmp( fld_val, "MastersThesis" ) ) {  
		  type = 8;
		}
		else if ( !strcmp( fld_val, "TechReport" ) ) {     
		  type = 9;
		}
		else if ( !strcmp( fld_val, "Manual" ) ) {         
		  type = 10;
		}
		else if ( !strcmp( fld_val, "Collection" ) ) {     
		  type = 11;
		}
		else if ( !strcmp( fld_val, "InCollection" ) ) {   
		  type = 12;
		}
		else if ( !strcmp( fld_val, "Unpublished" ) ) {    
		  type = 13;
		}
		else if ( !strcmp( fld_val, "Electronic" ) ) {     
		  type = 14;
		}
		else if ( !strcmp( fld_val, "Misc" ) ) {           
		  type = 15;
		}
		// TODO: temporary!!
		else if ( !strcmp( fld_val, "online" ) ) {     // patch!!      
		  type = 15;
		}
		else {
		  type = 0; // unknown
		}

		// REprintf("kiki: fld_val=%s\n", fld_val);
		//  REprintf("type = %d\n\n", type);
		
		if ( strcmp( fld_val, "online" ) )
		  fstatus = fields_add( out, "bibtype", fld_val, LEVEL_MAIN );
		else
		  // this is temporary patch!
		  fstatus = fields_add( out, "bibtype", "Misc", LEVEL_MAIN );

		if ( fstatus!=FIELDS_OK ) status = BIBL_ERR_MEMERR;
	}

	
	// append_type        ( type, out, &status );
	append_simple        ( in, "REFNUM", "refnum", out, &status );

	// append_citekey     ( in, out, pm->format_opts, &status );
	append_simple      ( in, "REFNUM", "refnum", out, &status );
	
	append_people      ( in, "AUTHOR",     "AUTHOR:CORP",     "AUTHOR:ASIS",     "author", LEVEL_MAIN, out, pm->format_opts, pm->latexout, &status );
	append_people      ( in, "EDITOR",     "EDITOR:CORP",     "EDITOR:ASIS",     "editor", LEVEL_ANY, out, pm->format_opts, pm->latexout, &status );
	append_people      ( in, "TRANSLATOR", "TRANSLATOR:CORP", "TRANSLATOR:ASIS", "translator", LEVEL_ANY, out, pm->format_opts, pm->latexout, &status );
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
		if ( not_person && not_other )  fprintf( fp, "\"" );
		
	}

	/* ...finish reference */
	// if ( format_opts & BIBL_FORMAT_BIBOUT_FINALCOMMA ) fprintf( fp, "," );
	// fprintf( fp, "\n}\n\n" );
	fprintf( fp, " )" );

	fflush( fp );

	return BIBL_OK;
}
