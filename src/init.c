#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* .C calls */

extern void bibl_freeparams( void * );

extern void any2xml_main( int *argcin, char *argv[], char *outfile[], double *nref );
extern void xml2any_main( int *argcin, char *argv[], char *outfile[], double *nref );


// extern void biblatex2xml_main( int argc, char *argv[], char *outfile[] );
// extern void bib2xml_main( int argc, char *argv[], char *outfile[] );
// extern void copac2xml_main( int argc, char *argv[], char *outfile[] );
// extern void ebi2xml_main( int argc, char *argv[], char *outfile[] );
// extern void endx2xml_main( int argc, char *argv[], char *outfile[] );
// extern void isi2xml_main( int argc, char *argv[], char *outfile[] );
// extern void med2xml_main( int argc, char *argv[], char *outfile[] );
// extern void nbib2xml_main( int argc, char *argv[], char *outfile[] );
// extern void ris2xml_main( int argc, char *argv[], char *outfile[] );
// extern void wordbib2xml_main( int argc, char *argv[], char *outfile[] );

// extern void xml2ads_main( int argc, char *argv[], char *outfile[], const char *progname );
// extern void xml2bib_main( int argc, char *argv[], char *outfile[], const char *progname );
// extern void xml2biblatex_main( int argc, char *argv[], char *outfile[], const char *progname );
// extern void xml2end_main( int argc, char *argv[], char *outfile[], const char *progname );
// extern void xml2isi_main( int argc, char *argv[], char *outfile[], const char *progname );
// extern void xml2nbib_main( int argc, char *argv[], char *outfile[], const char *progname );
// extern void xml2ris_main( int argc, char *argv[], char *outfile[], const char *progname );
// extern void xml2wordbib_main( int argc, char *argv[], char *outfile[], const char *progname );
  
static const R_CMethodDef CEntries[] = {
  {"bibl_freeparams", (DL_FUNC) &bibl_freeparams, 1},

  {"any2xml_main", (DL_FUNC) &any2xml_main, 4},
  {"xml2any_main", (DL_FUNC) &xml2any_main, 4},
  
//   {"biblatex2xml_main", (DL_FUNC) &biblatex2xml_main, 3},
//   {"bib2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"copac2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"ebi2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"end2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"endx2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"isi2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"med2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"nbib2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"ris2xml_main", (DL_FUNC) &bib2xml_main, 3},
//   {"wordbib2xml_main", (DL_FUNC) &bib2xml_main, 3},
  
//   {"xml2bib_main", (DL_FUNC) &xml2bib_main, 4},
//   {"xml2biblatex_main", (DL_FUNC) &xml2biblatex_main, 4},
//   {"xml2ads_main", (DL_FUNC) &xml2bib_main, 4},
//   {"xml2end_main", (DL_FUNC) &xml2bib_main, 4},
//   {"xml2isi_main", (DL_FUNC) &xml2bib_main, 4},
//   {"xml2nbib_main", (DL_FUNC) &xml2bib_main, 4},
//   {"xml2ris_main", (DL_FUNC) &xml2bib_main, 4},
//   {"xml2wordbib_main", (DL_FUNC) &xml2bib_main, 4},
  {NULL, NULL, 0}
};

void R_init_rbibutils(DllInfo *dll)
{
  R_registerRoutines(dll, CEntries, NULL, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
  // R_forceSymbols(dll, TRUE);
}
