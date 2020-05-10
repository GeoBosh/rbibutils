/*
 * latex_parse.h
 */
#ifndef LATEX_PARSE_H
#define LATEX_PARSE_H

#include "slist.h"

int latex_parse( str *in, str *out );
int latex_tokenize( slist *tokens, str *s );


#endif
