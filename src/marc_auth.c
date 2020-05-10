/*
 * marc_auth.c
 *
 * MARC (MAchine-Readable Cataloging) 21 authority codes/values from the Library of Congress initiative
 *
 * Copyright (c) Chris Putnam 2004-2020
 *
 * Source code released under the GPL version 2
 *
 */
#include "marc_auth.h"
#include <string.h>

static const char *marc_genre[] = {
	"abstract or summary",
	"art original",
	"art reproduction",
	"article",
	"atlas",
	"autobiography",
	"bibliography",
	"biography",
	"book",
	"calendar",
	"catalog",
	"chart",
	"comic or graphic novel",
	"comic strip",
	"conference publication",
	"database",
	"dictionary",
	"diorama",
	"directory",
	"discography",
	"drama",
	"encyclopedia",
	"essay",
	"festschrift",
	"fiction",
	"filmography",
	"filmstrip",
	"finding aid",
	"flash card",
	"folktale",
	"font",
	"game",
	"government publication",
	"graphic",
	"globe",
	"handbook",
	"history",
	"humor, satire",
	"hymnal",
	"index",
	"instruction",
	"interview",
	"issue",
	"journal",
	"kit",
	"language instruction",
	"law report or digest",
	"legal article",
	"legal case and case notes",
	"legislation",
	"letter",
	"loose-leaf",
	"map",
	"memoir",
	"microscope slide",
	"model",
	"motion picture",
	"multivolume monograph",
	"newspaper",
	"novel",
	"numeric data",
	"offprint",
	"online system or service",
	"patent",
	"periodical",
	"picture",
	"poetry",
	"programmed text",
	"realia",
	"rehearsal",
	"remote sensing image",
	"reporting",
	"review",
	"series",
	"short story",
	"slide",
	"sound",
	"speech",
	"standard or specification",
	"statistics",
	"survey of literature",
	"technical drawing",
	"technical report",
	"thesis",
	"toy",
	"transparency",
	"treaty",
	"videorecording",
	"web site",
	"yearbook",
};
static const int nmarc_genre = sizeof( marc_genre ) / sizeof( const char* );

static const char *marc_resource[] = {
	"cartographic",
	"kit",
	"mixed material",
	"moving image",
	"notated music",
	"software, multimedia",
	"sound recording",
	"sound recording - musical",
	"sound recording - nonmusical",
	"still image",
	"text",
	"three dimensional object"
};
static const int nmarc_resource = sizeof( marc_resource ) / sizeof( const char* );


/* www.loc.gov/marc/relators/relacode.html */

typedef struct marc_trans {
	char *internal_name;
	char *abbreviation;
} marc_trans;

