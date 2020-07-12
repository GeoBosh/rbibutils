/*
 * tomods.c
 *
 * Copyright (c) Chris Putnam 2004-2020
 *
 * Program and source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"
#include "bibl.h"
#include "bibutils.h"
#include "tomods.h"
#include "args.h"
#include "bibprog.h"

static void
args_tomods_help( char *progname, char *help1, char *help2 )
{
	args_tellversion( progname );
	REprintf("%s", help1 );

	REprintf("usage: %s %s > xml_file\n\n", progname, help2 );
        REprintf("  %s can be replaced with file list or "
			"omitted to use as a filter\n\n", help2 );

	REprintf("  -h, --help                display this help\n");
	REprintf("  -v, --version             display version\n");
	REprintf("  -a, --add-refcount        add \"_#\", where # is reference count to reference\n");
	REprintf("  -s, --single-refperfile   one reference per output file\n");
	REprintf("  -i, --input-encoding      input character encoding\n");
	REprintf("  -o, --output-encoding     output character encoding\n");
	REprintf("  -u, --unicode-characters  DEFAULT: write unicode (not xml entities)\n");
	REprintf("  -un,--unicode-no-bom      as -u, but don't write byte order mark\n");
	REprintf("  -x, --xml-entities        write xml entities and not direclty unicode\n");
	REprintf("  -nl,--no-latex            do not convert latex-style character combinations\n");
	REprintf("  -d, --drop-key            don't put key in MODS ID field\n");
	REprintf("  -c, --corporation-file    specify file of corporation names\n");
	REprintf("  -as, --asis               specify file of names that shouldn't be mangled\n");
	REprintf("  -nt, --nosplit-title      don't split titles into TITLE/SUBTITLE pairs\n");
	REprintf("  --verbose                 report all warnings\n");
	REprintf("  --debug                   very verbose output\n\n");

	REprintf("http://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n");
}

static void
args_namelist( int argc, char *argv[], int i, char *progname, char *shortarg, 
		char *longarg )
{
	if ( i+1 >= argc ) {
		REprintf( "%s: error %s (%s) takes the argument of "
			"the file\n", progname, shortarg, longarg );
		error("\n"); // error( EXIT_FAILURE );
	}
}

void
tomods_processargs( int *argc, char *argv[], param *p,
	char *help1, char *help2 )
{
	int i, j, subtract, status;

	process_charsets( argc, argv, p );

        i = 0;
	while ( i<*argc ) {
		subtract = 0;
		if ( args_match( argv[i], "-h", "--help" ) ) {
			args_tomods_help( p->progname, help1, help2 );
			error("\n"); // error( EXIT_SUCCESS );
		} else if ( args_match( argv[i], "-v", "--version" ) ) {
			args_tellversion( p->progname );
			error("\n"); // error( EXIT_SUCCESS );
		} else if ( args_match( argv[i], "-a", "--add-refcount" ) ) {
			p->addcount = 1;
			subtract = 1;
		} else if ( args_match(argv[i], NULL, "--verbose" ) ) {
			/* --debug + --verbose = --debug */
			if ( p->verbose<1 ) p->verbose = 1;
			p->format_opts |= BIBL_FORMAT_VERBOSE;
			subtract = 1;
		} else if ( args_match(argv[i], NULL, "--debug" ) ) {
			p->verbose = 3;
			p->format_opts |= BIBL_FORMAT_VERBOSE;
			subtract = 1;
		} else if ( args_match( argv[i], "-d", "--drop-key" ) ) {
			p->format_opts |= BIBL_FORMAT_MODSOUT_DROPKEY;
			subtract = 1;
		} else if ( args_match( argv[i], "-s", "--single-refperfile" )){
			p->singlerefperfile = 1;
			subtract = 1;
		} else if ( args_match( argv[i], "-u", "--unicode-characters")){
			p->utf8out = 1;
			p->utf8bom = 1;
			p->charsetout = BIBL_CHARSET_UNICODE;
			p->charsetout_src = BIBL_SRC_USER;
			subtract = 1;
		} else if ( args_match( argv[i], "-un", "--unicode-no-bom")){
			p->utf8out = 1;
			p->utf8bom = 0;
			p->charsetout = BIBL_CHARSET_UNICODE;
			p->charsetout_src = BIBL_SRC_USER;
			subtract = 1;
		} else if ( args_match( argv[i], "-nl", "--no-latex" ) ) {
			p->latexin = 0;
			subtract = 1;
		} else if ( args_match( argv[i], "-nt", "--nosplit-title" ) ){
			p->nosplittitle = 1;
			subtract = 1;
		} else if ( args_match( argv[i], "-x", "--xml-entities" ) ) {
			p->utf8out = 0;
			p->utf8bom = 0;
			p->xmlout = 1;
			subtract = 1;
		} else if ( args_match( argv[i], "-c", "--corporation-file")){
			args_namelist( *argc, argv, i, p->progname,
				"-c", "--corporation-file" );
			status = bibl_readcorps( p, argv[i+1] );
			if ( status == BIBL_ERR_MEMERR ) {
				REprintf( "%s: Memory error when reading --corporation-file '%s'\n",
					p->progname, argv[i+1] );
				error("\n"); // error( EXIT_FAILURE );
			} else if ( status == BIBL_ERR_CANTOPEN ) {
				REprintf( "%s: Cannot read --corporation-file '%s'\n",
					p->progname, argv[i+1] );
			}
			subtract = 2;
		} else if ( args_match( argv[i], "-as", "--asis")) {
			args_namelist( *argc, argv, i, p->progname,
				"-as", "--asis" );
			status = bibl_readasis( p, argv[i+1] );
			if ( status == BIBL_ERR_MEMERR ) {
				REprintf( "%s: Memory error when reading --asis file '%s'\n",
					p->progname, argv[i+1] );
				error("\n"); // error( EXIT_FAILURE );
			} else if ( status == BIBL_ERR_CANTOPEN ) {
				REprintf( "%s: Cannot read --asis file '%s'\n",
					p->progname, argv[i+1] );
			}
			subtract = 2;
		}
		if ( subtract ) {
			for ( j=i+subtract; j<*argc; j++ ) {
				argv[j-subtract] = argv[j];
			}
			*argc -= subtract;
		} else {
			if ( argv[i][0]=='-' ) REprintf( "Warning: Did not recognize potential command-line argument %s\n", argv[i] );
			i++;
		}
	}
}











