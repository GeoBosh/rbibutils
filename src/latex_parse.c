/*
 * latex_parse.c
 *
 * Parse LaTeX code.
 *
 * Copyright (c) Chris Putnam 2020
 * Copyright (c) Georgi N. Boshnakov 2020
 *
 * Source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <R.h>

#include "bibdefs.h"
#include "is_ws.h"
#include "latex_parse.h"

typedef struct latex_node {
	struct latex_edge *next_edge;
	struct latex_node *down_node;
} latex_node;

typedef struct latex_edge {
	struct latex_node *next_node;
	str text;
} latex_edge;

static latex_node *
latex_node_new( void )
{
	return ( latex_node * ) calloc( 1, sizeof( latex_node ) );
}

static void
latex_node_delete( latex_node *n )
{
	free( n );
}

static latex_edge *
latex_edge_new( void )
{
	latex_edge *e;
	e = ( latex_edge * ) calloc( 1, sizeof( latex_edge ) );
	if ( e ) str_init( &(e->text) );
	return e;
}

static void
latex_edge_delete( latex_edge *e )
{
	str_free( &(e->text) );
	free( e );
}

static int
is_unescaped( char *p, unsigned long *offset, char c )
{
	if ( *p!=c ) return 0;
	if ( *offset > 0 && *(p-1)=='\\' ) return 0;
	return 1;
}

static int
build_latex_graph_r( str *in, unsigned long *offset, int *mathmode, int depth, latex_node **node )
{
	latex_node *newnode, *downnode;
	int status = BIBL_OK;
	latex_edge *newedge;
	char *p;

	newnode = latex_node_new();
	if ( !newnode ) return BIBL_ERR_MEMERR;

	newedge = latex_edge_new();
	if ( !newedge ) {
		latex_node_delete( newnode );
		return BIBL_ERR_MEMERR;
	}

	*node = newnode;

	newnode->next_edge = newedge;

	p = str_cstr( in ) + *offset;

	while ( *p ) {
	  
		if ( is_unescaped( p, offset, '{' ) ) {
		  // REprintf("unescaped {\n");
			*offset += 1;
			newnode = latex_node_new();
			if ( !newnode ) { status = BIBL_ERR_MEMERR; goto out; }
			newedge->next_node = newnode;
			newedge = latex_edge_new();
			if ( !newedge ) { status = BIBL_ERR_MEMERR; goto out; }
			newnode->next_edge = newedge;
			status = build_latex_graph_r( in, offset, mathmode, depth+1, &downnode );
			if ( status!=BIBL_OK ) goto out;
			newnode->down_node = downnode;
			p = str_cstr( in ) + *offset;
		}
		else if ( is_unescaped( p, offset, '}' ) ) {
		  // REprintf("unescaped }\n");
			*offset += 1;
			if ( depth==0 ) {
				REprintf( "Unmatched '}' character in LaTeX encoding '%s'.\n", str_cstr( in ) );
				p++;
				continue;
			}
			goto out;
		}
		else if ( is_unescaped( p, offset, '$' ) ) {
		  // REprintf("unescaped $ ");
 
			*mathmode = !(*mathmode);
			*offset += 1;
			if ( *mathmode ) {
			  // REprintf("mathmode!\n");
				newnode = latex_node_new();
				if ( !newnode ) { status = BIBL_ERR_MEMERR; goto out; }
				newedge->next_node = newnode;
				newedge = latex_edge_new();
				if ( !newedge ) { status = BIBL_ERR_MEMERR; goto out; }
				newnode->next_edge = newedge;
				status = build_latex_graph_r( in, offset, mathmode, depth+1, &downnode );
				if ( status!=BIBL_OK ) goto out;
				newnode->down_node = downnode;
				p = str_cstr( in ) + *offset;
			}
			else {
			  // REprintf("not mathmode!\n");
				if ( depth==0 ) {
					REprintf( "Unmatched '$' character in LaTeX encoding '%s'.\n", str_cstr( in ) );
					p++;
					continue;
				}
				goto out;
			}
		}
		else {
			str_addchar( &(newedge->text), *p );
			p++;
			*offset += 1;
		}
	}

	if ( depth!=0 ) {
		REprintf( "Unmatched '{' character in LaTeX encoding '%s'.\n", str_cstr( in ) );
	}

out:
	if ( status!=BIBL_OK || str_memerr( &(newedge->text) ) ) {
		latex_node_delete( newnode );
		latex_edge_delete( newedge );
		*node = NULL;
		return BIBL_ERR_MEMERR;
	}

	return BIBL_OK;
}

int
build_latex_graph( str *in, latex_node **start )
{
	unsigned long offset = 0;
	int mathmode = 0;
	latex_node *n;

	n = latex_node_new();
	if ( !n ) return BIBL_ERR_MEMERR;
	
	return build_latex_graph_r( in, &offset, &mathmode, 0, start );
}

typedef struct {
	const char *wbracket;
	int wbracketsize;
	const char *wobracket;
	const char *toreplace;
} latex_cmds_t;

static const latex_cmds_t latex_cmds[] = {
	{ "\\it",     3, "\\it ",     NULL },
	{ "\\em",     3, "\\em ",     NULL },
	{ "\\bf",     3, "\\bf ",     NULL },
	{ "\\small",  6, "\\small ",  NULL },
	/* 'textcomp' annotations */
	{ "\\textit", 7, "\\textit ", NULL },
	{ "\\textbf", 7, "\\textbf ", NULL },
	{ "\\textrm", 7, "\\textrm ", NULL },
	{ "\\textsl", 7, "\\textsl ", NULL },
	{ "\\textsc", 7, "\\textsc ", NULL },
	{ "\\textsf", 7, "\\textsf ", NULL },
	{ "\\texttt", 7, "\\texttt ", NULL },
	{ "\\emph",   5, "\\emph ",   NULL },
	{ "\\url",    4, "\\url ",    NULL },
	{ "\\mbox",   5, "\\mbox ",   NULL },
	{ "\\mkbibquote", 11, "\\mkbibquote ", NULL },
	/* math functions */
	{ "\\ln",     3, "\\ln ",     "ln" },
	{ "\\sin",    4, "\\sin ",    "sin" },
	{ "\\cos",    4, "\\cos ",    "cos" },
	{ "\\tan",    4, "\\tan ",    "tan" },
};
static const int nlatex_cmds = sizeof( latex_cmds ) / sizeof( latex_cmds[0] );

