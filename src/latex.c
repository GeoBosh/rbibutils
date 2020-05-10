/* 
 * latex.c
 *
 * convert between latex special chars and unicode
 *
 * Copyright (c) Chris Putnam 2004-2020
 *
 * Source code released under the GPL version 2
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "latex.h"

#define LATEX_COMBO (0)  /* 'combo' no need for protection on output */
#define LATEX_MACRO (1)  /* 'macro_name' to be protected by {\macro_name} on output */
#define LATEX_MATH  (2)  /* 'math_expression' to be protected by $math_expression$ on output */

#define NUM_VARIANTS (2)

#define ENTRY(a)     { (a), (sizeof(a)/sizeof(char))-1 }
#define EMPTY        { NULL, 0 }

#define COMBO2(a,b,c,d) { (a), LATEX_COMBO, (b), { ENTRY(c), ENTRY(d) } }
#define COMBO1(a,b,c)   { (a), LATEX_COMBO, (b), { ENTRY(c), EMPTY    } }
#define MACRO2(a,b,c,d) { (a), LATEX_MACRO, (b), { ENTRY(c), ENTRY(d) } }
#define MACRO1(a,b,c)   { (a), LATEX_MACRO, (b), { ENTRY(c), EMPTY    } }
#define MATH2(a,b,c,d)  { (a), LATEX_MATH,  (b), { ENTRY(c), ENTRY(d) } }
#define MATH1(a,b,c)    { (a), LATEX_MATH,  (b), { ENTRY(c), EMPTY    } }

struct latex_entry {
	char *entry;
	int  length;
};

struct latex_chars {
	unsigned int unicode;                      /* unicode code point */
	unsigned char type;                        /* LATEX_COMBO/LATEX_MACRO/LATEX_MATH */
	char *out;                                 /* unadorned latex combination for output */
	struct latex_entry variant[NUM_VARIANTS];  /* possible variations on input */
};

static struct latex_chars latex_chars[] = { 

   MACRO1(  192, "`A",                   "\\`A"                                        ), /*  Latin Capital A with grave */
   MACRO1(  224, "`a",                   "\\`a"                                        ), /*  Latin Small a   with grave */
   MACRO1(  193, "'A",                   "\\'A"                                        ), /*  Latin Capital A with acute */
   MACRO1(  225, "'a",                   "\\'a"                                        ), /*  Latin Small a   with acute */
   MACRO1(  194, "^A",                   "\\^A"                                        ), /*  Latin Capital A with circumflex */
   MACRO1(  226, "^a",                   "\\^a"                                        ), /*  Latin Small a   with circumflex */
   MACRO1(  195, "~A",                   "\\~A"                                        ), /*  Latin Capital A with tilde */
   MACRO1(  227, "~a",                   "\\~a"                                        ), /*  Latin Small a   with tilde */
   MACRO1(  196, "\"A",                  "\\\"A"                                       ), /*  Latin Capital A with diuresis */
   MACRO1(  228, "\"a",                  "\\\"a"                                       ), /*  Latin Small a   with diuresis */
   MACRO1(  197, "AA",                   "\\AA"                                        ), /*  Latin Capital A with ring above */
   MACRO2(  197, "AA",                   "\\r A",                "\\rA"                ), /*  Latin Capital A with ring above */
   MACRO1(  229, "aa",                   "\\aa"                                        ), /*  Latin Small a   with ring above */
   MACRO2(  229, "aa",                   "\\r a",                "\\ra"                ), /*  Latin Small a   with ring above */
   MACRO1(  256, "={A}",                 "\\=A"                                        ), /*  Latin Capital A with macron */
   MACRO1(  257, "={a}",                 "\\=a"                                        ), /*  Latin Small a   with macron */
   MACRO2(  258, "u{A}",                 "\\u A",                "\\uA"                ), /*  Latin Capital A with breve */
   MACRO2(  259, "u{a}",                 "\\u a",                "\\ua"                ), /*  Latin Small a   with breve */
   MACRO2(  260, "k{A}",                 "\\k A",                "\\kA"                ), /*  Latin Capital A with ogonek */
   MACRO2(  261, "k{a}",                 "\\k a",                "\\ka"                ), /*  Latin Small a   with ogonek */
   MACRO2(  461, "v{A}",                 "\\v A",                "\\vA"                ), /*  Latin Capital A with caron */
   MACRO2(  462, "v{a}",                 "\\v a",                "\\va"                ), /*  Latin Small a   with caron */

   MACRO1(  198, "AE",                   "\\AE"                                        ), /*  Latin Capital AE Ligature */
   MACRO1(  230, "ae",                   "\\ae"                                        ), /*  Latin Small ae   Ligature */

   MACRO2(  199, "c{C}",                 "\\c C",                "\\cC"                ), /*  Latin Capital C with cedilla */
   MACRO2(  231, "c{c}",                 "\\c c",                "\\cc"                ), /*  Latin Small c   with cedilla */
   MACRO2(  199, "k{C}",                 "\\k C",                "\\kC"                ), /*  Latin Capital C with cedilla (actually ogonek) */
   MACRO2(  231, "k{c}",                 "\\k c",                "\\kc"                ), /*  Latin Small c   with cedilla (actually ogonek) */
   MACRO1(  262, "'{C}",                 "\\'C"                                        ), /*  Latin Capital C with acute */
   MACRO1(  263, "'{c}",                 "\\'c"                                        ), /*  Latin Small c   with acute */
   MACRO1(  264, "^{C}",                 "\\^C"                                        ), /*  Latin Capital C with circumflex */
   MACRO1(  265, "^{c}",                 "\\^c"                                        ), /*  Latin Small c   with circumflex */
   MACRO1(  266, ".{C}",                 "\\.C"                                        ), /*  Latin Capital C with dot above */
   MACRO1(  267, ".{c}",                 "\\.c"                                        ), /*  Latin Small c   with dot above */
   MACRO2(  268, "v{C}",                 "\\v C",                "\\vC"                ), /*  Latin Capital C with caron (hacek) */
   MACRO2(  269, "v{c}",                 "\\v c",                "\\vc"                ), /*  Latin Small c   with caron (hacek) */

