/*
 * xml2any.c
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
#include "args.h"
#include "bibprog.h"

// const char progname[] = "xml2bib";

void
help_xml2bibtex( char *progname )
{
	args_tellversion( progname );
	REprintf( "Converts the MODS XML intermediate reference file "
			"into Bibtex\n\n");

	REprintf("usage: %s xml_file > bibtex_file\n\n",progname);
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n");

	REprintf("  -h,  --help               display this help\n");
	REprintf("  -v,  --version            display version\n");
	REprintf("  -at, --abbreviatedtitles  use abbreviated titles, if available\n");
	REprintf("  -fc, --finalcomma         add final comman to bibtex output\n");
	REprintf("  -sd, --singledash         use one dash '-', not two '--', in page ranges\n" );
	REprintf("  -b,  --brackets           use brackets, not quotation marks surrounding data\n");
	REprintf("  -w,  --whitespace         use beautifying whitespace to output\n");
	REprintf("  -sk, --strictkey          use only alphanumeric characters for bibtex key\n");
	REprintf("                            (overly strict, but useful for other programs)\n");
	REprintf("  -nl, --no-latex           no latex encodings; put characters in directly\n");
	REprintf("  -nb, --no-bom             do not write Byte Order Mark in UTF8 output\n");
	REprintf("  -U,  --uppercase          write bibtex tags/types in upper case\n" );
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
help_xml2biblatex( char *progname )
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
help_xml2end( char *progname )
{
	args_tellversion( progname );
	REprintf("Converts an XML intermediate reference file into a pre-EndNote format\n\n");

	REprintf("usage: %s xml_file > endnote_file\n\n", progname);
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n");

	REprintf("  -h, --help     display this help\n");
	REprintf("  -v, --version  display version\n\n");
	REprintf("  -nb, --no-bom   do not write Byte Order Mark in UTF8 output\n");
	REprintf("  -s, --single-refperfile one reference per output file\n");
	REprintf("  -i, --input-encoding interpret input file with requested character set (use\n" );
	REprintf("                       argument for current list)\n");
	REprintf("  -o, --output-encoding interprest output file with requested character set\n" );
	REprintf("  --verbose      for verbose output\n");
	REprintf("  --debug        for debug output\n");

	REprintf("http://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n");
}

void
help_xml2isi( char *progname )
{
	args_tellversion( progname );
	REprintf("Converts an XML intermediate reference file into ISI format\n\n");

	REprintf("usage: %s xml_file > isi_file\n\n", progname);
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n");

	REprintf("  -h, --help     display this help\n");
	REprintf("  -v, --version  display version\n\n");
	REprintf("  -nb, --no-bom  do not write Byte Order Mark in UTF8 output\n");
	REprintf("  -s, --single-refperfile one reference per output file\n");
	REprintf("  -i, --input-encoding  interpret input file with requested character set\n" );
	REprintf("                       (use w/o argument for current list)\n" );
	REprintf("  -o, --output-encoding write output file with requested character set\n" );
	REprintf("                       (use w/o argument for current list)\n" );
	REprintf("  --verbose      for verbose output\n");
	REprintf("  --debug        for debug output\n");

	REprintf("http://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n");
}


void
help_xml2nbib( char *progname )
{
	args_tellversion( progname );
	REprintf("Converts an XML intermediate reference file into NBIB format\n\n");

	REprintf("usage: %s xml_file > nbib_file\n\n", progname);
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n");

	REprintf("  -h, --help     display this help\n");
	REprintf("  -v, --version  display version\n\n");
	REprintf("  -nb, --no-bom  do not write Byte Order Mark in UTF8 output\n");
	REprintf("  -s, --single-refperfile one reference per output file\n");
	REprintf("  -i, --input-encoding  interpret input file with requested character set\n" );
	REprintf("                       (use w/o argument for current list)\n" );
	REprintf("  -o, --output-encoding write output file with requested character set\n" );
	REprintf("                       (use w/o argument for current list)\n" );
	REprintf("  --verbose      for verbose output\n");
	REprintf("  --debug        for debug output\n");

	REprintf("http://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n");
}

void
help_xml2ris( char *progname )
{
	args_tellversion( progname );
        REprintf("Converts an XML intermediate reference file into RIS format\n\n");
        
        REprintf("usage: %s xml_file > ris_file\n\n",progname);
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n");
        REprintf("  -h, --help     display this help\n");
        REprintf("  -v, --version  display version\n\n");
        REprintf("  -nb, --no-bom  do not write Byte Order Mark in UTF8 output\n");
        REprintf("  -s, --single-refperfile one reference per output file\n");
        REprintf("  -i, --input-encoding  interpret the input with specified character set\n" );
        REprintf("                        (use w/o argument for current list)\n" );
        REprintf("  -o, --output-encoding write the output with specified character set\n" );
        REprintf("                        (use w/o argument for current list)\n" );
        REprintf("  --verbose      for verbose output\n");
        REprintf("  --debug        for debug output\n");

	REprintf("Citation codes (ID  - ) generated from <REFNUM> tag.   See \n");
	REprintf("http://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n");
}

void
help_xml2wordbib( char *progname )
{
	args_tellversion( progname );
        REprintf("Converts a MODS XML bibliogrphy into Word2007 format bibliography\n\n");
        REprintf("usage: %s xml_file > word_file\n\n", progname );
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n" );
        REprintf("  -h, --help              display this help\n" );
        REprintf("  -v, --version           display version\n\n" );
        REprintf("  -nb, --no-bom           do not write Byte Order Mark if writing UTF8\n" );
        REprintf("  -s, --single-refperfile one reference per output file\n");
        REprintf("  -i, --input-encoding    interpret input file as using requested character set\n");
        REprintf("                          (use w/o argument for current list)\n" );
        REprintf("  --verbose               for verbose output\n" );
        REprintf("  --debug                 for debug output\n" );

        REprintf("http://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n" );
}


void
help_xml2ads( char *progname )
{
	args_tellversion( progname );
	REprintf("Converts an XML intermediate reference file into a ADS aabstracts format\n\n");

	REprintf("usage: %s xml_file > adsabs_file\n\n", progname );
        REprintf("  xml_file can be replaced with file list or omitted to use as a filter\n\n");
	REprintf("  -h, --help               display this help\n");
	REprintf("  -v, --version            display version\n");
	REprintf("  -nb, --no-bom            do not write Byte Order Mark in UTF8 output\n");
	REprintf("  -s, --single-refperfile  one reference per output file\n");
	REprintf("  --verbose                for verbose output\n");
	REprintf("  --debug                  for debug output\n");

	REprintf("\nhttp://sourceforge.net/p/bibutils/home/Bibutils for more details\n\n");
}



//void helpA( char *progname )
void (*helpAll[])(char *) = {
			     help_xml2bibtex,
			     help_xml2biblatex,
			     
			     help_xml2end,
			     help_xml2isi,
			     help_xml2nbib,
			     help_xml2ris,
			     help_xml2wordbib,
			     help_xml2ads
};

// extern void process_args( int *argc, char *argv[], param *p );

// xml2bib
void
process_args( int *argc, char *argv[], param *p, const char *progname[] )
{
  void (*help)( char *progname );
  
	if(strcmp(*progname, "xml2bib") == 0  || strcmp(*progname, "xml2biblatex") ){
	  if(strcmp(*progname, "xml2bib") == 0)
	    help = helpAll[0];
	  else
	    help = helpAll[1];
	  int i, j, subtract;
	  i = 1;
	  while ( i<*argc ) {
	  	subtract = 0;
	  	if ( args_match( argv[i], "-h", "--help" ) ) {
	  		help( p->progname );
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
	}else{ // the remaining xml2xxx
	  // process_args for the rest
	  int i, j, subtract;
	  i = 1;
	  while ( i<*argc ) {
	  	subtract = 0;
	  	if ( args_match( argv[i], "-h", "--help" ) ) {
	  		help_xml2end( p->progname );
	  		error("\n"); // exit( EXIT_SUCCESS );
	  	} else if ( args_match( argv[i], "-v", "--version" ) ) {
	  		args_tellversion( p->progname );
	  		error("\n"); // exit( EXIT_SUCCESS );
	  	} else if ( args_match( argv[i], "-s", "--single-refperfile")){
	  		p->singlerefperfile = 1;
	  		subtract = 1;
	  	} else if ( args_match( argv[i], "-nb", "--no-bom" ) ) {
	  		p->utf8bom = 0;
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
	  		if ( argv[i][0]=='-' ) REprintf( "Warning: Did not recognize potential command-line argument %s\n", argv[i] );
	  		i++;
	  	}
	  }
	}
}

// int
void
// xml2any_main( int *argc, char *argv[], char *outfile[], const char *progname_in[] )
xml2any_main( int *argc, char *argv[], char *outfile[], double *nref )
{
  // Georgi
  // REprintf("argc: %d\n", *argc);
  // for( int ii = 0; ii < *argc; ii++)
  //   REprintf("argv[%d]: %s\n", ii, argv[ii]);

  const char *progname = argv[0];

      	param p;
	// int ihelp;
	modsin_initparams( &p, progname );

	if(strcmp(progname, "xml2bib") == 0){
	  bibtexout_initparams( &p, progname );
	  // ihelp = 0;
	}else if(strcmp(progname,  "xml2biblatex") == 0){
	  biblatexout_initparams( &p, progname );
	  // ihelp = 2;
	}else if(strcmp(progname, "xml2copac") == 0){
	  // copacout_initparams( &p, progname );
	  // ihelp = 4;
	}else if(strcmp(progname, "xml2ebi") == 0){
	  // ebiout_initparams( &p, progname );
	  // ihelp = 6;
	}else if(strcmp(progname, "xml2end") == 0){
	  endout_initparams( &p, progname );
	  // ihelp = 8;
	}else if(strcmp(progname, "xml2endx") == 0){
	  // endxout_initparams( &p, progname );
	  // ihelp = 10;
	}else if(strcmp(progname, "xml2isi") == 0){
	  isiout_initparams( &p, progname );
	  // ihelp = 12;
	}else if(strcmp(progname, "xml2med") == 0){
	  // medout_initparams( &p, progname );
	  // ihelp = 14;
	}else if(strcmp(progname, "xml2nbib") == 0){
	  nbibout_initparams( &p, progname );
	  // ihelp = 16;
	}else if(strcmp(progname, "xml2ris") == 0){
	  risout_initparams( &p, progname );
	  // ihelp = 18;
	}else if(strcmp(progname, "xml2word") == 0){
	  wordout_initparams( &p, progname );
	  // ihelp = 20;
	}else if(strcmp(progname, "xml2ads") == 0){
	  adsout_initparams( &p, progname );
	  // ihelp = 22;
	}else if(strcmp(progname,  "xml2bibentry") == 0){
	  bibentryout_initparams( &p, progname );
	}else
	  error("cannot deduce input format from name %s", progname);
	
	process_charsets( argc, argv, &p );

	process_args( argc, argv, &p, &progname );          // process_args( &argc, argv, &p );

	//Georgi
	//REprintf("OOOOh: p.latexout: %d, p.charsetout: %d\n", p.latexout, p.charsetout );
	
	*nref = bibprog( argc[0], argv, &p, outfile );   // bibprog( argc, argv, &p );
	// Georgi, no need to print, returned to caller
	// if( p.progname ) REprintf( "%s: ", p.progname );
	// REprintf( "processed %g references.\n", *nref );


	bibl_freeparams( &p );
	// return EXIT_SUCCESS;
}