static const marc_trans relators[] = {
	{ "ABRIDGER",                          "abr"                                 },
	{ "ART_COPYIST",                       "acp"                                 },
	{ "ACTOR",                             "act"                                 },
	{ "ART_DIRECTOR",                      "adi"                                 },
	{ "ADAPTER",                           "adp"                                 },
	{ "AFTERAUTHOR",                       "aft"                                 },
	{ "ANALYST",                           "anl"                                 },
	{ "ANIMATOR",                          "anm"                                 },
	{ "ANNOTATOR",                         "ann"                                 },
	{ "BIBLIOGRAPHIC_ANTECEDENT",          "ant"                                 },
	{ "APPELLEE",                          "ape"                                 },
	{ "APPELLANT",                         "apl"                                 },
	{ "APPLICANT",                         "app"                                 },
	{ "AUTHOR",                            "aqt"                                 },/* Author in quotations or text abstracts */
	{ "ARCHITECT",                         "arc"                                 },
	{ "ARTISTIC_DIRECTOR",                 "ard"                                 },
	{ "ARRANGER",                          "arr"                                 },
	{ "ARTIST",                            "art"                                 },
	{ "ASSIGNEE",                          "asg"                                 },
	{ "ASSOCIATED_NAME",                   "asn"                                 },
	{ "AUTOGRAPHER",                       "ato"                                 },
	{ "ATTRIBUTED_NAME",                   "att"                                 },
	{ "AUCTIONEER",                        "auc"                                 },
	{ "AUTHOR",                            "aud"                                 },/* Author of dialog */
	{ "INTROAUTHOR",                       "aui"                                 },/* Author of introduction, etc. */
	{ "AUTHOR",                            "aus"                                 },/* Screenwriter */
	{ "AUTHOR",                            "aut"                                 },
	{ "AUTHOR",                            "author"                              },
	{ "AFTERAUTHOR",                       "author of afterword, colophon, etc." },
	{ "INTROAUTHOR",                       "author of introduction, etc."        },
	{ "BINDING_DESIGNER",                  "bdd"                                 },
	{ "BOOKJACKET_DESIGNER",               "bjd"                                 },
	{ "BOOK_DESIGNER",                     "bkd"                                 },
	{ "BOOK_PRODUCER",                     "bkp"                                 },
	{ "AUTHOR",                            "blw"                                 },/* Blurb writer */
	{ "BINDER",                            "bnd"                                 },
	{ "BOOKPLATE_DESIGNER",                "bpd"                                 },
	{ "BROADCASTER",                       "brd"                                 },
	{ "BRAILLE_EMBOSSER",                  "brl"                                 },
	{ "BOOKSELLER",                        "bsl"                                 },
	{ "CASTER",                            "cas"                                 },
	{ "CONCEPTOR",                         "ccp"                                 },
	{ "CHOREOGRAPHER",                     "chr"                                 },
	{ "COLLABORATOR",                      "clb"                                 },
	{ "CLIENT",                            "cli"                                 },
	{ "CALLIGRAPHER",                      "cll"                                 },
	{ "COLORIST",                          "clr"                                 },
	{ "COLLOTYPER",                        "clt"                                 },
	{ "COMMENTATOR",                       "cmm"                                 },
	{ "COMPOSER",                          "cmp"                                 },
	{ "COMPOSITOR",                        "cmt"                                 },
	{ "CONDUCTOR",                         "cnd"                                 },
	{ "CINEMATOGRAPHER",                   "cng"                                 },
	{ "CENSOR",                            "cns"                                 },
	{ "CONTESTANT-APPELLEE",               "coe"                                 },
	{ "COLLECTOR",                         "col"                                 },
	{ "COMPILER",                          "com"                                 },
	{ "CONSERVATOR",                       "con"                                 },
	{ "COLLECTION_REGISTRAR",              "cor"                                 },
	{ "CONTESTANT",                        "cos"                                 },
	{ "CONTESTANT-APPELLANT",              "cot"                                 },
	{ "COURT_GOVERNED",                    "cou"                                 },
	{ "COVER_DESIGNER",                    "cov"                                 },
	{ "COPYRIGHT_CLAIMANT",                "cpc"                                 },
	{ "COMPLAINANT-APPELLEE",              "cpe"                                 },
	{ "COPYRIGHT_HOLDER",                  "cph"                                 },
	{ "COMPLAINANT",                       "cpl"                                 },
	{ "COMPLAINANT-APPELLANT",             "cpt"                                 },
	{ "AUTHOR",                            "cre"                                 },/* Creator */
	{ "AUTHOR",                            "creator"                             },/* Creator */
	{ "CORRESPONDENT",                     "crp"                                 },
	{ "CORRECTOR",                         "crr"                                 },
	{ "COURT_REPORTER",                    "crt"                                 },
	{ "CONSULTANT",                        "csl"                                 },
	{ "CONSULTANT",                        "csp"                                 },/* Consultant to a project */
	{ "COSTUME_DESIGNER",                  "cst"                                 },
	{ "CONTRIBUTOR",                       "ctb"                                 },
	{ "CONTESTEE-APPELLEE",                "cte"                                 },
	{ "CARTOGRAPHER",                      "ctg"                                 },
	{ "CONTRACTOR",                        "ctr"                                 },
	{ "CONTESTEE",                         "cts"                                 },
	{ "CONTESTEE-APPELLANT",               "ctt"                                 },
	{ "CURATOR",                           "cur"                                 },
	{ "COMMENTATOR",                       "cwt"                                 },/* Commentator for written text */
	{ "DISTRIBUTION_PLACE",                "dbp"                                 },
	{ "DEGREEGRANTOR",                     "degree grantor"                      },/* Degree granting institution */
	{ "DEFENDANT",                         "dfd"                                 },
	{ "DEFENDANT-APPELLEE",                "dfe"                                 },
	{ "DEFENDANT-APPELLANT",               "dft"                                 },
	{ "DEGREEGRANTOR",                     "dgg"                                 },/* Degree granting institution */
	{ "DEGREE_SUPERVISOR",                 "dgs"                                 },
	{ "DISSERTANT",                        "dis"                                 },
	{ "DELINEATOR",                        "dln"                                 },
	{ "DANCER",                            "dnc"                                 },
	{ "DONOR",                             "dnr"                                 },
	{ "DEPICTED",                          "dpc"                                 },
	{ "DEPOSITOR",                         "dpt"                                 },
	{ "DRAFTSMAN",                         "drm"                                 },
	{ "DIRECTOR",                          "drt"                                 },
	{ "DESIGNER",                          "dsr"                                 },
	{ "DISTRIBUTOR",                       "dst"                                 },
	{ "DATA_CONTRIBUTOR",                  "dtc"                                 },
	{ "DEDICATEE",                         "dte"                                 },
	{ "DATA_MANAGER",                      "dtm"                                 },
	{ "DEDICATOR",                         "dto"                                 },
	{ "AUTHOR",                            "dub"                                 },/* Dubious author */
	{ "EDITOR",                            "edc"                                 },/* Editor of compilation */
	{ "EDITOR",                            "edm"                                 },/* Editor of moving image work */
	{ "EDITOR",                            "edt"                                 },
	{ "ENGRAVER",                          "egr"                                 },
	{ "ELECTRICIAN",                       "elg"                                 },
	{ "ELECTROTYPER",                      "elt"                                 },
	{ "ENGINEER",                          "eng"                                 },
	{ "ENACTING_JURISDICTION",             "enj"                                 },
	{ "ETCHER",                            "etr"                                 },
	{ "EVENT_PLACE",                       "evp"                                 },
	{ "EXPERT",                            "exp"                                 },
	{ "FACSIMILIST",                       "fac"                                 },
	{ "FILM_DISTRIBUTOR",                  "fds"                                 },
	{ "FIELD_DIRECTOR",                    "fld"                                 },
	{ "EDITOR",                            "flm"                                 },/* Film editor */
	{ "DIRECTOR",                          "fmd"                                 },/* Film director */
	{ "FILMMAKER",                         "fmk"                                 },
	{ "FORMER_OWNER",                      "fmo"                                 },
	{ "PRODUCER",                          "fmp"                                 },/* Film producer */
	{ "FUNDER",                            "fnd"                                 },
	{ "FIRST_PARTY",                       "fpy"                                 },
	{ "FORGER",                            "frg"                                 },
	{ "GEOGRAPHIC_INFORMATION_SPECIALIST", "gis"                                 },
	{ "GRAPHIC_TECHNICIAN",                "grt"                                 },
	{ "HOST_INSTITUTION",                  "his"                                 },
	{ "HONOREE",                           "hnr"                                 },
	{ "HOST",                              "hst"                                 },
	{ "ILLUSTRATOR",                       "ill"                                 },
	{ "ILLUMINATOR",                       "ilu"                                 },
	{ "INSCRIBER",                         "ins"                                 },
	{ "INVENTOR",                          "inv"                                 },
	{ "ISSUING_BODY",                      "isb"                                 },
	{ "MUSICIAN",                          "itr"                                 },/* Instrumentalist */
	{ "INTERVIEWEE",                       "ive"                                 },
	{ "INTERVIEWER",                       "ivr"                                 },
	{ "JUDGE",                             "jud"                                 },
	{ "JURISDICTION_GOVERNED",             "jug"                                 },
	{ "LABORATORY",                        "lbr"                                 },
	{ "AUTHOR",                            "lbt"                                 },/* Librettist */
	{ "LABORATORY_DIRECTOR",               "ldr"                                 },
	{ "LEAD",                              "led"                                 },
	{ "LIBELEE-APPELLEE",                  "lee"                                 },
	{ "LIBELEE",                           "lel"                                 },
	{ "LENDER",                            "len"                                 },
	{ "LIBELEE-APPELLANT",                 "let"                                 },
	{ "LIGHTING_DESIGNER",                 "lgd"                                 },
	{ "LIBELANT-APPELLEE",                 "lie"                                 },
	{ "LIBELANT",                          "lil"                                 },
	{ "LIBELANT-APPELLANT",                "lit"                                 },
	{ "LANDSCAPE_ARCHITECT",               "lsa"                                 },
	{ "LICENSEE",                          "lse"                                 },
	{ "LICENSOR",                          "lso"                                 },
	{ "LITHOGRAPHER",                      "ltg"                                 },
	{ "AUTHOR",                            "lyr"                                 },/* Lyricist */
	{ "MUSIC_COPYIST",                     "mcp"                                 },
	{ "METADATA_CONTACT",                  "mdc"                                 },
	{ "MEDIUM",                            "med"                                 },
	{ "MANUFACTURE_PLACE",                 "mfp"                                 },
	{ "MANUFACTURER",                      "mfr"                                 },
	{ "MODERATOR",                         "mod"                                 },
	{ "THESIS_EXAMINER",                   "mon"                                 },/* Monitor */
	{ "MARBLER",                           "mrb"                                 },
	{ "EDITOR",                            "mrk"                                 },/* Markup editor */
	{ "MUSICAL_DIRECTOR",                  "msd"                                 },
	{ "METAL-ENGRAVER",                    "mte"                                 },
	{ "MINUTE_TAKER",                      "mtk"                                 },
	{ "MUSICIAN",                          "mus"                                 },
	{ "NARRATOR",                          "nrt"                                 },
	{ "THESIS_OPPONENT",                   "opn"                                 },/* Opponent */
	{ "ORIGINATOR",                        "org"                                 },
	{ "ORGANIZER",                         "organizer of meeting"                },
	{ "ORGANIZER",                         "orm" 	                             },
	{ "ONSCREEN_PRESENTER",                "osp" 	                             },
	{ "THESIS_OTHER",                      "oth" 	                             },/* Other */
	{ "OWNER",                             "own" 	                             },
	{ "PANELIST",                          "pan" 	                             },
	{ "PATRON",                            "pat" 	                             },
	{ "ASSIGNEE",                          "patent holder"                       },/* Patent holder */
	{ "PUBLISHING_DIRECTOR",               "pbd" 	                             },
	{ "PUBLISHER",                         "pbl"                                 },
	{ "PROJECT_DIRECTOR",                  "pdr"                                 },
	{ "PROOFREADER",                       "pfr"                                 },
	{ "PHOTOGRAPHER",                      "pht"                                 },
	{ "PLATEMAKER",                        "plt"                                 },
	{ "PERMITTING_AGENCY",                 "pma"                                 },
	{ "PRODUCTION_MANAGER",                "pmn"                                 },
	{ "PRINTER_OF_PLATES",                 "pop"                                 },
	{ "PAPERMAKER",                        "ppm"                                 },
	{ "PUPPETEER",                         "ppt"                                 },
	{ "PRAESES",                           "pra"                                 },
	{ "PROCESS_CONTRACT",                  "prc"                                 },
	{ "PRODUCTION_PERSONNEL",              "prd"                                 },
	{ "PRESENTER",                         "pre"                                 },
	{ "PERFORMER",                         "prf"                                 },
	{ "AUTHOR",                            "prg"                                 },/* Programmer */
	{ "PRINTMAKER",                        "prm"                                 },
	{ "PRODUCTION_COMPANY",                "prn"                                 },
	{ "PRODUCER",                          "pro"                                 },
	{ "PRODUCTION_PLACE",                  "prp"                                 },
	{ "PRODUCTION_DESIGNER",               "prs"                                 },
	{ "PRINTER",                           "prt"                                 },
	{ "PROVIDER",                          "prv"                                 },
	{ "PATENT_APPLICANT",                  "pta"                                 },
	{ "PLAINTIFF-APPELLEE",                "pte"                                 },
	{ "PLAINTIFF",                         "ptf"                                 },
	{ "ASSIGNEE",                          "pth"                                 },/* Patent holder */
	{ "PLAINTIFF-APPELLANT",               "ptt"                                 },
	{ "PUBLICATION_PLACE",                 "pup"                                 },
	{ "RUBRICATOR",                        "rbr"                                 },
	{ "RECORDIST",                         "rcd"                                 },
	{ "RECORDING_ENGINEER",                "rce"                                 },
	{ "ADDRESSEE",                         "rcp"                                 },/* Recipient */
	{ "RADIO_DIRECTOR",                    "rdd"                                 },
	{ "REDAKTOR",                          "red"                                 },
	{ "RENDERER",                          "ren"                                 },
	{ "RESEARCHER",                        "res"                                 },
	{ "REVIEWER",                          "rev"                                 },
	{ "RADIO_PRODUCER",                    "rpc"                                 },
	{ "REPOSITORY",                        "rps"                                 },
	{ "REPORTER",                          "rpt"                                 },
	{ "RESPONSIBLE_PARTY",                 "rpy"                                 },
	{ "RESPONDENT-APPELLEE",               "rse"                                 },
	{ "RESTAGER",                          "rsg"                                 },
	{ "RESPONDENT",                        "rsp"                                 },
	{ "RESTORATIONIST",                    "rsr"                                 },
	{ "RESPONDENT-APPELLANT",              "rst"                                 },
	{ "RESEARCH_TEAM_HEAD",                "rth"                                 },
	{ "RESEARCH_TEAM_MEMBER",              "rtm"                                 },
	{ "SCIENTIFIC_ADVISOR",                "sad"                                 },
	{ "SCENARIST",                         "sce"                                 },
	{ "SCULPTOR",                          "scl"                                 },
	{ "SCRIBE",                            "scr"                                 },
	{ "SOUND_DESIGNER",                    "sds"                                 },
	{ "SECRETARY",                         "sec"                                 },
	{ "STAGE_DIRECTOR",                    "sgd"                                 },
	{ "SIGNER",                            "sgn"                                 },
	{ "SUPPORTING_HOST",                   "sht"                                 },
	{ "SELLER",                            "sll"                                 },
	{ "SINGER",                            "sng"                                 },
	{ "SPEAKER",                           "spk"                                 },
	{ "SPONSOR",                           "spn"                                 },
	{ "SECOND_PARTY",                      "spy"                                 },
	{ "SURVEYOR",                          "srv"                                 },
	{ "SET_DESIGNER",                      "std"                                 },
	{ "SETTING",                           "stg"                                 },
	{ "STORYTELLER",                       "stl"                                 },
	{ "STAGE_MANAGER",                     "stm"                                 },
	{ "STANDARDS_BODY",                    "stn"                                 },
	{ "STEREOTYPER",                       "str"                                 },
	{ "TECHNICAL_DIRECTOR",                "tcd"                                 },
	{ "TEACHER",                           "tch"                                 },
	{ "THESIS_ADVISOR",                    "ths"                                 },
	{ "TELEVISION_DIRECTOR",               "tld"                                 },
	{ "TELEVISION_PRODUCER",               "tlp"                                 },
	{ "TRANSCRIBER",                       "trc"                                 },
	{ "TRANSLATOR",                        "translator"                          },
	{ "TRANSLATOR",                        "trl"                                 },
	{ "TYPE_DIRECTOR",                     "tyd"                                 },
	{ "TYPOGRAPHER",                       "tyg"                                 },
	{ "UNIVERSITY_PLACE",                  "uvp"                                 },
	{ "VOICE_ACTOR",                       "vac"                                 },
	{ "VIDEOGRAPHER",                      "vdg"                                 },
	{ "VOCALIST",                          "voc"                                 },
	{ "AUTHOR",                            "wac"                                 },/* Writer of added commentary */
	{ "AUTHOR",                            "wal"                                 },/* Writer of added lyrics */
	{ "AUTHOR",                            "wam"                                 },/* Writer of accompanying material */
	{ "AUTHOR",                            "wat"                                 },/* Writer of added text */
	{ "WOODCUTTER",                        "wdc"                                 },
	{ "WOOD_ENGRAVER",                     "wde"                                 },
	{ "INTROAUTHOR",                       "win"                                 },/* Writer of introduction */
	{ "WITNESS",                           "wit"                                 },
	{ "INTROAUTHOR",                       "wpr"                                 },/* Writer of preface */
	{ "AUTHOR",                            "wst"                                 },/* Writer of supplementary textual content */
};

static const int nrealtors = sizeof( relators ) / sizeof( relators[0] );

char *
marc_convert_role( const char *query )
{
	int i;

	for ( i=0; i<nrealtors; ++i ) {
		if ( !strcasecmp( query, relators[i].abbreviation ) )
			return relators[i].internal_name;
	}
	return NULL;
}

static int
position_in_list( const char *list[], int nlist, const char *query )
{
	int i;
	for ( i=0; i<nlist; ++i ) {
		if ( !strcasecmp( query, list[i] ) ) return i;
	}
	return -1;
}

int
marc_find_genre( const char *query )
{
	return position_in_list( marc_genre, nmarc_genre, query );
}

int
marc_find_resource( const char *query )
{
	return position_in_list( marc_resource, nmarc_resource, query );
}

int
is_marc_genre( const char *query )
{
	if ( marc_find_genre( query ) != -1 ) return 1;
	else return 0;
}

int
is_marc_resource( const char *query )
{
	if ( marc_find_resource( query ) != -1 ) return 1;
	else return 0;
}
