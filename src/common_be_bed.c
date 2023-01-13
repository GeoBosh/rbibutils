/*
 * common_be_bed.c
 *
 * Copyright (c) Georgi N. Boshnakov 2020-2023
 *
 * File created by taking out common parts from bibentry.c and bibentrydirectout.c.
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

#include "common_beout.h"

void bibentrydirectout_writeheader( FILE *outptr, param *pm )
{
  fprintf( outptr, "c( #" );
}

void bibentrydirectout_writefooter( FILE *outptr)
{
  fprintf( outptr, "\n)" );
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

	// REprintf( "\n(name_build_bibentry_direct input) %s \n ", p );
 
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

	 // REprintf( "\n(name_build_bibentry_direct ouput) %s \n ", s->data );
}

void
append_people_be( fields *in, char *tag, char *ctag, char *atag,
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

// Georgi
 void
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
