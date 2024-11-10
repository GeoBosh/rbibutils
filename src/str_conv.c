/*
 * str_conv.c
 *
 * Copyright (c) Chris Putnam 1999-2020
 * Copyright (c) Georgi N. Boshnakov 2020-2023
 *
 * Source code released under the GPL version 2
 *
 * str routines for converting strs between character sets
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "latex.h"
#include "entities.h"
#include "utf8.h"
#include "gb18030.h"
#include "charsets.h"
#include "str_conv.h"

#include <R.h>

int export_tex_chars_only = 0; // Georgi

static void
addentity( str *s, unsigned int ch )
{
	char buf[512];
	snprintf( buf, 512, "&#%u;", ch );
	str_strcatc( s, buf );
}

/* These are the five minimal predefined entites in XML */
static int
minimalxmlchars( str *s, unsigned int ch )
{
	if ( ch==34 )      { str_strcatc( s, "&quot;" ); return 1; }
	else if ( ch==38 ) { str_strcatc( s, "&amp;" );  return 1; }
	else if ( ch==39 ) { str_strcatc( s, "&apos;" ); return 1; }
	else if ( ch==60 ) { str_strcatc( s, "&lt;" );   return 1; }
	else if ( ch==62 ) { str_strcatc( s, "&gt;" );   return 1; }
	return 0;
}

static void
addxmlchar( str *s, unsigned int ch )
{
	if ( minimalxmlchars( s, ch ) ) return;
	if ( ch > 127 ) addentity( s, ch );
	else str_addchar( s, ch );
}

static void
addutf8char( str *s, unsigned int ch, int xmlout )
{
	unsigned char code[6];
	int nc, i;
	if ( xmlout ) {
		if ( minimalxmlchars( s, ch ) ) return;
		if ( ch > 127 && xmlout == STR_CONV_XMLOUT_ENTITIES )
			{ addentity( s, ch ); return; }
	}
	nc = utf8_encode( ch, code );
	for ( i=0; i<nc; ++i )
		str_addchar( s, code[i] );
}

static void
addgb18030char( str *s, unsigned int ch, int xmlout )
{
	unsigned char code[4];
	int nc, i;
	if ( xmlout ) {
		if ( minimalxmlchars( s, ch ) ) return;
		if ( ch > 127 && xmlout == STR_CONV_XMLOUT_ENTITIES )
			{ addentity( s, ch ); return; }
	}
	nc = gb18030_encode( ch, code );
	for ( i=0; i<nc; ++i )
		str_addchar( s, code[i] );
}

static void
addlatexchar( str *s, unsigned int ch, int xmlout, int utf8out )
{
	char buf[512];

	
	uni2latex( ch, buf, sizeof( buf ) );

	// Georgi
	if(export_tex_chars_only) {
	  if( ch == 36  || ch == 123 || ch == 125 ) { // '$', '{', '}'
	    str_addchar(s, (char) ch);
	    return;
	  }
	  else if( !strcmp(buf, "{\\backslash}") ) {
	    str_addchar(s, '\\');
	    return;
	  }
	}
	
	/* If the unicode character isn't recognized as latex output
	 * a '?' unless the user has requested unicode output.  If so,
	 * output the unicode.
	 */
	if ( utf8out && !strcmp( buf, "?" ) ) {
		addutf8char( s, ch, xmlout );
	} else {
		str_strcatc( s, buf );
	}
}

/*
 * get_unicode()
 * 
 *   This can be a little tricky.  If the character is simply encoded
 *   such as UTF8 for > 128 or by numeric xml entities such as "&#534;"
 *   then the output of decode_entity() and utf8_decode will necessarily
 *   be in the charsetin character set.  On the other hand, if it's a
 *   fancy latex expression, such as "\alpha", or a non-numeric xml entity
 *   like "&amp;", then we'll get the Unicode value (because our lists only
 *   keep the Unicode equivalent).
 *
 *   The unicode variable indicates whether or not a Unicode-based listing
 *   was used to convert the character (remember that charsetin could be
 *   Unicode independently).
 *
 *   The charset variable is used to keep track of what character set
 *   the character is in prior to conversion.
 *
 */

