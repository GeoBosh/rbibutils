/*
 * bibentryout.c  (based on bibtexout.c)
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
 PUBLIC: int bibentryout_initparams()
*****************************************************/

static int  bibentryout_assemble( fields *in, fields *out, param *pm, unsigned long refnum );

int
bibentryout_initparams( param *pm, const char *progname )
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
	pm->assemblef = bibentryout_assemble;
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
 PUBLIC: int bibentryout_assemble()
*****************************************************/

static int
bibentryout_type( fields *in, const char *progname, const char *filename, unsigned long refnum )
{
	match_type genre_matches[] = {
		{ "periodical",             TYPE_ARTICLE,       LEVEL_ANY  },
		{ "academic journal",       TYPE_ARTICLE,       LEVEL_ANY  },
		{ "magazine",               TYPE_ARTICLE,       LEVEL_ANY  },
		{ "newspaper",              TYPE_ARTICLE,       LEVEL_ANY  },
		{ "article",                TYPE_ARTICLE,       LEVEL_ANY  },
		{ "instruction",            TYPE_MANUAL,        LEVEL_ANY  },
		{ "book",                   TYPE_BOOK,          LEVEL_MAIN },
		{ "book",                   TYPE_INBOOK,        LEVEL_ANY  },
		{ "book chapter",           TYPE_INBOOK,        LEVEL_ANY  },
		{ "unpublished",            TYPE_UNPUBLISHED,   LEVEL_ANY  },
		{ "manuscript",             TYPE_UNPUBLISHED,   LEVEL_ANY  },
		{ "conference publication", TYPE_PROCEEDINGS,   LEVEL_MAIN },
		{ "conference publication", TYPE_INPROCEEDINGS, LEVEL_ANY  },
		{ "collection",             TYPE_COLLECTION,    LEVEL_MAIN },
		{ "collection",             TYPE_INCOLLECTION,  LEVEL_ANY  },
		{ "report",                 TYPE_REPORT,        LEVEL_ANY  },
		{ "technical report",       TYPE_REPORT,        LEVEL_ANY  },
		{ "Masters thesis",         TYPE_MASTERSTHESIS, LEVEL_ANY  },
		{ "Diploma thesis",         TYPE_DIPLOMATHESIS, LEVEL_ANY  },
		{ "Ph.D. thesis",           TYPE_PHDTHESIS,     LEVEL_ANY  },
		{ "Licentiate thesis",      TYPE_PHDTHESIS,     LEVEL_ANY  },
		{ "thesis",                 TYPE_PHDTHESIS,     LEVEL_ANY  },
		{ "electronic",             TYPE_ELECTRONIC,    LEVEL_ANY  },
		{ "miscellaneous",          TYPE_MISC,          LEVEL_ANY  },
	};
	int ngenre_matches = sizeof( genre_matches ) / sizeof( genre_matches[0] );

	match_type resource_matches[] = {
		{ "moving image",           TYPE_ELECTRONIC,    LEVEL_ANY  },
		{ "software, multimedia",   TYPE_ELECTRONIC,    LEVEL_ANY  },
	};
	int nresource_matches = sizeof( resource_matches ) /sizeof( resource_matches[0] );

	match_type issuance_matches[] = {
		{ "monographic",            TYPE_BOOK,          LEVEL_MAIN },
		{ "monographic",            TYPE_INBOOK,        LEVEL_ANY  },
	};
	int nissuance_matches = sizeof( issuance_matches ) / sizeof( issuance_matches[0] );

	int type, maxlevel, n;

	type = type_from_mods_hints( in, TYPE_FROM_GENRE, genre_matches, ngenre_matches, TYPE_UNKNOWN );
	if ( type==TYPE_UNKNOWN ) type = type_from_mods_hints( in, TYPE_FROM_RESOURCE, resource_matches, nresource_matches, TYPE_UNKNOWN );
	if ( type==TYPE_UNKNOWN ) type = type_from_mods_hints( in, TYPE_FROM_ISSUANCE, issuance_matches, nissuance_matches, TYPE_UNKNOWN );

	/* default to TYPE_MISC */
	if ( type==TYPE_UNKNOWN ) {
		maxlevel = fields_maxlevel( in );
		if ( maxlevel > 0 ) type = TYPE_MISC;
		else {
			if ( progname ) REprintf( "%s: ", progname );
			REprintf( "Cannot identify TYPE in reference %lu ", refnum+1 );
			n = fields_find( in, "REFNUM", LEVEL_ANY );
			if ( n!=FIELDS_NOTFOUND ) 
				REprintf( " %s", (char*) fields_value( in, n, FIELDS_CHRP ) );
			REprintf( " (defaulting to @Misc)\n" );
			type = TYPE_MISC;
		}
	}

	// REprintf("type = %d\n\n", type);
	return type;
}

