/*
 * common_bt_blt_btd_out.h
 *
 * Copyright (c) Georgi N. Boshnakov 2022-2023
 *
 * Program and source code released under the GPL version 2
 *
 */

#ifndef COMMON_BT_BLT_BTD_OUT_H
#define COMMON_BT_BLT_BTD_OUT_H

enum {
	TYPE_UNKNOWN = 0,
	TYPE_ARTICLE,
	TYPE_INBOOK,
	TYPE_INPROCEEDINGS,
	TYPE_PROCEEDINGS,
	TYPE_INCOLLECTION,
	TYPE_COLLECTION,
	TYPE_BOOK,
	TYPE_PHDTHESIS,        /* legacy */
	TYPE_MASTERSTHESIS,    /* legacy */
	TYPE_DIPLOMATHESIS,
	TYPE_REPORT,
	TYPE_MANUAL,
	TYPE_UNPUBLISHED,
	TYPE_ELECTRONIC,       /* legacy */
	TYPE_MISC,
	TYPE_SUPPPERIODICAL,
	TYPE_CONFERENCE,       /* legacy */
	TYPE_SUPPCOLLECTION,
	TYPE_REFERENCE,
	TYPE_MVREFERENCE,
	TYPE_BOOKLET,
	TYPE_SUPPBOOK,
	TYPE_TECHREPORT,
	TYPE_PATENT,
	TYPE_ONLINE,
	TYPE_WWW,              /* jurabib compatibility */
	NUM_TYPES
};

// TYPE_SUPPPERIODICAL must be just after the last bibtex type above (i.e. MISC) !!!
enum {
      NUM_BIBTEX_TYPES = TYPE_SUPPPERIODICAL
};

enum {
      NUM_BIBENTRY_TYPES = NUM_BIBTEX_TYPES
};

void append_citekey( fields *in, fields *out, int format_opts, int *status );
void append_simple( fields *in, char *intag, char *outtag, fields *out, int *status );
void append_simpleall( fields *in, char *intag, char *outtag, fields *out, int *status );
void append_keywords( fields *in, fields *out, int *status );
void append_fileattach( fields *in, fields *out, int *status );

void append_titles( fields *in, int type, fields *out, int format_opts, int *status );
void append_date( fields *in, fields *out, int *status );
void append_arxiv( fields *in, fields *out, int *status );
void append_urls( fields *in, fields *out, int *status );
void append_isi( fields *in, fields *out, int *status );

void append_articlenumber( fields *in, fields *out, int *status );
void append_pages( fields *in, fields *out, int format_opts, int *status );
void append_issue_number( fields *in, fields *out, int *status );
void append_howpublished( fields *in, fields *out, int *status );

#endif