   MACRO2(  270, "v{D}",                 "\\v D",                "\\vD"                ), /*  Latin Capital D with caron */
   MACRO2(  271, "v{d}",                 "\\v d",                "\\vd"                ), /*  Latin Small d   with caron */
   MACRO1(  272, "DJ",                   "\\DJ"                                        ), /*  Latin Capital D with stroke */
   MACRO1(  273, "dj",                   "\\dj"                                        ), /*  Latin Small d   with stroke */

   MACRO1(  200, "`E",                   "\\`E"                                        ), /*  Latin Capital E with grave */
   MACRO1(  232, "`e",                   "\\`e"                                        ), /*  Latin Small e   with grave */
   MACRO1(  201, "'E",                   "\\'E"                                        ), /*  Latin Capital E with acute */
   MACRO1(  233, "'e",                   "\\'e"                                        ), /*  Latin Small e   with acute */
   MACRO1(  202, "^E",                   "\\^E"                                        ), /*  Latin Capital E with circumflex */
   MACRO1(  234, "^e",                   "\\^e"                                        ), /*  Latin Small e   with circumflex */
   MACRO1(  203, "\"E",                  "\\\"E"                                       ), /*  Latin Capital E with diuresis */
   MACRO1(  235, "\"e",                  "\\\"e"                                       ), /*  Latin Small e   with diuresis */
   MACRO1(  274, "={E}",                 "\\=E"                                        ), /*  Latin Capital E with macron */
   MACRO1(  275,  "={e}",                "\\=e"                                        ), /*  Latin Small e   with macron */
   MACRO2(  276, "u{E}",                 "\\u E",                "\\uE"                ), /*  Latin Capital E with breve */
   MACRO2(  277, "u{e}",                 "\\u e",                "\\ue"                ), /*  Latin Small e   with breve */
   MACRO1(  278, ".{E}",                 "\\.E"                                        ), /*  Latin Capital E with dot above */
   MACRO1(  279, ".{e}",                 "\\.e"                                        ), /*  Latin Small e   with dot above */
   MACRO2(  280, "k{E}",                 "\\k E",                "\\kE"                ), /*  Latin Capital E with ogonek */
   MACRO2(  281, "k{e}",                 "\\k e",                "\\ke"                ), /*  Latin Small e   with ogonek */
   MACRO2(  282, "v{E}",                 "\\v E",                "\\vE"                ), /*  Latin Capital E with caron */
   MACRO2(  283, "v{e}",                 "\\v e",                "\\ve"                ), /*  Latin Small e   with caron */

   MACRO1(  284, "^{G}",                 "\\^G"                                        ), /*  Latin Capital G with circumflex */
   MACRO1(  285, "^{g}",                 "\\^g"                                        ), /*  Latin Small g   with circumflex */
   MACRO2(  286, "u{G}",                 "\\u G",                "\\uG"                ), /*  Latin Capital G with breve */
   MACRO2(  287, "u{g}",                 "\\u g",                "\\ug"                ), /*  Latin Small g   with breve */
   MACRO1(  288, ".{G}",                 "\\.G"                                        ), /*  Latin Capital G with dot above */
   MACRO1(  289, ".{g}",                 "\\.g"                                        ), /*  Latin Small g   with dot above */
   MACRO2(  290, "c{G}",                 "\\c G",                "\\cG"                ), /*  Latin Capital G with cedilla */
   MACRO2(  291, "c{g}",                 "\\c g",                "\\cg"                ), /*  Latin Small g   with cedilla */
   MACRO2(  486, "v{G}",                 "\\v G",                "\\vG"                ), /*  Latin Capital G with caron */
   MACRO2(  487, "v{g}",                 "\\v g",                "\\vg"                ), /*  Latin Small g   with caron */
   MACRO1(  500, "'{G}",                 "\\'G"                                        ), /*  Latin Capital G with acute */
   MACRO1(  501, "'{g}",                 "\\'g"                                        ), /*  Latin Small g   with acute */

   MACRO1(  292, "^{H}",                 "\\^H"                                        ), /*  Latin Capital H with circumflex */
   MACRO1(  293, "^{h}",                 "\\^h"                                        ), /*  Latin Small h   with circumflex */ 

   MACRO1(  204, "`I",                   "\\`I"                                        ), /*  Latin Capital I with grave */
   MACRO2(  236, "`{\\i}",               "\\`\\i",               "\\`i"                ), /*  Latin Small i   with grave */
   MACRO1(  205, "'I",                   "\\'I"                                        ), /*  Latin Capital I with acute */
   MACRO2(  237, "'{\\i}",               "\\'\\i",               "\\'i"                ), /*  Latin Small i   with acute */
   MACRO1(  206, "^I",                   "\\^I"                                        ), /*  Latin Capital I with circumflex */
   MACRO2(  238, "^{\\i}",               "\\^\\i",               "\\^i"                ), /*  Latin Small i   with circumflex */
   MACRO1(  207, "\"I",                  "\\\"I"                                       ), /*  Latin Capital I with diuresis */
   MACRO2(  239, "\"{\\i}",              "\\\"\\i",              "\\\"i"               ), /*  Latin Small i   with diuresis */
   MACRO1(  296, "~{I}",                 "\\~I"                                        ), /*  Latin Capital I with tilde */
   MACRO2(  297, "`{\\i}",               "\\~\\i",               "\\~i"                ), /*  Latin Small i   with tilde */
   MACRO1(  298, "={I}",                 "\\=I"                                        ), /*  Latin Capital I with macron */
   MACRO2(  299, "={\\i}",               "\\=\\i",               "\\=i"                ), /*  Latin Small i   with macron */
   MACRO2(  300, "u{I}",                 "\\u I",                "\\uI"                ), /*  Latin Capital I with breve */
   MACRO2(  301, "u{\\i}",               "\\u\\i",               "\\ui"                ), /*  Latin Small i   with breve */
   MACRO2(  302, "k{I}",                 "\\k I",                "\\kI"                ), /*  Latin Capital I with ogonek */
   MACRO2(  303, "k{i}",                 "\\k i",                "\\ki"                ), /*  Latin Small i   with ogonek */
   MACRO1(  304, ".{I}",                 "\\.I"                                        ), /*  Latin Capital I with dot above */
   MACRO1(  305, "i",                    "\\i"                                         ), /*  Latin Small i   without dot above */
   MACRO2(  463, "v{I}",                 "\\v I",                "\\vI"                ), /*  Latin Capital I with caron */
   MACRO2(  464, "v{\\i}",               "\\v\\i",               "\\vi"                ), /*  Latin Small i   with caron */

