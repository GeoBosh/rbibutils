/*
 * common_bt_btd.h
 *
 * Copyright (c) Georgi N. Boshnakov 2022-2023
 *
 * Program and source code released under the GPL version 2
 *
 */
#ifndef COMMON_BT_BTD_H
#define COMMON_BT_BTD_H

#include "fields.h"
#include "bibutils.h"


extern variants bibtex_all[];
extern int bibtex_nall;

int bibtexin_processf( fields *bibin, const char *data, const char *filename, long nref, param *pm );

int is_url_tag( str *tag );
int is_name_tag( str *tag );

int bibtexin_crossref( bibl *bin, param *p );
int bibtexin_typef( fields *bibin, const char *filename, int nrefs, param *p );

int bibtexin_person( fields *bibin, int m, param *pm );

#endif
