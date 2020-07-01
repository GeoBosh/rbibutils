#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

// #include "bibutils.h"
// #include "bibformats.h"


/* .C calls */

// extern void poly_mult(void *, void *, void *, void *, void *);
extern void bibl_freeparams( void * );
extern void biblatex2xml_main( int argc, char *argv[], char *outfile[] );
extern void bib2xml_main( int argc, char *argv[], char *outfile[] );
extern void xml2bib_main( int argc, char *argv[], char *outfile[] );
  
static const R_CMethodDef CEntries[] = {
  {"bibl_freeparams", (DL_FUNC) &bibl_freeparams, 1},
  {"biblatex2xml_main", (DL_FUNC) &biblatex2xml_main, 3},
  {"bib2xml_main", (DL_FUNC) &bib2xml_main, 3},
  {"xml2bib_main", (DL_FUNC) &xml2bib_main, 3},
  {NULL, NULL, 0}
};

void R_init_bibutils(DllInfo *dll)
{
  R_registerRoutines(dll, CEntries, NULL, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
  // R_forceSymbols(dll, TRUE);
}
