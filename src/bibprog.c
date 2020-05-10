/*
 * bibprog.c
 *
 * Copyright (c) Chris Putnam 2004-2020
 *
 * Source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include "bibutils.h"
#include "bibprog.h"

void
bibprog( int argc, char *argv[], param *p, char *outfile[] )
{
	FILE *fp;
	bibl b;
	int err, i;

	// Georgi
	FILE *fout;
	// fout = fopen("bbbbb.bib", "w");
	fout = fopen(outfile[0], "w");
	
	bibl_init( &b );
	if ( argc<2 ) {
		err = bibl_read( &b, stdin, "stdin", p );
		if ( err ) bibl_reporterr( err ); 
	} else {
		for ( i=1; i<argc; ++i ) {
			fp = fopen( argv[i], "r" );
			if ( fp ) {
				err = bibl_read( &b, fp, argv[i], p );
				if ( err ) bibl_reporterr( err );
				fclose( fp );
			}
		} 
	}
        bibl_write( &b, fout, p );
        fflush( fout );
	// if( p->progname ) fprintf( stderr, "%s: ", p->progname );
	// fprintf( stderr, "Processed %ld references.\n", b.n );
	bibl_free( &b );
}






