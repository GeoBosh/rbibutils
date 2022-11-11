/*
 * bib2be.c
 *
 * Copyright (c) Georgi N. Boshnakov 2021
 * 
 * The code in this file is based on xxx2yyy utilities by Chris Putnam 2003-2020
 *
 * Program and source code released under the GPL version 2
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <R.h>

#include "bibutils.h"
#include "bibformats.h"
#include "tomods.h"
#include "bibprog.h"

#include "args.h"

extern int convert_latex_escapes_only;
extern int export_tex_chars_only;
extern int rdpack_patch_for_i_acute_variant;

extern int bibtexdirectin_initparams( param *pm, const char *progname );
extern void bibdirectin_more_cleanf( void );
extern int bibentrydirectout_initparams( param *pm, const char *progname );

char *helpBE[] = {
     /* bib2be */
     "Converts a Bibtex reference file into Bibentry source\n\n",
     "bibtex_file"
};

// extern void process_args( int *argc, char *argv[], param *p );

// bib2be
void
process_direct_args( int *argc, char *argv[], param *p, const char *progname[] )
{
     // void (*help)( char *progname );
     // if(strcmp(*progname, "xml2bib") == 0  || strcmp(*progname, "xml2biblatex") ){
     // if(strcmp(*progname, "xml2bib") == 0)
     //   help = helpBE[0];
     // else
     //   help = helpBE[0];
  
     int i, j, subtract;
     i = 1;
     while ( i<*argc ) {
	  subtract = 0;
	  if ( args_match( argv[i], "-h", "--help" ) ) {
	       // help( p->progname );
	       REprintf("help not ready yet\n");
	       error("\n"); // exit( EXIT_SUCCESS );
	  } else if ( args_match( argv[i], "--keep-tex-chars", "" ) ) { // Georgi
	       p->latexin = 0;
	       subtract = 1;
	  } else if ( args_match( argv[i], "--convert_latex_escapes", "" ) ) { // Georgi
	       p->latexin = 0; // like --keep-tex-chars
	       convert_latex_escapes_only = 1;
	       p->latexout = 0;
	       subtract = 1;
	  } else if ( args_match( argv[i], "--export_tex_chars", "" ) ) { // Georgi
	       p->latexin = 0; // like --keep-tex-chars
	       export_tex_chars_only = 1;
	       p->latexout = 1;
	       subtract = 1;
	  } else if ( args_match( argv[i], "--Rdpack", "" ) ) { // new 2021-10-13
	       rdpack_patch_for_i_acute_variant = 1;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-nl", "--no-latex" ) ) {
	       p->latexout = 0;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-v", "--version" ) ) {
	       args_tellversion( p->progname );
	       error("\n"); // exit( EXIT_SUCCESS );
	  } else if ( args_match( argv[i], "-fc", "--finalcomma" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_FINALCOMMA;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-s", "--single-refperfile" )){
	       p->singlerefperfile = 1;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-sd", "--singledash" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_SINGLEDASH;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-b", "--brackets" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_BRACKETS;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-w", "--whitespace" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_WHITESPACE;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-sk", "--strictkey" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_STRICTKEY;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-U", "--uppercase" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_UPPERCASE;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-at", "--abbreviated-titles" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_SHORTTITLE;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-nb", "--no-bom" ) ) {
	       p->utf8bom = 0;
	       subtract = 1;
	  } else if ( args_match( argv[i], "-d", "--drop-key" ) ) {
	       p->format_opts |= BIBL_FORMAT_BIBOUT_DROPKEY;
	       subtract = 1;
	  } else if ( args_match( argv[i], "--verbose", "" ) ) {
	       p->verbose = 1;
	       subtract = 1;
	  } else if ( args_match( argv[i], "--debug", "" ) ) {
	       p->verbose = 3;
	       subtract = 1;
	  }
	  if ( subtract ) {
	       for ( j=i+subtract; j<*argc; ++j )
		    argv[j-subtract] = argv[j];
	       *argc -= subtract;
	  } else {
	       if ( argv[i][0]=='-' ) REprintf("Warning did not recognize potential command-line option %s\n", argv[i] );
	       i++;
	  }
    
     }
}

// xml2any_main( int *argc, char *argv[], char *outfile[], double *nref )
void
bib2be_main( int *argcin, char *argv[], char *outfile[], double *nref)
{
     int argc = *argcin;
     // const char *progname = progname_in[0];
     const char *progname = argv[0];

     // REprintf("argc: %d\n", argc);
     // REprintf("argv[0]: %s\n", argv[0]);
     // REprintf("argv[1]: %s\n", argv[1]);
  
     param p;
     // int ihelp;

     bibtexdirectin_initparams( &p, progname );
     // ihelp = 0;

     // modsout_initparams( &p, progname );
     // tomods_processargs( &argc, argv, &p, help0[ihelp], help0[ihelp + 1] );

     // bibentryout_initparams( &p, progname ); // this works, kind of
     bibentrydirectout_initparams( &p, progname );
	
     process_charsets( &argc, argv, &p );

     // !!! TODO: this needs to be sorted out! !!!
     //
     process_direct_args( &argc, argv, &p, &progname );  // process_args( &argc, argv, &p );

     //Georgi
     //REprintf("OOOOh: p.latexout: %d, p.charsetout: %d\n", p.latexout, p.charsetout );
	
     *nref = bibprog( argc, argv, &p, outfile );
     // *nref = bibprog( argc[0], argv, &p, outfile );   // bibprog( argc, argv, &p );
     // Georgi, no need to print, returned to caller
     // if( p.progname ) REprintf( "%s: ", p.progname );
     // REprintf( "processed %g references.\n", *nref );

     bibl_freeparams( &p );

     bibdirectin_more_cleanf();
	
     *argcin = argc;
     // return EXIT_SUCCESS;
}
