/*
 * biblatex2xml.c
 *
 * Copyright (c) Chris Putnam 2003-2020
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

char help1[] = "Converts a biblatex (not to be confused with bibtex) reference file into MODS XML\n\n";
char help2[] = "bibtex_file";

const char progname2[] = "biblatex2xml";

// int
void
biblatex2xml_main( int *argc, char *argv[], char *outfile[] )
{
	param p;

        // REprintf("Argh..!");
	
	biblatexin_initparams( &p, progname2 );
	modsout_initparams( &p, progname2 );
	// tomods_processargs( argc, argv, &p, help1, help2 );
	bibprog( argc[0], argv, &p, outfile );
	bibl_freeparams( &p );
	// return EXIT_SUCCESS;
}


