/*
 * xml2biblatex.c
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
#include "args.h"
#include "bibprog.h"

const char progname_biblatex[] = "xml2biblatex";

void
help_biblatex( char *progname )
{
	args_tellversion( progname );
	REprintf("Converts the MODS XML intermediate reference file "
			"into BibLaTex\n\n");

	REprintf("usage: %s xml_file > biblatex_file\n\n",progname);
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n");

	REprintf("  -h,  --help               display this help\n");
	REprintf("  -v,  --version            display version\n");
	REprintf("  -at, --abbreviatedtitles  use abbreviated titles, if available\n");
	REprintf("  -fc, --finalcomma         add final comman to biblatex output\n");
	REprintf("  -sd, --singledash         use one dash '-', not two '--', in page ranges\n" );
	REprintf("  -b,  --brackets           use brackets, not quotation marks surrounding data\n");
	REprintf("  -w,  --whitespace         use beautifying whitespace to output\n");
	REprintf("  -sk, --strictkey          use only alphanumeric characters for bibtex key\n");
	REprintf("                            (overly strict, but useful for other programs)\n");
	REprintf("  -nl, --no-latex           no latex encodings; put characters in directly\n");
	REprintf("  -nb, --no-bom             do not write Byte Order Mark in UTF8 output\n");
	REprintf("  -U,  --uppercase          write biblatex tags/types in upper case\n" );
	REprintf("  -s,  --single-refperfile  one reference per output file\n");
	REprintf("  -i, --input-encoding      interpret input file with requested character set\n" );
	REprintf("                            (use argument for current list)\n");
	REprintf("  -o, --output-encoding     write output file with requested character set\n" );
	REprintf("                            (use argument for current list)\n");
	REprintf("  --verbose                 for verbose\n" );
	REprintf("  --debug                   for debug output\n" );
	REprintf("\n");

	REprintf("Citation codes generated from <REFNUM> tag.   See \n");
	REprintf("http://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n");
}

void
process_args_biblatex( int *argc, char *argv[], param *p )
{
	int i, j, subtract;
	i = 1;
	while ( i<*argc ) {
		subtract = 0;
		if ( args_match( argv[i], "-h", "--help" ) ) {
			help_biblatex( p->progname );
			error("\n"); // exit( EXIT_SUCCESS );
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
		} else if ( args_match( argv[i], "-nl", "--no-latex" ) ) {
			p->latexout = 0;
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

// int
void
xml2biblatex_main( int *argc, char *argv[], char *outfile[] )
{
	param p;
	modsin_initparams( &p, progname_biblatex );
	biblatexout_initparams( &p, progname_biblatex );

	// process_charsets( &argc, argv, &p );
	process_charsets( argc, argv, &p );

	// process_args( &argc, argv, &p );
	process_args_biblatex( argc, argv, &p );

	// bibprog( argc, argv, &p );
	bibprog( argc[0], argv, &p, outfile );

	bibl_freeparams( &p );
	// return EXIT_SUCCESS;
}