static const latex_cmds_t math_cmds[] = {
	{ "\\ln",     3, "\\ln ",     "ln" },
	{ "\\sin",    4, "\\sin ",    "sin" },
	{ "\\cos",    4, "\\cos ",    "cos" },
	{ "\\tan",    4, "\\tan ",    "tan" },
	{ "\\mathrm", 7, "\\mathrm ", ""    },
	{ "\\rm",     3, "\\rm ",     ""    },
	{ "\\LaTeX",  6, "\\LaTeX ",  "LaTeX" },
};
static const int nmath_cmds = sizeof( math_cmds ) / sizeof( math_cmds[0] );

/* remove from "ABC \it{DEF}" --> parses to "ABC \it" */
static int
remove_latex_cmds_with_brackets( str *s )
{
	unsigned long offset;
	int i;
	for ( i=0; i<nlatex_cmds; ++i ) {
		if ( s->len < latex_cmds[i].wbracketsize ) continue;
		offset = s->len - latex_cmds[i].wbracketsize;
		if ( !strcmp( str_cstr( s ) + offset, latex_cmds[i].wbracket ) ) {
			str_trimend( s, latex_cmds[i].wbracketsize );
			return 1;
		}
	}
	return 0;
}

/* remove from "{\it ABC}" */
static void
remove_latex_cmds_without_brackets( str *s )
{
	int i;
	for ( i=0; i<nlatex_cmds; ++i ) {
		str_findreplace( s, latex_cmds[i].wobracket, "" );
	}
}

