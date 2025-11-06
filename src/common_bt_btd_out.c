/*
 * common_bt_btd_out.c
 *
 * Copyright (c) Chris Putnam 2003-2020
 * Copyright (c) Georgi N. Boshnakov 2020-2023
 *
 * File created by taking out common parts from bibtexout.c and bibdirectout.c.
 *
 * Program and source code released under the GPL version 2
 *
 */

#include <ctype.h>

#include "name.h"
#include "fields.h"
#include "bibutils.h"

#include "common_bt_blt_btd_out.h"

void
append_people( fields *in, char *tag, char *ctag, char *atag,
		char *bibtag, int level, fields *out, int format_opts, int latex_out, int *status )
{
	int i, npeople, person, corp, asis, fstatus;
	str allpeople, oneperson;

	strs_init( &allpeople, &oneperson, NULL );

	/* primary citation authors */
	npeople = 0;
	for ( i=0; i<in->n; ++i ) {
		if ( level!=LEVEL_ANY && in->level[i]!=level ) continue;
		person = ( strcasecmp( in->tag[i].data, tag ) == 0 );
		corp   = ( strcasecmp( in->tag[i].data, ctag ) == 0 );
		asis   = ( strcasecmp( in->tag[i].data, atag ) == 0 );
		if ( person || corp || asis ) {
			if ( npeople>0 ) {
				if ( format_opts & BIBL_FORMAT_BIBOUT_WHITESPACE )
					str_strcatc( &allpeople, "\n\t\tand " );
				else str_strcatc( &allpeople, "\nand " );
			}
			if ( corp ) {
				if ( latex_out ) str_addchar( &allpeople, '{' );
				str_strcat( &allpeople, fields_value( in, i, FIELDS_STRP ) );
				if ( latex_out ) str_addchar( &allpeople, '}' );
			} else if ( asis ) {
				if ( latex_out ) str_addchar( &allpeople, '{' );
				str_strcat( &allpeople, fields_value( in, i, FIELDS_STRP ) );
				if ( latex_out ) str_addchar( &allpeople, '}' );
			} else {
				name_build_withcomma( &oneperson, fields_value( in, i, FIELDS_CHRP ) );
				str_strcat( &allpeople, &oneperson );
			}
			npeople++;
		}
	}
	if ( npeople ) {
		fstatus = fields_add( out, bibtag, allpeople.data, LEVEL_MAIN );
		if ( fstatus!=FIELDS_OK ) *status = BIBL_ERR_MEMERR;
	}

	strs_free( &allpeople, &oneperson, NULL );
}

/*****************************************************
 PUBLIC: int bibtexout_write()
*****************************************************/

int
bibtexout_write( fields *out, FILE *fp, param *pm, unsigned long refnum )
{
	int i, j, len, nquotes, format_opts = pm->format_opts;
	char *tag, *value, ch;

	/* ...output type information "@article{" */
	value = ( char * ) fields_value( out, 0, FIELDS_CHRP );
	if ( !(format_opts & BIBL_FORMAT_BIBOUT_UPPERCASE) ) fprintf( fp, "@%s{", value );
	else {
		len = (value) ? strlen( value ) : 0;
		fprintf( fp, "@" );
		for ( i=0; i<len; ++i )
			fprintf( fp, "%c", toupper((unsigned char)value[i]) );
		fprintf( fp, "{" );
	}

	/* ...output refnum "Smith2001" */
	value = ( char * ) fields_value( out, 1, FIELDS_CHRP );
	fprintf( fp, "%s", value );

	/* ...rest of the references */
	for ( j=2; j<out->n; ++j ) {
		nquotes = 0;
		tag   = ( char * ) fields_tag( out, j, FIELDS_CHRP );
		value = ( char * ) fields_value( out, j, FIELDS_CHRP );
		fprintf( fp, ",\n" );
		if ( format_opts & BIBL_FORMAT_BIBOUT_WHITESPACE ) fprintf( fp, "  " );
		if ( !(format_opts & BIBL_FORMAT_BIBOUT_UPPERCASE ) ) fprintf( fp, "%s", tag );
		else {
			len = strlen( tag );
			for ( i=0; i<len; ++i )
				fprintf( fp, "%c", toupper((unsigned char)tag[i]) );
		}
		if ( format_opts & BIBL_FORMAT_BIBOUT_WHITESPACE ) fprintf( fp, " = \t" );
		else fprintf( fp, "=" );

		if ( format_opts & BIBL_FORMAT_BIBOUT_BRACKETS ) fprintf( fp, "{" );
		else fprintf( fp, "\"" );

		len = strlen( value );
		for ( i=0; i<len; ++i ) {
			ch = value[i];
			if ( ch!='\"' ) fprintf( fp, "%c", ch );
			else {
				if ( format_opts & BIBL_FORMAT_BIBOUT_BRACKETS || ( i>0 && value[i-1]=='\\' ) )
					fprintf( fp, "\"" );
				else {
					if ( nquotes % 2 == 0 )
						fprintf( fp, "``" );
					else    fprintf( fp, "\'\'" );
					nquotes++;
				}
			}
		}

		if ( format_opts & BIBL_FORMAT_BIBOUT_BRACKETS ) fprintf( fp, "}" );
		else fprintf( fp, "\"" );
	}

	/* ...finish reference */
	if ( format_opts & BIBL_FORMAT_BIBOUT_FINALCOMMA ) fprintf( fp, "," );
	fprintf( fp, "\n}\n\n" );

	fflush( fp );

	return BIBL_OK;
}