static unsigned int
get_unicode( str *s, unsigned int *pi, int charsetin, int latexin, int utf8in, int xmlin )
{
	unsigned int ch;
	int unicode = 0, err = 0;

	// REprintf("s->data[*pi] = %d\n", s->data[*pi]);

	if ( xmlin && s->data[*pi]=='&' ) {
		ch = decode_entity( s->data, pi, &unicode, &err );
	} else if ( charsetin==CHARSET_GB18030 ) {
		ch = gb18030_decode( s->data, pi );
		unicode = 1;
	} else if ( latexin ) {
		/* Must handle bibtex files in UTF8/Unicode */
		if ( utf8in && ( s->data[*pi] & 128 ) ) {
			ch = utf8_decode( s->data, pi );
			unicode = 1;
		} else ch = latex2char( s->data, pi, &unicode );
	}
	else if ( utf8in )
		ch = utf8_decode( s->data, pi );
	else {
		ch = (unsigned int) s->data[*pi];
		*pi = *pi + 1;
	}
	if ( !unicode && charsetin!=CHARSET_UNICODE )
		ch = charset_lookupchar( charsetin, ch );
	
	// REprintf("(get_unicode) ch: %d, latexin: %d, utf8in: %d, charsetin = %d\n",
	//	                ch,     latexin,     utf8in,      charsetin);
	
	// REprintf("(at end of get_unicode) s = %s\n", s->data);
	return ch;
}

static int
write_unicode( str *s, unsigned int ch, int charsetout, int latexout,
		int utf8out, int xmlout )
{
	unsigned int c;

	// Georgi
	// REprintf("(write_unicode) ch: %d, latexout: %d, utf8out: %d, charsetout = %d\n\n",
	// 	                  ch,     latexout,      utf8out,    charsetout);
	
	if ( latexout ) {
		addlatexchar( s, ch, xmlout, utf8out );
	} else if ( utf8out ) {
		addutf8char( s, ch, xmlout );
	} else if ( charsetout==CHARSET_GB18030 ) {
		addgb18030char( s, ch, xmlout );
	} else {
		c = charset_lookupuni( charsetout, ch );
		if ( xmlout ) addxmlchar( s, c );
		else str_addchar( s, c );

		// Georgi
		// REprintf("kiki: %x", c);
		  
	}

	return 1;
}

/*
 * Returns 1 on memory error condition
 */
int
str_convert( str *s,
	int charsetin,  int latexin,  int utf8in,  int xmlin,
	int charsetout, int latexout, int utf8out, int xmlout )
{
	unsigned int pos = 0;
	unsigned int ch;
	str ns;
	int ok = 1;

	if ( !s || s->len==0 ) return ok;
	// REprintf("(str_convert): s = %s\n", s->data);

	/* Ensure that string is internally allocated.
	 * This fixes NULL pointer derefernce in CVE-2018-10775 in bibutils
	 * as a string with a valid data pointer is potentially replaced
	 * by a string without a valid data pointer due to it being invalid
	 * unicode.
	 * This probably also fixes CVE-2018-10773 and CVE-2018-10774 which
	 * are NULL dereferences also likely due to a fuzzer, but without
	 * test cases in the report, I can't be completely sure.
	 */
	str_initstrc( &ns, "" );

	if ( charsetin==CHARSET_UNKNOWN ) charsetin = CHARSET_DEFAULT;
	if ( charsetout==CHARSET_UNKNOWN ) charsetout = CHARSET_DEFAULT;

	while ( s->data[pos] ) {
	// REprintf("(str_convert): pos = %d\n", pos);
	// REprintf("(str_convert): s = %s\n", s->data);
		ch = get_unicode( s, &pos, charsetin, latexin, utf8in, xmlin );
		ok = write_unicode( &ns, ch, charsetout, latexout, utf8out, xmlout );
		if ( !ok ) goto out;
		// test output for the bug in v2.4.5 and 2.4.6 (from isiout.c)
		//    get_unicode went over the final NULL in that case,
		//    TODO: maybe make it more robust?
		// if(pos > s->len)
		//   REprintf("(str_convert) pos past the NULL byte!: pos = %d, s->len = %d, s->data = %s\n",
		// 	   pos, s->len, s->data);
	}

	str_swapstrings( s, &ns );
out:
	str_free( &ns );

	return ok;
}
