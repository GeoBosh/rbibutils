# Read and write bibtex files

Read and write bibtex files.

## Usage

``` r
readBib(file, encoding = NULL, ..., direct = FALSE, 
        texChars = c("keep", "convert", "export", "Rdpack"), 
        macros = NULL, extra = FALSE, key, fbibentry = NULL)

writeBib(object, con = stdout(), append = FALSE)

charToBib(text, informat, ...)
```

## Arguments

- file:

  name or path to the file, a character string.

- encoding:

  the encoding of `file`, a character string.

- direct:

  If `TRUE` parse `file` directly to `bibentry`, otherwise convert first
  to intermediate XML, then to `bibentry`.

- texChars:

  What to do with characters represented by TeX commands (for example,
  accented Latin charaters? If `"export"`, export as TeX escapes when
  possible. If `"convert"`, convert to the target encoding. If `"keep"`,
  output the characters as they were in the input file, like `"export"`,
  but don't convert normal characters to TeX escapes.

  `"Rdpack"` is mainly for internal use and its actions may be changed.
  It is equivalent to "keep" plus some additional processing, see
  <https://github.com/GeoBosh/rbibutils/issues/7#issue-1020385889>.

- macros:

  additional bib files, usually containing bibtex macros, such as
  journal abbreviations.

- object:

  a `bibentry` object.

- con:

  filename (a character string) or a text connection

- append:

  if `TRUE` append to the file.

- text:

  a character vector.

- informat:

  the input format, defaults to `"bibtex"`.

- key:

  a character vectors of key(s) to use for entries without cite keys.
  Should have the same number of elements as the number of such entries.

- ...:

  for `charTobib`, arguments to be passed on to `readBib` or
  `bibConvert`, see section “Details”. Not used by `readBib` and
  `writeBib` (which throw error to avoid silently ignoring unknown
  arguments).

- extra:

  if `TRUE`, allow non-standard bibtex types.

- fbibentry:

  a function to use for generating bib objects instead of
  [`bibentry()`](https://rdrr.io/r/utils/bibentry.html), see section
  “Details”.

## Details

`readBib` is wrapper around `bibConvert` for import of bibtex files into
bibentry objects.

If `direct = FALSE`, the bibtex file is converted first to XML
intermediate, then the XML file is converted to bibentry. The advantage
of this is that it gives a standardised representation of the bibtex
input. Fields that cannot be mapped to the intermediate format are
generally omitted.

If `direct = TRUE` the input file is converted directly to bibentry,
without the XML intermediate step. This means that non-standard fields
in the bib entries are preserved in the bibentry object.

Argument `texChars`, currently implemented only for the case
`direct = TRUE`, gives some control over the processing of TeX sequences
representing characters (such as accented Latin characters): If it is
`"keep"` (the default), such sequences are kept as in the input.
`"convert"` causes them to be converted to the characters they
represent. Finally, `"export"` exports characters as TeX sequences,
whenever possible.

The difference between `"keep"` and `"export"` is that `"keep"` does not
convert normal characters to TeX escapes, while `"export"` does it if
possible. For example, if the input file contains the TeX sequence `\"o`
representing the letter o-umlaut, `"keep"` and `"export"` will keep it
as TeX sequence, while `"convert"` will convert it to the character
o-umlaut in the output encoding (normally UTF-8). On the othe hand, if
the input file contains the character o-umlaut, then `"keep"` and
`"convert"` will convert it to the output encoding of o-umlaut, while
`"export"` will export it as `\"o`.

Currently, `texChars = "export"` does not process properly mathematical
formulas.

`fbibentry`, if supplied, will be used in place of
[`utils::bibentry`](https://rdrr.io/r/utils/bibentry.html) to create bib
objects in R. The arguments of `fbibentry` should be the same as for
[`utils::bibentry`](https://rdrr.io/r/utils/bibentry.html).

`writeBib` writes a `bibentry` object to a bibtex file.

`charTobib` is a convenience function for reading or converting
bibliography information, accepting the input from a character vector
rather than a file. If `informat` is missing it calls `readBib`,
otherwise `bibConvert`. In both cases the remaining arguments are passed
on and should be suitable for the called function.

The files specified by argument `macros` are read in before those in
`file`. Currently this is implemented by concatenating the files in the
order they appear in `c(macros, file)`. It is ok for `macros` to be
`character(0)`.

## Value

for `readBib`, a `bibentry` object. If `extra` is `TRUE` it can also be
`bibentryExtra` (which inherits from `bibentry`). If `fbibentry` is a
function the return value is whatever it returns.

for `writeBib`, the `bibentry` object (invisibly)

## Author

Georgi N. Boshnakov

## See also

[`readBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)
and
[`writeBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)
for import/export to R code,

[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md)

## Examples

``` r
## create a bibentry object
bibs <- readBib(system.file("REFERENCES.bib", package = "rbibutils"),
                encoding = "UTF-8")
## write bibs to a file
fn <- tempfile(fileext = ".bib")
writeBib(bibs, fn)

## see the contents of the file
readLines(fn) # or: file.show(fn)
#>  [1] "@Manual{Rpackage:bibtex,"                                                               
#>  [2] "  author = {Romain Francois},"                                                          
#>  [3] "  title = {bibtex: bibtex parser},"                                                     
#>  [4] "  year = {2014},"                                                                       
#>  [5] "  note = {R package version 0.4.0},"                                                    
#>  [6] "}"                                                                                      
#>  [7] ""                                                                                       
#>  [8] "@Misc{Rpackage:Rdpack,"                                                                 
#>  [9] "  author = {Georgi N Boshnakov},"                                                       
#> [10] "  title = {{Rdpack}: Update and Manipulate Rd Documentation Objects},"                  
#> [11] "  year = {2020},"                                                                       
#> [12] "  note = {R package version 2.0.0},"                                                    
#> [13] "  doi = {10.5281/zenodo.3925612},"                                                      
#> [14] "  url = {https://doi.org/10.5281/zenodo.3925612},"                                      
#> [15] "}"                                                                                      
#> [16] ""                                                                                       
#> [17] "@Article{Rpackage:RefManageR,"                                                          
#> [18] "  author = {Mathew William McLean},"                                                    
#> [19] "  title = {RefManageR: Import and Manage BibTeX and BibLaTeX References in R},"         
#> [20] "  journal = {The Journal of Open Source Software},"                                     
#> [21] "  year = {2017},"                                                                       
#> [22] "  doi = {10.21105/joss.00338},"                                                         
#> [23] "  url = {https://doi.org/10.21105/joss.00338},"                                         
#> [24] "}"                                                                                      
#> [25] ""                                                                                       
#> [26] "@Misc{bibutils6.10,"                                                                    
#> [27] "  author = {Chris Putnam},"                                                             
#> [28] "  title = {Library bibutils, version 6.10},"                                            
#> [29] "  year = {2020},"                                                                       
#> [30] "  url = {https://sourceforge.net/projects/bibutils/},"                                  
#> [31] "}"                                                                                      
#> [32] ""                                                                                       
#> [33] "@Misc{Rpackage:easyPubMed,"                                                             
#> [34] "  author = {Damiano Fantini},"                                                          
#> [35] "  title = {easyPubMed: Search and Retrieve Scientific Publication Records from PubMed},"
#> [36] "  year = {2019},"                                                                       
#> [37] "  note = {R package version 2.13},"                                                     
#> [38] "  url = {https://CRAN.R-project.org/package=easyPubMed},"                               
#> [39] "}"                                                                                      

## import a bib file containing Chineese characters encoded with UTF-8:
ch_bib <- readBib(system.file("bib/xeCJK_utf8.bib", package = "rbibutils"))
ch_bib
#> 陈骁, 黄声华, 万山明, 庞珽 (2012).
#> “基于电无级变速器的内燃机最优控制策略及整车能量管理.” _电工技术学报_,
#> *27*(2), 133-138.
print(ch_bib, style = "R")
#> bibentry(bibtype = "Article",
#>          key = "chen2012",
#>          author = c(person(family = "陈骁"),
#>                     person(family = "黄声华"),
#>                     person(family = "万山明"),
#>                     person(family = "庞珽")),
#>          title = "基于电无级变速器的内燃机最优控制策略及整车能量管理",
#>          journal = "电工技术学报",
#>          year = "2012",
#>          volume = "27",
#>          number = "2",
#>          pages = "133--138")

## import a bib file encoded with the official Chineese encoding:
ch_bib2 <- readBib(system.file("bib/xeCJK_gb18030.bib", package = "rbibutils"),
                   encoding = "gb18030")


## a dummy reference with accented characters
## (in the file some are utf8, others are TeX escapes)
bibacc <- system.file("bib/latin1accents_utf8.bib", package = "rbibutils")

## export as UTF-8 characters
## this will print as true characters in suitable locale:
be <- readBib(bibacc, direct = TRUE, texChars = "convert")
print(be, style = "R")
#> bibentry(bibtype = "Article",
#>          key = "test1",
#>          author = person(given = "Kàrè",
#>                          family = "Lêáé"),
#>          title = "Kàrè üä Łłêáé",
#>          journal = "Latin-1 accents",
#>          year = "2020",
#>          note = "Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\\tan(\\alpha)$, $\\sin(\\delta)/\\delta \\to 1$, $\\sqrt{x}$.")
print(be, style = "bibtex")
#> @Article{test1,
#>   author = {Kàrè Lêáé},
#>   title = {Kàrè üä Łłêáé},
#>   journal = {Latin-1 accents},
#>   year = {2020},
#>   note = {Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\tan(\alpha)$, $\sin(\delta)/\delta \to 1$, $\sqrt{x}$.},
#> }
## compare to the input file:
readLines(bibacc)
#> [1] "   @ARTICLE{test1,"                                                                             
#> [2] "     AUTHOR = {Kàrè Lêáé},"                                                                     
#> [3] "     TITLE = {Kàrè \\\"u\\\"a \\L\\lêáé},"                                                      
#> [4] "     JOURNAL = {Latin-1 accents},"                                                              
#> [5] "     YEAR = {2020},"                                                                            
#> [6] "     NOTE = {Having the similar title and author is helpful for debugging since authors are"    
#> [7] "                  processed differently from other fields. Here is a formula: $\\tan(\\alpha)$,"
#> [8] "                  $\\sin(\\delta)/\\delta \\to 1$, $\\sqrt{x}$.}"                               
#> [9] "}"                                                                                              


be1 <- readBib(bibacc, direct = TRUE)
be1a <- readBib(bibacc, direct = TRUE, texChars = "keep") # same
be1
#> Lêáé K (2020). “Kàrè üä Łłêáé.” _Latin-1 accents_. Having the similar
#> title and author is helpful for debugging since authors are processed
#> differently from other fields. Here is a formula: $\tan(\alpha)$,
#> $\sin(\delta)/\delta\to1$, $\sqrt{x}$.
print(be1, style = "R")
#> bibentry(bibtype = "Article",
#>          key = "test1",
#>          author = person(given = "Kàrè",
#>                          family = "Lêáé"),
#>          title = "Kàrè \\\"u\\\"a \\L\\lêáé",
#>          journal = "Latin-1 accents",
#>          year = "2020",
#>          note = "Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\\tan(\\alpha)$, $\\sin(\\delta)/\\delta \\to 1$, $\\sqrt{x}$.")
print(be1, style = "bibtex")
#> @Article{test1,
#>   author = {Kàrè Lêáé},
#>   title = {Kàrè \"u\"a \L\lêáé},
#>   journal = {Latin-1 accents},
#>   year = {2020},
#>   note = {Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\tan(\alpha)$, $\sin(\delta)/\delta \to 1$, $\sqrt{x}$.},
#> }

## export as TeX escapes, when possible
be2 <- readBib(bibacc, direct = TRUE, texChars = "export") ## same
be2
#> Lêáé K (2020). “Kàrè üä Łłêáé.” _Latin-1 accents_. Having the similar
#> title and author is helpful for debugging since authors are processed
#> differently from other fields. Here is a formula: $\tan(\alpha)$,
#> $\sin(\delta)/\delta\to1$, $\sqrt{x}$.
print(be2, style = "R")
#> bibentry(bibtype = "Article",
#>          key = "test1",
#>          author = person(given = "K{\\`a}r{\\`e}",
#>                          family = "L{\\^e}{\\'a}{\\'e}"),
#>          title = "K{\\`a}r{\\`e} \\\"u\\\"a \\L\\l{\\^e}{\\'a}{\\'e}",
#>          journal = "Latin-1 accents",
#>          year = "2020",
#>          note = "Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\\tan(\\alpha)$, $\\sin(\\delta)/\\delta \\to 1$, $\\sqrt{x}$.")
print(be2, style = "bibtex")
#> @Article{test1,
#>   author = {K{\`a}r{\`e} L{\^e}{\'a}{\'e}},
#>   title = {K{\`a}r{\`e} \"u\"a \L\l{\^e}{\'a}{\'e}},
#>   journal = {Latin-1 accents},
#>   year = {2020},
#>   note = {Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\tan(\alpha)$, $\sin(\delta)/\delta \to 1$, $\sqrt{x}$.},
#> }

## in older versions (up to 2.2.4) of rbibutils, "convert" converted
##     a lot of TeX commands representing symbols  to characters.
## This is no longer the case:
be3 <- readBib(bibacc, direct = TRUE, texChars = "convert")
## be3
print(be3, style = "R")
#> bibentry(bibtype = "Article",
#>          key = "test1",
#>          author = person(given = "Kàrè",
#>                          family = "Lêáé"),
#>          title = "Kàrè üä Łłêáé",
#>          journal = "Latin-1 accents",
#>          year = "2020",
#>          note = "Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\\tan(\\alpha)$, $\\sin(\\delta)/\\delta \\to 1$, $\\sqrt{x}$.")
## print(be3, style = "bibtex")

## charToBib
##
## get a bibtex reference for R
Rcore <- format(citation(), style = "bibtex")
## add a citation key
Rcore <- sub("@Manual{", "@Manual{Rcore", Rcore, fixed = TRUE)
cat(Rcore, sep = "\n")
#> @Manual{Rcore,
#>   title = {R: A Language and Environment for Statistical Computing},
#>   author = {{R Core Team}},
#>   organization = {R Foundation for Statistical Computing},
#>   address = {Vienna, Austria},
#>   year = {2025},
#>   url = {https://www.R-project.org/},
#> }
beRcore <- charToBib(Rcore)
beRcore
#> R Core Team (2025). _R: A Language and Environment for Statistical
#> Computing_. <https://www.R-project.org/>.
class(beRcore)
#> [1] "bibentryExtra" "bibentry"     
print(beRcore, style = "R")
#> bibentry(bibtype = "Manual",
#>          key = "Rcore",
#>          author = person(family = "R Core Team"),
#>          title = "R: A Language and Environment for Statistical Computing",
#>          year = "2025",
#>          publisher = "R Foundation for Statistical Computing",
#>          address = "Vienna, Austria",
#>          url = "https://www.R-project.org/")

## bibtex entries generated by citation() don't have cite keys.
## this sets the key to 'Rcore'
beRcore <- charToBib(toBibtex(citation()), key = "Rcore")
beRcore$key == "Rcore"  # TRUE
#> [1] TRUE

## this sets two keys
bemore <- charToBib(toBibtex( c(citation(), citation("rbibutils"))),
    key = c("Rcore", "Rpackage:rbibutils"))
all.equal(names(bemore), c("Rcore", "Rpackage:rbibutils"))
#> [1] TRUE


## a large example with several files - needs internet access;
## it is better to clone repository https://github.com/iridia-ulb/references
## and work on local files
##
## iridia_mac <- c("abbrev.bib", "authors.bib", "journals.bib", "crossref.bib")
## iridia_biblio <- "biblio.bib"
## 
## iridia_raw_url <- "https://raw.githubusercontent.com/iridia-ulb/references/master"
## iridia_mac_url <- file.path(iridia_raw_url, iridia_mac)
## iridia_biblio_url <- file.path(iridia_raw_url, iridia_biblio)
## 
## bibdir <- tempdir()
## iridia_mac_loc <- file.path(bibdir, iridia_mac)
## iridia_biblio_loc <- file.path(bibdir, iridia_biblio)
## 
## ## download the files to bibdir
## sapply(c(iridia_biblio_url, iridia_mac_url),
##        function(x) download.file(x, file.path(bibdir, basename(x))))
## 
## iridia <- readBib(iridia_biblio_loc, direct = TRUE, macros = iridia_mac_loc)
## iridia[1]
## print(iridia[1], style = "R")
## toBibtex(iridia[1])  # or: print(iridia[1], style = "bibtex")
## length(iridia)  # 2576 at the time of writing

unlink(fn)
```