// Georgi
//     TODO: consolidate with append_type
static int
is_TechReport_type( int type )
{
    // 2023-11-05 was: char *typenames[ NUM_BIBENTRY_TYPES ] = {
	char *typenames[ NUM_TYPES ] = {
		[ TYPE_ARTICLE       ] = "Article",
		[ TYPE_INBOOK        ] = "Inbook",
		[ TYPE_PROCEEDINGS   ] = "Proceedings",
		[ TYPE_INPROCEEDINGS ] = "InProceedings",
		[ TYPE_BOOK          ] = "Book",
		[ TYPE_PHDTHESIS     ] = "PhdThesis",
		[ TYPE_MASTERSTHESIS ] = "MastersThesis",
		[ TYPE_DIPLOMATHESIS ] = "MastersThesis",
		[ TYPE_REPORT        ] = "TechReport",
		[ TYPE_MANUAL        ] = "Manual",
		[ TYPE_COLLECTION    ] = "Collection",
		[ TYPE_INCOLLECTION  ] = "InCollection",
		[ TYPE_UNPUBLISHED   ] = "Unpublished",
		[ TYPE_ELECTRONIC    ] = "Electronic",
		[ TYPE_MISC          ] = "Misc",
	};
	
	return( !strcmp(typenames[ type ], "TechReport") );
}

static void
append_type( int type, fields *out, int *status )
{
    // 2023-11-05 was: char *typenames[ NUM_BIBENTRY_TYPES ] = {
	char *typenames[ NUM_TYPES ] = {
		[ TYPE_ARTICLE       ] = "Article",
		[ TYPE_INBOOK        ] = "Inbook",
		[ TYPE_PROCEEDINGS   ] = "Proceedings",
		[ TYPE_INPROCEEDINGS ] = "InProceedings",
		[ TYPE_BOOK          ] = "Book",
		[ TYPE_PHDTHESIS     ] = "PhdThesis",
		[ TYPE_MASTERSTHESIS ] = "MastersThesis",
		[ TYPE_DIPLOMATHESIS ] = "MastersThesis",
		[ TYPE_REPORT        ] = "TechReport",
		[ TYPE_MANUAL        ] = "Manual",
		[ TYPE_COLLECTION    ] = "Collection",
		[ TYPE_INCOLLECTION  ] = "InCollection",
		[ TYPE_UNPUBLISHED   ] = "Unpublished",
		[ TYPE_ELECTRONIC    ] = "Electronic",
		[ TYPE_MISC          ] = "Misc",
	};
	int fstatus;
	char *s;

	if ( type < 0 || type >= NUM_BIBENTRY_TYPES ) type = TYPE_MISC;
	s = typenames[ type ];

	fstatus = fields_add( out, "TYPE", s, LEVEL_MAIN );
	if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
}

static int
bibentryout_assemble( fields *in, fields *out, param *pm, unsigned long refnum )
{
	int type, status = BIBL_OK;

	// Georgi; for testing
	// fields_report_stderr(in);

	type = bibentryout_type( in, pm->progname, "", refnum );

	append_type        ( type, out, &status );
	append_citekey     ( in, out, pm->format_opts, &status );
	append_people_be      ( in, "AUTHOR",     "AUTHOR:CORP",     "AUTHOR:ASIS",     "author", LEVEL_MAIN, out, pm->format_opts, pm->latexout, &status );
	append_people_be      ( in, "EDITOR",     "EDITOR:CORP",     "EDITOR:ASIS",     "editor", LEVEL_ANY, out, pm->format_opts, pm->latexout, &status );
	append_people_be      ( in, "TRANSLATOR", "TRANSLATOR:CORP", "TRANSLATOR:ASIS", "translator", LEVEL_ANY, out, pm->format_opts, pm->latexout, &status );
	append_titles      ( in, type, out, pm->format_opts, &status );
	append_date        ( in, out, &status );
	append_simple      ( in, "EDITION",            "edition",   out, &status );

	// Georgi TODO: it seems that bibutils import "institution" as "publisher"
	if( is_TechReport_type(type))
	  append_simple      ( in, "PUBLISHER",          "institution", out, &status );
	else
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

	return status;
}
