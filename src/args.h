/*
 * args.h
 *
 * Copyright (c) Chris Putnam 2012-2020
 *
 * Program and source code released under the GPL version 2
 *
 */
#ifndef ARGS_H
#define ARGS_H

void  args_tellversion( const char *progname );
int   args_match( const char *check, const char *shortarg, const char *longarg );
char *args_next( int argc, char *argv[], int n, const char *progname, const char *shortarg, const char *longarg );
void  process_charsets( int *argc, char *argv[], param *p );

#endif