   MACRO1(  308, "^{J}",                 "\\^J"                                        ), /*  Latin Capital J with circumflex */
   MACRO1(  309, "^{j}",                 "\\^j"                                        ), /*  Latin Small j   with circumflex */

   MACRO2(  310, "c{K}",                 "\\c K",                "\\cK"                ), /*  Latin Capital K with cedilla */
   MACRO2(  311, "c{k}",                 "\\c k",                "\\ck"                ), /*  Latin Small k   with cedilla */
   MACRO2(  488, "v{K}",                 "\\v K",                "\\vK"                ), /*  Latin Capital K with caron */
   MACRO2(  489, "v{k}",                 "\\v k",                "\\vk"                ), /*  Latin Small k   with caron */

   MACRO1(  313, "'{L}",                 "\\'L"                                        ), /*  Latin Capital L with acute */
   MACRO1(  314, "'{l}",                 "\\'l"                                        ), /*  Latin Small l   with acute */
   MACRO2(  315, "c{L}",                 "\\c L",                "\\cL"                ), /*  Latin Capital L with cedilla */
   MACRO2(  316, "c{l}",                 "\\c l",                "\\cl"                ), /*  Latin Small l   with cedilla */
   MACRO2(  317, "v{L}",                 "\\v L",                "\\vL"                ), /*  Latin Capital L with caron */
   MACRO2(  318, "v{l}",                 "\\v l",                "\\vl"                ), /*  Latin Small l   with caron */
   MACRO1(  319, "{L\\hspace{-0.35em}$\\cdot$}", "L\\hspace-0.35em\\cdot"              ), /*  Latin Capital L with middle dot */
   MACRO1(  320, "{l$\\cdot$}",          "l\\cdot"                                     ), /*  Latin Small l   with middle dot */
   MACRO1(  321, "L",                    "\\L"                                         ), /*  Latin Capital L with stroke */
   MACRO1(  322, "l",                    "\\l"                                         ), /*  Latin Small l   with stroke */

   MACRO1(  209, "~{N}",                 "\\~N"                                        ), /*  Latin Capital N with tilde */
   MACRO1(  241, "~{n}",                 "\\~n"                                        ), /*  Latin Small n   with tilde */
   MACRO1(  323, "'{N}",                 "\\'N"                                        ), /*  Latin Capital N with acute */
   MACRO1(  324, "'{n}",                 "\\'n"                                        ), /*  Latin Small n   with acute */
   MACRO2(  325, "c{N}",                 "\\c N",                "\\cN"                ), /*  Latin Capital N with cedilla */
   MACRO2(  326, "c{n}",                 "\\c n",                "\\cn"                ), /*  Latin Small n   with cedilla */
   MACRO2(  327, "v{N}",                 "\\v N",                "\\vN"                ), /*  Latin Capital N with caron */
   MACRO2(  328, "v{n}",                 "\\v n",                "\\vn"                ), /*  Latin Small n   with caron */
   MACRO1(  329, "n",                    "\\n"                                         ), /*  Latin Small n preceeded by apostrophe */
 
   MACRO1(  210, "`O",                   "\\`O"                                        ), /*  Latin Capital O with grave */
   MACRO1(  242, "`o",                   "\\`o"                                        ), /*  Latin Small o   with grave */
   MACRO1(  211, "'O",                   "\\'O"                                        ), /*  Latin Capital O with acute */
   MACRO1(  243, "'o",                   "\\'o"                                        ), /*  Latin Small o   with acute */
   MACRO1(  212, "^O",                   "\\^O"                                        ), /*  Latin Capital O with circumflex */
   MACRO1(  244, "^o",                   "\\^o"                                        ), /*  Latin Small o   with circumflex */
   MACRO1(  213, "~O",                   "\\~O"                                        ), /*  Latin Capital O with tilde */
   MACRO1(  245, "~o",                   "\\~o"                                        ), /*  Latin Small o   with tilde */
   MACRO1(  214, "\"O",                  "\\\"O"                                       ), /*  Latin Capital O with diaeresis */
   MACRO1(  246, "\"o",                  "\\\"o"                                       ), /*  Latin Small o   with diaeresis */
   MACRO1(  216, "O",                    "\\O"                                         ), /*  Latin Capital O with stroke */
   MACRO1(  248, "o",                    "\\o"                                         ), /*  Latin Small o   with stroke */
   MACRO1(  332, "={O}",                 "\\=O"                                        ), /*  Latin Capital O with macron */
   MACRO1(  333, "={o}",                 "\\=o"                                        ), /*  Latin Small o   with macron */
   MACRO2(  334, "u{O}",                 "\\u O",                "\\uO"                ), /*  Latin Capital O with breve */
   MACRO2(  335, "u{o}",                 "\\u o",                "\\uo"                ), /*  Latin Small o   with breve */
   MACRO2(  336, "H{O}",                 "\\H O",                "\\HO"                ), /*  Latin Capital O with double acute */
   MACRO2(  337, "H{o}",                 "\\H o",                "\\Ho"                ), /*  Latin Small o   with double acute */
   MACRO2(  465, "v{O}",                 "\\v O",                "\\vO"                ), /*  Latin Capital O with caron */
   MACRO2(  466, "v{o}",                 "\\v o",                "\\vo"                ), /*  Latin Small o   with caron */
   MACRO2(  490, "k{O}",                 "\\k O",                "\\kO"                ), /*  Latin Capital O with ogonek */
   MACRO2(  491, "k{o}",                 "\\k o",                "\\ko"                ), /*  Latin Small o   with ogonek */

