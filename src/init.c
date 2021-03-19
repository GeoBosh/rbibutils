/*
 * init.c
 *
 * Copyright (c) Georgi N. Boshnakov 2020
 *
 * Source code released under the GPL version 2
 *
 */
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* .C calls */

extern void bibl_freeparams( void * );

extern void any2xml_main( int *argcin, char *argv[], char *outfile[], double *nref );
extern void xml2any_main( int *argcin, char *argv[], char *outfile[], double *nref );
extern void bib2be_main(  int *argcin, char *argv[], char *outfile[], double *nref );
  
static const R_CMethodDef CEntries[] = {
  {"bibl_freeparams", (DL_FUNC) &bibl_freeparams, 1},

  {"any2xml_main", (DL_FUNC) &any2xml_main, 4},
  {"xml2any_main", (DL_FUNC) &xml2any_main, 4},
  {"bib2be_main",  (DL_FUNC) &bib2be_main, 4},
  
  {NULL, NULL, 0}
};

void R_init_rbibutils(DllInfo *dll)
{
  R_registerRoutines(dll, CEntries, NULL, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
  // R_forceSymbols(dll, TRUE);
}
