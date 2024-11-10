/*
 * bibprog.c
 *
 * Copyright (c) Chris Putnam 2004-2020
 * Copyright (c) Georgi N. Boshnakov 2020
 *
 * Source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include "bibutils.h"
#include "bibprog.h"

//Georgi
// void
double
bibprog( int argc, char *argv[], param *p, char *outfile[] )
{
	FILE *fp;
	bibl b;
	int err, i;
	// REprintf("(bibprog) start of bibprog!\n");

	// Georgi
	FILE *fout;
	// fout = fopen("bbbbb.bib", "w");
	fout = fopen(outfile[0], "w");
	
	bibl_init( &b );
	// REprintf("(bibprog) before bibl_read!\n");
	if ( argc<2 ) {
	    REprintf("(bibprog) args < 2\n");

		err = bibl_read( &b, stdin, "stdin", p );
		if ( err ) bibl_reporterr( err ); 
	} else {
	        // REprintf("(bibprog) args >= 2\n");
		for ( i=1; i<argc; ++i ) {
			fp = fopen( argv[i], "r" );
			if ( fp ) {
				err = bibl_read( &b, fp, argv[i], p );
				if ( err ) bibl_reporterr( err );
				fclose( fp );
			}
		} 
	}
	// REprintf("(bibprog) after bibl_read!\n");
	
	// Georgi: for testing
	// REprintf("(bibprog) Bh: before_bibl_write\n");
	// for(long i = 0; i < b.n; ++i) {
	//   fields_report_stderr( b.ref[i] );
	// }
  
        bibl_write( &b, fout, p );
        fflush( fout );
        fclose( fout );

	// // Georgi - now the value is returned to the caller
	// if( p->progname ) REprintf( "%s: ", p->progname );
	// REprintf( "Processed %ld references.\n", b.n );

	// Georgi
	double val = (double)b.n;

	bibl_free( &b );

	//Georgi
	return val; 
}