   MACRO1(  338, "OE",                   "\\OE"                                        ), /*  Latin Capital OE Ligature */
   MACRO1(  339, "oe",                   "\\oe"                                        ), /*  Latin Small oe   Ligature */

   MACRO1(  340, "'R",                   "\\'R"                                        ), /*  Latin Capital R with acute */ 
   MACRO1(  341, "'r",                   "\\'r"                                        ), /*  Latin Small r   with acute */
   MACRO2(  342, "c{R}",                 "\\c R",                "\\cR"                ), /*  Latin Capital R with cedilla */
   MACRO2(  343, "c{r}",                 "\\c r",                "\\cr"                ), /*  Latin Small r   with cedilla */
   MACRO2(  344, "v{R}",                 "\\v R",                "\\vR"                ), /*  Latin Capital R with caron */
   MACRO2(  345, "v{r}",                 "\\v r",                "\\vr"                ), /*  Latin Small r   with caron */

   MACRO1(  346, "'{S}",                 "\\'S"                                        ), /*  Latin Capital S with acute */
   MACRO1(  347, "'{s}",                 "\\'s"                                        ), /*  Latin Small s   with acute */
   MACRO1(  348, "^{S}",                 "\\^S"                                        ), /*  Latin Capital S with circumflex */
   MACRO1(  349, "^{s}",                 "\\^s"                                        ), /*  Latin Small s   with circumflex */
   MACRO2(  350, "c{S}",                 "\\c S",                "\\cS"                ), /*  Latin Capital S with cedilla */
   MACRO2(  351, "c{s}",                 "\\c s",                "\\cs"                ), /*  Latin Small s   with cedilla */
   MACRO2(  352, "v{S}",                 "\\v S",                "\\vS"                ), /*  Latin Capital S with caron */
   MACRO2(  353, "v{s}",                 "\\v s",                "\\vs"                ), /*  Latin Small s   with caron */

   MACRO1(  223, "ss",                   "\\ss"                                        ), /* German sz Ligature, "sharp s" */

   MACRO2(  354, "c{T}",                 "\\c T",                "\\cT"                ), /*  Latin Capital T with cedilla */
   MACRO2(  355, "c{t}",                 "\\c t",                "\\ct"                ), /*  Latin Small t   with cedilla */
   MACRO2(  356, "v{T}",                 "\\v T",                "\\vT"                ), /*  Latin Capital T with caron */
   MACRO2(  357, "v{t}",                 "\\v t",                "\\vt"                ), /*  Latin Small t   with caron */

   MACRO1(  217, "`U",                   "\\`U"                                        ), /*  Latin Capital U with grave */
   MACRO1(  249, "`u",                   "\\`u"                                        ), /*  Latin Small u   with grave */
   MACRO1(  218, "'U",                   "\\'U"                                        ), /*  Latin Capital U with acute */
   MACRO1(  250, "'u",                   "\\'u"                                        ), /*  Latin Small u   with acute */
   MACRO1(  219, "^U",                   "\\^U"                                        ), /*  Latin Capital U with circumflex */
   MACRO1(  251, "^u",                   "\\^u"                                        ), /*  Latin Small u   with circumflex */
   MACRO1(  220, "\"U",                  "\\\"U"                                       ), /*  Latin Capital U with diaeresis */
   MACRO1(  252, "\"u",                  "\\\"u"                                       ), /*  Latin Small u   with diaeresis */
   MACRO1(  360, "~{U}",                 "\\~U"                                        ), /*  Latin Capital U with tilde */
   MACRO1(  361, "~{u}",                 "\\~u"                                        ), /*  Latin Small u   with tilde */
   MACRO1(  362, "={U}",                 "\\=U"                                        ), /*  Latin Capital U with macron */
   MACRO1(  363, "={u}",                 "\\=u"                                        ), /*  Latin Small u   with macron */
   MACRO2(  364, "u{U}",                 "\\u U",                "\\uU"                ), /*  Latin Capital U with breve */
   MACRO2(  365, "u{u}",                 "\\u u",                "\\uu"                ), /*  Latin Small u   with breve */
   MACRO2(  366, "r{U}",                 "\\r U",                "\\rU"                ), /*  Latin Capital U with ring above */
   MACRO2(  367, "r{u}",                 "\\r u",                "\\ru"                ), /*  Latin Small u   with ring above */
   MACRO2(  368, "H{U}",                 "\\H U",                "\\HU"                ), /*  Latin Capital U with double acute */
   MACRO2(  369, "H{u}",                 "\\H u",                "\\Hu"                ), /*  Latin Small u   with double acute */
   MACRO2(  370, "k{U}",                 "\\k U",                "\\kU"                ), /*  Latin Capital U with ogonek */
   MACRO2(  371, "k{u}",                 "\\k u",                "\\ku"                ), /*  Latin Small u   with ogonek */
   MACRO2(  467, "v{U}",                 "\\v U",                "\\vU"                ), /*  Latin Capital U with caron */
   MACRO2(  468, "v{u}",                 "\\v u",                "\\vu"                ), /*  Latin Small u   with caron */

   MACRO1(  372, "^{W}",                 "\\^W"                                        ), /*  Latin Capital W with circumflex */
   MACRO1(  373, "^{w}",                 "\\^w"                                        ), /*  Latin Small w   with circumflex */

   MACRO1(  221, "'{Y}",                 "\\'Y"                                        ), /*  Latin Capital Y with acute */
   MACRO1(  253, "'y",                   "\\'y"                                        ), /*  Latin Small y   with acute */
   MACRO1(  374, "^{Y}",                 "\\^Y"                                        ), /*  Latin Capital Y with circumflex */
   MACRO1(  375, "^{y}",                 "\\^y"                                        ), /*  Latin Small y   with circumflex */
   MACRO1(  376, "\"{Y}",                "\\\"Y"                                       ), /*  Latin Capital Y with diaeresis */
   MACRO1(  255, "\"y",                  "\\\"y"                                       ), /*  Latin Small y   with diaeresis */

