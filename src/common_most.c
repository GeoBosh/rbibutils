#include <string.h>
#include <R.h>

#include "common_most.h"

// copacin.c
// bibtexin.c
// biblatexin.c
// isiin.c
// nbibin.c
// risin.c
void
common_report_notag( param *p, char *tag, char *val, char *fmt )
{
  if ( p->verbose && (strcmp( tag, val ) || !strcmp( val, "" )) ) {
		if ( p->progname ) REprintf( "%s: ", p->progname );
		REprintf( "Did not identify %s tag '%s'\n", fmt, tag );
	}
}
