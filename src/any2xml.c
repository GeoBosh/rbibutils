/*
 * any2xml.c
 *
 * Copyright (c) Georgi N. Boshnakov 2020 
 * 
 * The code in this file is based on xxx2yyy utilities by Chris Putnam 2003-2020
 * Reponsibility for any bugs introduced in this adaptation lies with GNB.
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

char *help0[] = {
		 /* bib2xml */
		 "Converts a Bibtex reference file into MODS XML\n\n",
		 "bibtex_file",

		 /* biblatex2xml */
		 "Converts a biblatex (not to be confused with bibtex) reference file into MODS XML\n\n",
		 "bibtex_file",

		 /* copac2xml */
		 "Converts a Copac reference file into MODS XML\n\n",
		 "copac_file",

		 /* ebi2xml */
		 "Converts an EBI XML file into MODS XML\n\n",
		 "ebi_file",

		 /* end2xml */
		 "Converts an Endnote reference file (refer format) " "into MODS XML\n\n",
		 "endnote_file",

		 /* endx2xml */
		 "Converts a Endnote XML file (v8 or later) into MODS XML\n\n",
		 "endnotexml_file",                                            

		 /* isi2xml */
		 "Converts a ISI reference file into MODS XML\n\n",
		 "isi_file",

		 /* med2xml */
		  "Converts a Medline XML file into MODS XML\n\n",
		 "medline_file",

		 /* nbib2xml */
		 "Converts a NCBI NBIB reference file into MODS XML\n\n",
                 "nbib_file",

		 /* ris2xml */
		 "Converts a RIS reference file into MODS XML",
		 "ris_file",
		 
		 /* wordbib2xml */ 
		 "Converts a Word2007 Bibliography XML file into MODS XML\n\n",
		 "word2007bib_file"                                           
};

// int
void
//any2xml_main( int *argcin, char *argv[], char *outfile[], const char *progname_in[] )
any2xml_main( int *argcin, char *argv[], char *outfile[], double *nref)
{
  int argc = *argcin;
  // const char *progname = progname_in[0];
  const char *progname = argv[0];

  // REprintf("argc: %d\n", argc);
  // REprintf("argv[0]: %s\n", argv[0]);
  // REprintf("argv[1]: %s\n", argv[1]);
  
	param p;
	int ihelp;
	if(strcmp(progname, "bib2xml") == 0){
	  bibtexin_initparams( &p, progname );
	  ihelp = 0;
	}else if(strcmp(progname, "biblatex2xml") == 0){
	  biblatexin_initparams( &p, progname );
	  ihelp = 2;
	}else if(strcmp(progname, "copac2xml") == 0){
	  copacin_initparams( &p, progname );
	  ihelp = 4;
	}else if(strcmp(progname, "ebi2xml") == 0){
	  ebiin_initparams( &p, progname );
	  ihelp = 6;
	}else if(strcmp(progname, "end2xml") == 0){
	  endin_initparams( &p, progname );
	  ihelp = 8;
	}else if(strcmp(progname, "endx2xml") == 0){
	  endxmlin_initparams( &p, progname );
	  ihelp = 10;
	}else if(strcmp(progname, "isi2xml") == 0){
	  isiin_initparams( &p, progname );
	  ihelp = 12;
	}else if(strcmp(progname, "med2xml") == 0){
	  medin_initparams( &p, progname );
	  ihelp = 14;
	}else if(strcmp(progname, "nbib2xml") == 0){
	  nbibin_initparams( &p, progname );
	  ihelp = 16;
	}else if(strcmp(progname, "ris2xml") == 0){
	  risin_initparams( &p, progname );
	  ihelp = 18;
	}else if(strcmp(progname, "wordbib2xml") == 0){
	  wordin_initparams( &p, progname );
	  ihelp = 20;
	}else if(strcmp(progname, "ads2xml") == 0){
	  error("import from ADS abstracts format not implemented");
	  // adsin_initparams( &p, progname );
	  ihelp = 22;
	}else
	  error("cannot deduce input format from name %s", progname);

	modsout_initparams( &p, progname );
	tomods_processargs( &argc, argv, &p, help0[ihelp], help0[ihelp + 1] );

	*nref = bibprog( argc, argv, &p, outfile );
	// Georgi, no need to print, returned to caller
	// if( p.progname ) REprintf( "%s: ", p.progname );
	// REprintf( "processed %g references.\n", *nref );


	bibl_freeparams( &p );

	*argcin = argc;
	// return EXIT_SUCCESS;
}