   MACRO1(  377, "'{Z}",                 "\\'Z"                                        ), /*  Latin Capital Z with acute */
   MACRO1(  378, "'{z}",                 "\\'z"                                        ), /*  Latin Small z   with acute */
   MACRO1(  379, ".{Z}",                 "\\.Z"                                        ), /*  Latin Capital Z with dot above */
   MACRO1(  380, ".{z}",                 "\\.z"                                        ), /*  Latin Small z   with dot above */
   MACRO2(  381, "v{Z}",                 "\\v Z",                "\\vZ"                ), /*  Latin Capital Z with caron */
   MACRO2(  382, "v{z}",                 "\\v z",                "\\vz"                ), /*  Latin Small z   with caron */

   MATH1 ( 8203, "\\null",               "\\null"                                      ), /* No space &#x200B; Needs to be before \nu*/

   MATH1 (  913, "\\Alpha",              "\\Alpha"                                     ), /*GREEK CAPITAL LETTER ALPHA*/
   MATH1 (  914, "\\Beta",               "\\Beta"                                      ), /*GREEK CAPITAL LETTER BETA*/
   MATH1 (  915, "\\Gamma",              "\\Gamma"                                     ), /*GREEK CAPITAL LETTER GAMMA*/
   MATH1 (  916, "\\Delta",              "\\Delta"                                     ), /*GREEK CAPITAL LETTER DELTA*/
   MATH1 (  917, "\\Epsilon",            "\\Epsilon"                                   ), /*GREEK CAPITAL LETTER EPSILON*/
   MATH1 (  918, "\\Zeta",               "\\Zeta"                                      ), /*GREEK CAPITAL LETTER ZETA*/
   MATH1 (  919, "\\Eta",                "\\Eta"                                       ), /*GREEK CAPITAL LETTER ETA*/
   MATH1 (  920, "\\Theta",              "\\Theta"                                     ), /*GREEK CAPITAL LETTER THETA*/
   MATH1 (  921, "\\Iota",               "\\Iota"                                      ), /*GREEK CAPITAL LETTER IOTA*/
   MATH1 (  922, "\\Kappa",              "\\Kappa"                                     ), /*GREEK CAPITAL LETTER KAPPA*/
   MATH1 (  923, "\\Lambda",             "\\Lambda"                                    ), /*GREEK CAPITAL LETTER LAMDA*/
   MATH1 (  924, "\\Mu",                 "\\Mu"                                        ), /*GREEK CAPITAL LETTER MU*/
   MATH1 (  925, "\\Nu",                 "\\Nu"                                        ), /*GREEK CAPITAL LETTER NU*/
   MATH1 (  926, "\\Xi",                 "\\Xi"                                        ), /*GREEK CAPITAL LETTER XI*/
   MATH1 (  927, "\\Omicron",            "\\Omicron"                                   ), /*GREEK CAPITAL LETTER OMICRON*/
   MATH1 (  928, "\\Pi",                 "\\Pi"                                        ), /*GREEK CAPITAL LETTER PI*/
   MATH1 (  929, "\\Rho",                "\\Rho"                                       ), /*GREEK CAPITAL LETTER RHO*/
   MATH1 (  931, "\\Sigma",              "\\Sigma"                                     ), /*GREEK CAPITAL LETTER SIGMA*/
   MATH1 (  932, "\\Tau",                "\\Tau"                                       ), /*GREEK CAPITAL LETTER TAU*/
   MATH1 (  933, "\\Upsilon",            "\\Upsilon"                                   ), /*GREEK CAPITAL LETTER UPSILON*/
   MATH1 (  934, "\\Phi",                "\\Phi"                                       ), /*GREEK CAPITAL LETTER PHI*/
   MATH1 (  935, "\\Chi",                "\\Chi"                                       ), /*GREEK CAPITAL LETTER CHI*/
   MATH1 (  936, "\\Psi",                "\\Psi"                                       ), /*GREEK CAPITAL LETTER PSI*/
   MATH1 (  937, "\\Omega",              "\\Omega"                                     ), /*GREEK CAPITAL LETTER OMEGA*/

   MATH1 (  945, "\\alpha",              "\\alpha"                                     ), /*GREEK SMALL LETTER ALPHA*/
   MATH1 (  946, "\\beta",               "\\beta"                                      ), /*GREEK SMALL LETTER BETA*/
   MATH1 (  947, "\\gamma",              "\\gamma"                                     ), /*GREEK SMALL LETTER GAMMA*/
   MATH1 (  948, "\\delta",              "\\delta"                                     ), /*GREEK SMALL LETTER DELTA*/
   MATH1 (  949, "\\epsilon",            "\\epsilon"                                   ), /*GREEK SMALL LETTER EPSILON*/
   MATH1 (  950, "\\zeta",               "\\zeta"                                      ), /*GREEK SMALL LETTER ZETA*/
   MATH1 (  951, "\\eta",                "\\eta"                                       ), /*GREEK SMALL LETTER ETA*/
   MATH1 (  952, "\\theta",              "\\theta"                                     ), /*GREEK SMALL LETTER THETA*/
   MATH1 (  953, "\\iota",               "\\iota"                                      ), /*GREEK SMALL LETTER IOTA*/
   MATH1 (  954, "\\kappa",              "\\kappa"                                     ), /*GREEK SMALL LETTER KAPPA*/
   MATH1 (  955, "\\lambda",             "\\lambda"                                    ), /*GREEK SMALL LETTER LAMDA*/
   MATH1 (  956, "\\mu",                 "\\mu"                                        ), /*GREEK SMALL LETTER MU*/
   MATH1 (  957, "\\nu",                 "\\nu"                                        ), /*GREEK SMALL LETTER NU*/
   MATH1 (  958, "\\xi",                 "\\xi"                                        ), /*GREEK SMALL LETTER XI*/
   MATH1 (  959, "\\omicron",            "\\omicron"                                   ), /*GREEK SMALL LETTER OMICRON*/
   MATH1 (  960, "\\pi",                 "\\pi"                                        ), /*GREEK SMALL LETTER PI*/
   MATH1 (  961, "\\rho",                "\\rho"                                       ), /*GREEK SMALL LETTER RHO*/
   MATH1 (  963, "\\sigma",              "\\sigma"                                     ), /*GREEK SMALL LETTER SIGMA*/
   MATH1 (  964, "\\tau",                "\\tau"                                       ), /*GREEK SMALL LETTER TAU*/
   MATH1 (  965, "\\upsilon",            "\\upsilon"                                   ), /*GREEK SMALL LETTER UPSILON*/
   MATH1 (  966, "\\phi",                "\\phi"                                       ), /*GREEK SMALL LETTER PHI*/
   MATH1 (  967, "\\chi",                "\\chi"                                       ), /*GREEK SMALL LETTER CHI*/
   MATH1 (  968, "\\psi",                "\\psi"                                       ), /*GREEK SMALL LETTER PSI*/
   MATH1 (  969, "\\omega",              "\\omega"                                     ), /*GREEK SMALL LETTER OMEGA*/

