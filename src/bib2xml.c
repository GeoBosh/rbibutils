/*
 * bib2xml.c
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

const char progname1[] = "bib2xml";

char help1bib[] = "Converts a Bibtex reference file into MODS XML\n\n";
char help2bib[] = "bibtex_file";

// int
void
bib2xml_main( int *argc, char *argv[], char *outfile[] )
{
	param p;
	bibtexin_initparams( &p, progname1 );
	modsout_initparams( &p, progname1 );
	// tomods_processargs( &argc, argv, &p, help1bib, help2bib );
	bibprog( argc[0], argv, &p, outfile );
	bibl_freeparams( &p );
	// return EXIT_SUCCESS;
}
