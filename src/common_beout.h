/*
 * common_beout.h
 *
 * Copyright (c) Georgi N. Boshnakov 2022-2023
 *
 * Program and source code released under the GPL version 2
 *
 */
#ifndef COMMON_BEOUT_H
#define COMMON_BEOUT_H

void bibentrydirectout_writeheader( FILE *outptr, param *pm );
void bibentrydirectout_writefooter( FILE *outptr);

void name_build_bibentry_direct( str *s, const char *p );
void append_people_be( fields *in, char *tag, char *ctag, char *atag,
	   char *bibtag, int level, fields *out, int format_opts, int latex_out, int *status );

void append_key( fields *in, char *intag, char *outtag, fields *out, int *status );

#endif