   MACRO2(  181, "textmu",               "\\textmu",             "\\mu"                ), /* 181=micro sign, techically &#xB5; */

/* Make sure that these don't stomp on other latex things above */

   COMBO2( 8212, "---",                  "---",                  "\\textemdash"        ), /* Em-dash &#x2014; */
   COMBO2( 8211, "--",                   "--",                   "\\textendash"        ), /* En-dash &#x2013; */
   MACRO2( 8230, "ldots",                "\\ldots",              "\\textellipsis"      ), /* Ellipsis &#x2026; */

   COMBO2( 8220, "``",                   "``",                   "\\textquotedblleft"  ), /* Opening double quote &#x201C; */
   COMBO1( 8221, "''",                   "\""                                          ), /* Closing double quote &#x201D; */
   COMBO2( 8221, "''",                   "''",                   "\\textquotedblright" ), /* Closing double quote &#x201D; */
   COMBO2( 8216, "`",                    "`",                    "\\textquoteleft"     ), /* Opening single quote &#x2018; */
   COMBO2( 8217, "'",                    "'",                    "\\textquoteright"    ), /* Closing single quote &#x2019; */

   MACRO1( 8242, "textasciiacutex",      "\\textasciiacutex"                           ), /* Prime symbol &#x2032; */
   MACRO2(  180, "textasciiacute",       "\\textasciiacute",     "\\'"                 ), /* acute accent &#xB4; */
   MACRO1( 8243, "textacutedbl",         "\\textacutedbl"                              ), /* Double prime &#x2033; */
   MACRO2( 8245, "textasciigrave",       "\\textasciigrave",     "\\`"                 ), /* Grave accent &#x2035; */

   MACRO1( 8963, "textasciicircum",      "\\textasciicircum"                           ), /* &#x2303; */
   MACRO1(  184, "textasciicedilla",     "\\textasciicedilla"                          ), /* cedilla &#xB8; */
   MACRO1(  168, "textasciidieresis",    "\\textasciidieresis"                         ), /* dieresis &#xA8; */
   MACRO1(  175, "textasciimacron",      "\\textasciimacron"                           ), /* macron &#xAF; */

   MACRO1( 8593, "textuparrow",          "\\textuparrow"                               ), /* Up arrow &#x2191; */
   MACRO1( 8595, "textdownarrow",        "\\textdownarrow"                             ), /* Down arrow &#x2193; */
   MACRO1( 8594, "textrightarrow",       "\\textrightarrow"                            ), /* Right arrow &#x2192; */
   MACRO1( 8592, "textleftarrow",        "\\textleftarrow"                             ), /* Left arrow &#x2190; */
   MACRO1(12296, "textlangle",           "\\textlangle"                                ), /* L-angle &#x3008; */
   MACRO1(12297, "textrangle",           "\\textrangle"                                ), /* R-angle &#x3009; */

   MACRO1(  166, "textbrokenbar",        "\\textbrokenbar"                             ), /* Broken vertical bar &#xA6; */
   MACRO2(  167, "textsection",          "\\textsection",        "\\S"                 ), /* Section sign, &#xA7; */
   MACRO2(  170, "textordfeminine",      "\\textordfeminine",    "^a"                  ), /* &#xAA; */
   MACRO1(  172, "textlnot",             "\\textlnot"                                  ), /* Lnot &#xAC; */
   MACRO1(  182, "textparagraph",        "\\textparagraph"                             ), /* Paragraph sign &#xB6; */
   MACRO1(  183, "textperiodcentered",   "\\textperiodcentered"                        ), /* Period-centered &#xB7; */
   MACRO1(  186, "textordmasculine",     "\\textordmasculine"                          ), /* &#xBA; */
   MACRO1( 8214, "textbardbl",           "\\textbardbl"                                ), /* Double vertical bar &#x2016; */
   MACRO1( 8224, "textdagger",           "\\textdagger"                                ), /* Dagger &#x2020; */
   MACRO1( 8225, "textdaggerdbl",        "\\textdaggerdbl"                             ), /* Double dagger &x2021; */
   MACRO1( 8226, "textbullet",           "\\textbullet"                                ), /* Bullet &#x2022; */
   MACRO1( 8494, "textestimated",        "\\textestimated"                             ), /* Estimated &#x212E; */
   MACRO1( 9526, "textopenbullet",       "\\textopenbullet"                            ), /* &#x2536; */

   MACRO1( 8261, "textlquill",           "\\textlquill"                                ), /* Left quill &#x2045; */
   MACRO1( 8262, "textrquill",           "\\textrquill"                                ), /* Right quill &#x2046; */