static void
remove_math_cmds( str *s )
{
	int i;
	for ( i=0; i<nmath_cmds; ++i ) {
		str_findreplace( s, math_cmds[i].wbracket, math_cmds[i].toreplace );
	}
}

static int
collapse_latex_graph( latex_node *n, str *out )
{
	latex_edge *e;
	int status;

	if ( n->down_node ) {
		status = collapse_latex_graph( n->down_node, out );
		if ( status!=BIBL_OK ) return status;
	}

	e = n->next_edge;
	if ( e ) {
		if ( !remove_latex_cmds_with_brackets( &(e->text) ) )
			remove_latex_cmds_without_brackets( &(e->text) );
		remove_math_cmds( &(e->text) );
		str_strcat( out, &(e->text) );
		if ( str_memerr( &(e->text) ) ) return BIBL_ERR_MEMERR;
		if ( e->next_node ) {
			status = collapse_latex_graph( e->next_node, out );
			if ( status!=BIBL_OK ) return status;
		}
	}

	return BIBL_OK;
}

static int
string_from_latex_graph( latex_node *n, str *out )
{
	int status;

	status = collapse_latex_graph( n, out );
	if ( status!=BIBL_OK ) return status;

	while( str_findreplace( out, "  ", " " ) ) {}

	if ( str_memerr( out ) ) return BIBL_ERR_MEMERR;
	else return BIBL_OK;
}

#if 0
static void
write_latex_graph( latex_node *n )
{
	latex_edge *e;

	while ( n ) {

		if ( n->down_node ) {
			Rprintf( "+{" );
			write_latex_graph( n->down_node );
			Rprintf( "}" );
		}
		else Rprintf( "." );

		e = n->next_edge;
		if ( e ) {
			if ( str_has_value( &(e->text) ) ) Rprintf( "%s", str_cstr( &(e->text) ) );
			n = e->next_node;
		}
		else n = NULL;
	}
}
#endif

int
latex_parse( str *in, str *out )
{
	latex_node *n;
	int status;

	str_empty( out );
	if ( str_is_empty( in ) ) return BIBL_OK;

	status = build_latex_graph( in, &n );
	if ( status!=BIBL_OK ) return status;

	status = string_from_latex_graph( n, out );
	if ( status!=BIBL_OK ) return status;

	str_trimendingws( out );

	return BIBL_OK;
}

int
latex_tokenize( slist *tokens, str *s )
{
	int i, n = s->len, nbrackets = 0, status = BIBL_OK;
	str tok, *t;

	str_init( &tok );

	for ( i=0; i<n; ++i ) {
		if ( s->data[i]=='{' && ( i==0 || s->data[i-1]!='\\' ) ) {
			nbrackets++;
			str_addchar( &tok, '{' );
		} else if ( s->data[i]=='}' && ( i==0 || s->data[i-1]!='\\' ) ) {
			nbrackets--;
			str_addchar( &tok, '}' );
		} else if ( !is_ws( s->data[i] ) || nbrackets ) {
			str_addchar( &tok, s->data[i] );
		} else if ( is_ws( s->data[i] ) ) {
			if ( str_has_value( &tok ) ) {
				status = slist_add_ret( tokens, &tok, BIBL_OK, BIBL_ERR_MEMERR );
				if ( status!=BIBL_OK ) goto out;
			}
			str_empty( &tok );
		}
	}
	if ( str_has_value( &tok ) ) {
		if ( str_memerr( &tok ) ) { status = BIBL_ERR_MEMERR; goto out; }
		status = slist_add_ret( tokens, &tok, BIBL_OK, BIBL_ERR_MEMERR );
		if ( status!=BIBL_OK ) goto out;
	}

	for ( i=0; i<tokens->n; ++i ) {
		t = slist_str( tokens, i );
		str_trimstartingws( t );
		str_trimendingws( t );
		if ( str_memerr( t ) ) { status = BIBL_ERR_MEMERR; goto out; }
	}
out:
	str_free( &tok );
	return status;
}
