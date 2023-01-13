/*
 * bibtexdirectin.c
 *
 * Copyright (c) Georgi N. Boshnakov 2021-2023
 *
 * Program and source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "is_ws.h"
#include "intlist.h"
#include "str.h"
#include "utf8.h"
#include "str_conv.h"
#include "fields.h"
#include "slist.h"
#include "name.h"
#include "title.h"
#include "url.h"
#include "reftypes.h"
#include "latex_parse.h"
#include "bibformats.h"
#include "generic.h"

#include "R.h"

#include "common_bt_btd_blt.h"
#include "common_bt_btd.h"

extern slist find;
extern slist replace;

extern int rdpack_patch_for_i_acute_variant;
extern int convert_latex_escapes_only; // Georgi
extern int export_tex_chars_only;      // Georgi

/*****************************************************
 PUBLIC: void bibtexdirectin_initparams()
*****************************************************/

static int bibtexdirectin_cleanf( bibl *bin, param *p );

int
bibtexdirectin_initparams( param *pm, const char *progname )
{
	pm->readformat       = BIBL_BIBTEXIN;
	pm->charsetin        = BIBL_CHARSET_DEFAULT;
	pm->charsetin_src    = BIBL_SRC_DEFAULT;
	pm->latexin          = 1;
	pm->xmlin            = 0;
	pm->utf8in           = 0;
	pm->nosplittitle     = 0;
	pm->verbose          = 0;
	pm->addcount         = 0;
	pm->output_raw       = BIBL_RAW_WITHCLEAN |
	                      BIBL_RAW_WITHMAKEREFID |
	                      BIBL_RAW_WITHCHARCONVERT;

	pm->readf    = bibtexin_readf;
	pm->processf = bibtexin_processf;
	pm->cleanf   = bibtexdirectin_cleanf;
	pm->typef    = bibtexin_typef;
	pm->convertf = NULL;
	pm->all      = bibtex_all;
	pm->nall     = bibtex_nall;

	slist_init( &(pm->asis) );
	slist_init( &(pm->corps) );

	//    note that 'find' and 'replace' work in tandem, so both need to be cleared.
	// slist_free( &find );
	// slist_free( &replace );

	if ( !progname ) pm->progname = NULL;
	else {
		pm->progname = strdup( progname );
		if ( pm->progname==NULL ) return BIBL_ERR_MEMERR;
	}

	return BIBL_OK;
}

// Georgi
// void (*more_cleanf)()
void bibdirectin_more_cleanf( void )
{
  // TODO: these probably should be made parameters, as the others above;
  //       note that 'find' and 'replace' work in tandem, so both need to be cleared.
  slist_free( &find );
  slist_free( &replace );
  convert_latex_escapes_only = 0;
  export_tex_chars_only = 0;
  rdpack_patch_for_i_acute_variant = 0;

}

/*****************************************************
 PUBLIC: void bibtexdirectin_cleanf()
*****************************************************/

static int
bibtexdirectin_cleanref( fields *bibin, param *pm )
{
     int i, n, fstatus, status = BIBL_OK;
     str *tag, *value;
     intlist toremove;

     intlist_init( &toremove );

     n = fields_num( bibin );

     // REprintf("\nbibtexdirectin_cleanref (start): n = %d\n" , n);
     // for(i = 0; i < n; i++) {
     //   REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
     // }

     for ( i=0; i<n; ++i ) {

	  tag = fields_tag( bibin, i, FIELDS_STRP_NOUSE );
	  // REprintf("\nbibtexdirectin_cleanref: tag = %s", tag->data);
	  if ( is_url_tag( tag ) ) continue; /* protect url from parsing */

	  /* Georgi:  protecting names, otherwise havoc ensues if the input is 
	     in a different encoding; 
	     TODO: test side effects of doing this.
	     delay names from undergoing any parsing */
	  /* 2020-09-26: but names need parsing since there may be more than one!
	     Commenting out to process properly names fields

	     TODO: return to this and check again!
	     I commented this out because of encodings - do tests!
	     Amendment: run the nex two lines  but only if tag is not names tag
	     (actually, moved them to the else part)
	  */
	  // if ( is_name_tag( tag ) ) return BIBL_OK;
	  // if ( !is_name_tag( tag ) ){
	  value = fields_value( bibin, i, FIELDS_STRP_NOUSE );
	  if ( str_is_empty( value ) ) continue;

	  // }

	  if(convert_latex_escapes_only) { // convert
	       str_convert( value,
			    // pm->charsetin,  pm->latexin,  pm->utf8in,  pm->xmlin,
			    pm->charsetin,  1,  pm->utf8in,  pm->xmlin,
			    // pm->charsetout, pm->latexout, pm->utf8out, pm->xmlout );
			    pm->charsetout, 0, pm->utf8out, pm->xmlout );
	  }
	
	  if(rdpack_patch_for_i_acute_variant) {
	    // This may introduce {{\\'\\i}} if \\'i was already in braces.
	    // In names this doesn't matter since braces are removed anyway.
	    // In other fields the superfluous braces probably don't matter either
	    // but they are cleared below in the 'else' branch.
	       str_findreplace(value, "\\'i", "{\\'\\i}");
	  }

	  if ( is_name_tag( tag ) ) {
	       status = bibtexin_person( bibin, i, pm );
						
	       // REprintf("\tbefore: i = %d, ", i);
	       // REprintf("value = %s\n", (bibin->value[i]).data);
	       // REprintf("\tafter:  newpos = %d, ", fields_num( bibin ) - 1);
	       // REprintf("value = %s\n", (bibin->value[fields_num( bibin ) - 1]).data);
 
	       if ( status!=BIBL_OK ) goto out;

	       fstatus = intlist_add( &toremove, i );
	       if ( fstatus!=INTLIST_OK ) { status = BIBL_ERR_MEMERR; goto out; }
	       // REprintf("nout = %d\n" , fields_num( bibin ));
	       // goto out;
	  }
	  else {
	    //   status = bibtex_cleanvalue( value );
	    if(rdpack_patch_for_i_acute_variant) {
	      str_findreplace(value, "{{\\'\\i}}",  "{\\'\\i}");
	    }
	    //   if ( status!=BIBL_OK ) goto out;
	    //   
	    //   REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
	    //   
	  }

     }


     // int nout = fields_num( bibin );
     // if(nout > n) {
     //   REprintf("\nbibtexdirectin_cleanref (2): nout = %d\n" , nout);
     //   for(i = 0; i < nout; i++) {
     //     REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
     //   }
     //   
     // }

	
     for ( i=toremove.n-1; i>=0; i-- ) {
	  fstatus = fields_remove( bibin, intlist_get( &toremove, i ) );
	  if ( fstatus!=FIELDS_OK ) { status = BIBL_ERR_MEMERR; goto out; }
     }
	
out:

     intlist_free( &toremove );
	
     // nout = fields_num( bibin );
     // if(nout > n) {
     //   REprintf("\nbibtexdirectin_cleanref (end): nout = %d\n" , nout);
     //   for(i = 0; i < nout; i++) {
     //     REprintf("i = %d, value = %s\n", i, (bibin->value[i]).data);
     //   }
     //   
     // }
	
     return status;
}

static int
bibtexdirectin_cleanf( bibl *bin, param *p )
{
	int status;
	long i;

        for ( i=0; i<bin->n; ++i ) {
		status = bibtexdirectin_cleanref( bin->ref[i], p );
		if ( status!=BIBL_OK ) return status;
	}
	status = bibtexin_crossref( bin, p );
	return status;
}