   MACRO2( 8194, "enspace",              "\\enspace",            "\\hspace.5em"        ), /* En-space &#x2002; */
   MACRO2( 8195, "emspace",              "\\emspace",            "\\hspace1em"         ), /* Em-space &#x2003; */
   MACRO1( 8201, "thinspace",            "\\thinspace"                                 ), /* Thin space &#x2009; */
   MACRO1( 8203, "textnospace",          "\\textnospace"                               ), /* No space &#x200B; */
   MACRO1( 9251, "textvisiblespace",     "\\textvisiblespace"                          ), /* Visible space &#x2423; */

   MACRO1(  215, "texttimes",            "\\texttimes"                                 ), /* Multiplication symbol &#xD7; */
   MACRO1(  247, "textdiv",              "\\textdiv"                                   ), /* Division symbol &#xF7; */
   MACRO1(  177, "textpm",               "\\textpm"                                    ), /* Plus-minus character &#B1; */
   MACRO1(  188, "textonequarter",       "\\textonequarter"                            ), /* Vulgar fraction one quarter &#xBC; */
   MACRO1(  189, "textonehalf",          "\\textonehalf"                               ), /* Vulgar fraction one half &#xBD; */
   MACRO1(  190, "textthreequarters",    "\\textthreequarters"                         ), /* Vulgar fraction three quarters &#xBE; */
   MACRO1( 8240, "texttenthousand",      "\\texttenthousand"                           ), /* Per thousand sign &#x2030; */
   MACRO1( 8241, "textpertenthousand",   "\\textpertenthousand"                        ), /* Per ten thousand sign &#x2031;*/
   MACRO1( 8260, "textfractionssolidus", "\\textfractionsolidus"                       ), /* &x8260 */
   MACRO1( 8451, "textcelcius",          "\\textcelcius"                               ), /* Celcius &#x2103; */
   MACRO1( 8470, "textnumero",           "\\textnumero"                                ), /* Numero symbol &#x2116; */
   MACRO1( 8486, "textohm",              "\\textohm"                                   ), /* Ohm symbol &#x2126; */
   MACRO1( 8487, "textmho",              "\\textmho"                                   ), /* Mho symbol &#x2127; */
   MACRO1( 8730, "textsurd",             "\\textsurd"                                  ), /* &#x221A; */

   MACRO2(  185, "textonesuperior",      "\\textonesuperior",    "^1"                  ), /*Superscript 1 &#xB9; */
   MACRO2(  178, "texttwosuperior",      "\\texttwosuperior",    "^2"                  ), /*Superscript 2 &#xB2; */
   MACRO2(  179, "textthreesuperior",    "\\textthreesuperior",  "^3"                  ), /*Superscript 3 &#xB3; */

   MATH1 ( 8308, "^4",                   "^4"                                          ), /*Superscript 4 U+2074*/
   MATH1 ( 8309, "^5",                   "^5"                                          ), /*Superscript 5 U+2075*/
   MATH1 ( 8310, "^6",                   "^6"                                          ), /*Superscript 6 U+2076*/
   MATH1 ( 8311, "^7",                   "^7"                                          ), /*Superscript 7 U+2077*/
   MATH1 ( 8312, "^8",                   "^8"                                          ), /*Superscript 8 U+2078*/
   MATH1 ( 8313, "^9",                   "^9"                                          ), /*Superscript 9 U+2079*/

   MATH1 ( 8314, "^+",                   "^+"                                          ), /*Superscript - U+207A*/
   MATH1 ( 8315, "^-",                   "^-"                                          ), /*Superscript - U+207B*/
   MATH1 ( 8316, "^=",                   "^="                                          ), /*Superscript = U+207C*/
   MATH1 ( 8317, "^(",                   "^("                                          ), /*Superscript ) U+207D*/
   MATH1 ( 8318, "^)",                   "^)"                                          ), /*Superscript ) U+207E*/
   MATH1 ( 8319, "^n",                   "^n"                                          ), /*Superscript n U+207F*/

   MATH1 ( 8320, "_0",                   "_0"                                          ), /*Subscript 0 U+2080*/
   MATH1 ( 8321, "_1",                   "_1"                                          ), /*Subscript 1 U+2081*/
   MATH1 ( 8322, "_2",                   "_2"                                          ), /*Subscript 2 U+2082*/
   MATH1 ( 8323, "_3",                   "_3"                                          ), /*Subscript 3 U+2083*/
   MATH1 ( 8324, "_4",                   "_4"                                          ), /*Subscript 4 U+2084*/
   MATH1 ( 8325, "_5",                   "_5"                                          ), /*Subscript 5 U+2085*/
   MATH1 ( 8326, "_6",                   "_6"                                          ), /*Subscript 6 U+2086*/
   MATH1 ( 8327, "_7",                   "_7"                                          ), /*Subscript 7 U+2087*/
   MATH1 ( 8328, "_8",                   "_8"                                          ), /*Subscript 8 U+2088*/
   MATH1 ( 8329, "_9",                   "_9"                                          ), /*Subscript 9 U+2089*/

   MATH2(   183, "\\cdot",               "\\cdot",               "^."                  ),
   MATH1(  2192, "\\to",                 "\\to"                                        ),

   MACRO1(  161, "textexclamdown",      "\\textexclamdown"                             ), /* Inverted exclamation mark &#xA1;*/
   MACRO1(  191, "textquestiondown",    "\\textquestiondown"                           ), /* Inverted question mark &#xBF; */

   MACRO1(  162, "textcent",            "\\textcent"                                   ), /* Cent sign &#xA2; */
   MACRO2(  163, "textsterling",        "\\textsterling",        "\\pounds"            ), /* Pound sign &#xA3; */
   MACRO1(  165, "textyen",             "\\textyen"                                    ), /* Yen sign &#xA5; */
   MACRO1(  402, "textflorin",          "\\textflorin"                                 ), /* Florin sign &#x192; */
   MACRO1( 3647, "textbaht",            "\\textbaht"                                   ), /* Thai currency &#xE3F; */
   MACRO1( 8355, "textfrenchfranc",     "\\textfrenchfranc"                            ), /* French franc &#x20A3; */
   MACRO1( 8356, "textlira",            "\\textlira"                                   ), /* Lira &#x20A4; */
   MACRO1( 8358, "textnaira",           "\\textnaira"                                  ), /* Naira &#x20A6; */
   MACRO1( 8361, "textwon",             "\\textwon"                                    ), /* &#x20A9; */
   MACRO1( 8363, "textdong",            "\\textdong"                                   ), /* Vietnamese currency &#x20AB; */
   MACRO1( 8364, "texteuro",            "\\texteuro"                                   ), /* Euro sign */

   MACRO1(  169, "textcopyright",       "\\textcopyright"                              ), /* Copyright (C) &#xA9; */
   MACRO1(  175, "textregistered",      "\\textregistered"                             ), /* Registered sign (R) &#xAF;*/
   MACRO2( 8482, "texttrademark",       "\\texttrademark",       "^TM"                 ), /* Trademark (TM) &#x2122; */
   MACRO2( 8480, "textservicemark",     "\\textservicemark",     "^SM"                 ), /* Servicemark (SM) &#x2120;*/
   MACRO1( 8471, "textcircledP",        "\\textcircledP"                               ), /* Circled P &#2117; */

   /* keep it after all other things like \~n */
   MACRO1(   35, "#",                    "\\#"                                         ), /* Number/pound/hash sign */
   MACRO1(   36, "$",                    "\\$"                                         ), /* Dollar Sign */
   MACRO2(   36, "$",                    "\\$",                  "\\textdollar"        ),
   MACRO1(   37, "%",                    "\\%"                                         ), /* Percent Sign */
   MACRO1(   37, "%",                    "\\%"                                         ),
   MACRO1(   38, "&",                    "\\&"                                         ), /* Ampersand */
   MACRO1(   95, "_",                    "\\_"                                         ), /* Underscore alone indicates subscript */
   MACRO1(   95, "_",                    "\\textunderscore"                            ),
   MACRO2(  123, "{",                    "\\{",                  "\\textbraceleft"     ), /* Left Curly Bracket */
   MACRO2(  125, "}",                    "\\}",                  "\\textbraceright"    ), /* Right Curly Bracket */
   MACRO2(  126, "~",                    "\\~",                  "\\textasciitilde"    ),
   MACRO1(   92, "backslash",            "\\backslash"                                 ), /* Backslash */
   MACRO2(  176, "textdegree",           "\\textdegree",         "^\\circ"             ), /* Degree sign */



};

static int nlatex_chars = sizeof(latex_chars)/sizeof(latex_chars[0]);


static struct latex_chars only_from_latex[] = {
   COMBO1(  32, " ", "~" ),
   MACRO1(  32, " ", "\\ " ), /* escaping the space is used to avoid extra space after periods */
};

static int num_only_from_latex = sizeof( only_from_latex ) / sizeof( only_from_latex[0] );


/* latex2char()
 *
 *   Use the latex_chars[] lookup table to determine if any character
 *   is a special LaTeX code.  Note that if it is, then the equivalency
 *   is a Unicode character and we need to flag (by setting *unicode to 1)
 *   that we know the output is Unicode.  Otherwise, we set *unicode to 0,
 *   meaning that the output is whatever character set was given to us
 *   (which could be Unicode, but is not necessarily Unicode).
 *
 */
static unsigned int
lookup_latex( struct latex_chars *lc, int n, char *p, unsigned int *pos, int *unicode )
{
	struct latex_entry *variant;
	int i, j;

	for ( i=0; i<n; ++i ) {
		for ( j=0; j<NUM_VARIANTS; ++j ) {
			variant = &(lc[i].variant[j] );
			if ( variant->entry == NULL ) break;
			if ( !strncmp( p, variant->entry, variant->length ) ) {
				*pos = *pos + variant->length;
				*unicode = 1;
				return lc[i].unicode;
			}
		}
	}

	return 0;
}

unsigned int
latex2char( char *s, unsigned int *pos, int *unicode )
{
	unsigned int value, result;
	char *p;

	p = &( s[*pos] );
	value = (unsigned char) *p;

	if ( strchr( "\\\'\"`-^_lL", value ) ) {
		result = lookup_latex( latex_chars, nlatex_chars, p, pos, unicode );
		if ( result!=0 ) return result;
	}

	if ( value=='~' || value=='\\' ) {
		result = lookup_latex( only_from_latex, num_only_from_latex, p, pos, unicode );
		if ( result!=0 ) return result;
	}

	*unicode = 0;
	*pos = *pos + 1;
	return value;
}

void
uni2latex( unsigned int ch, char buf[], int buf_size )
{
	int i, j, n;

	if ( buf_size==0 ) return;

	buf[0] = '?';
	buf[1] = '\0';

	if ( ch==' ' ) {
		buf[0] = ' '; /*special case to avoid &nbsp;*/
		return;
	}

	for ( i=0; i<nlatex_chars; ++i ) {
		if ( ch == latex_chars[i].unicode ) {
			n = 0;

			if ( latex_chars[i].type == LATEX_MACRO ) {
				if ( n < buf_size ) buf[n++] = '{';
				if ( n < buf_size ) buf[n++] = '\\';
			}
			else if ( latex_chars[i].type == LATEX_MATH ) {
				if ( n < buf_size ) buf[n++] = '$';
			}

			j = 0;
			while ( latex_chars[i].out[j] ) {
				if ( n < buf_size ) buf[n++] = latex_chars[i].out[j];
				j++;
			}

			if ( latex_chars[i].type == LATEX_MACRO ) {
				if ( n < buf_size ) buf[n++] = '}';
			}
			else if ( latex_chars[i].type == LATEX_MATH ) {
				if ( n < buf_size ) buf[n++] = '$';
			}

			if ( n < buf_size ) buf[n] = '\0';
			else buf[ buf_size-1 ] = '\0';

			return;
		}
	}

	if ( ch < 128 ) buf[0] = (char)ch;
}
