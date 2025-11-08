# Convert between bibliography formats

Read a bibliography file in one of the supported formats, convert it to
another format, and write that to a file.

## Usage

``` r
bibConvert(infile, outfile, informat, outformat, ..., tex, encoding, 
           options)
```

## Arguments

- infile:

  input file, a character string.

- outfile:

  output file, a character string.

- informat:

  input format, a character string, see sections “Supported formats” and
  “Details”.

- outformat:

  output format, a character string, see sections “Supported formats”
  and “Details”.

- ...:

  not used.

- tex:

  TeX/LaTeX specific options, see section “Details”, a character vector.

- encoding:

  `character(2)`, a length two vector specifying input and output
  encodings. Default to both is `"utf8"`, see section “Details”.

- options:

  mainly for debugging: additional options for the converters, see
  section “Details”.

## Details

Arguments `informat` and `outformat` can usually be omitted, since
`bibConvert` infers them from the extensions of the names of the input
and output files, see section "File extensions" below. However, there is
ambiguity for the extension `"bib"`, since it is used for Bibtex and
BibLaTeX entries. For this extension, the default for both, `informat`
and `outformat`, is `"bibtex"`.

Package rbibutils supports format `"bibentry"`, in addition to the
formats supported by the bibutils library. A `bibentry` object contains
one or more references. Two formats are supported for `"bibentry"` for
both input and output. A bibentry object previously saved to a file
using `saveRDS` (default extension `"rds"`) or an R source file
containing one or more `bibentry` commands. The `"rds"` file is just
read in and should contain a `bibentry` object.

When `bibconvert` outputs to an R source file, two variants are
supported: `"R"` and `"Rstyle"`. When (`outformat = "R"`, there is one
`bibentry` call for each reference, just as in a Bibtex file, each
reference is a single entry. `outformat = "Rstyle"` uses the format of
`print(be, style = "R")`, i.e., the `bibentry` calls are output as a
comma separated sequence wrapped in
[`c()`](https://rdrr.io/r/base/c.html). For input, it is not necessary
to specify which variant is used.

Note that when the input format and output formats are identical, the
conversion is not necessarilly a null operation (except for `xml`, and
even that may change). For example, depending on the arguments the
character encoding may change. Also, input BibTeX files may contain
additional instructions, such as journal abbreviations, which are
expanded and incorporated in the references but not exported. It should
be remembered also that there may be loss of information when converting
from one format to another.

For a complete list of supported bibliography formats, see section
“Supported formats” below. The documentation of the original bibutils
library (Putnam 2020) gives further details.

Argument `encoding` is a character vector containing 2 elements,
specifying the encoding of the input and output files. If the encodings
are the same, a length one vector can be supplied. The default encodings
are UTF-8 for input and output. A large number of familiar encodings are
supported, e.g. `"latin1"` and `"cp1251"` (Windows Cyrillic). Some
encodings have two or more aliases and they are also accepted. If an
unknown encoding is requested, a list of all supported encodings will be
printed.

Argument `tex` is an unnamed character vector containing switches for
bibtex input and output (mostly output). Currently, the following are
available:

- uppercase:

  write bibtex tags/types in upper case.

- no_latex:

  do not convert latex-style character combinations to letters.

- brackets:

  use brackets, not quotation marks surrounding data.

- dash:

  use one dash `"-"`, not two `"--"`, in page ranges.

- fc:

  add final comma to bibtex output.

By default latex encodings for accented characters are converted to
letters. This may be a problem if the output encoding is not UTF-8,
since some characters created by this process may be invalid in that
encoding. For example, a BibTeX file which otherwise contains only
cyriilic and latin characters may have a few entries with authors
containing latin accented characters represented using the TeX
convention. If those characters are not converted to Unicode letters,
they can be exported to `"cp1251"` (Windows Cyrillic) for example.
Specifying the option `no_latex` should solve the problem in such cases.

Argument `options` is mostly for debugging and mimics the command line
options of the bibutils' binaries. The argument is a named character
vector and is supplied as `c(tag1= val1, tag2 = val2, ...)`, where each
tag is the name of an option and the value is the corresponding value.
The value for options that do not require one is ignored and can be set
to `""`. Some of the available options are:

- h:

  help, show all available options.

- nb:

  do not write Byte Order Mark in UTF8 output.

- verbose:

  print intermediate output.

- debug:

  print even more intermediate output.

## Supported formats

If an input or output format is not specified by arguments, it is
inferred, if possible, from the file extension.

In the table below column Abbreviation shows the abbreviation for
arguments `informat` and `outformat`, column FileExt gives the default
file extension for that format, column Input (Output) contains TRUE if
the format is supported for input (output) and FALSE otherwise. Column
Description gives basic description of the format.

|                  |             |           |            |                                                       |
|------------------|-------------|-----------|------------|-------------------------------------------------------|
| **Abbreviation** | **FileExt** | **Input** | **Output** | **Description**                                       |
| ads              | ads         | FALSE     | TRUE       | ADS reference format                                  |
| bib              | bib         | TRUE      | TRUE       | BibTeX                                                |
| bibtex           | bibtex      | TRUE      | TRUE       | BibTeX                                                |
| biblatex         | biblatex    | TRUE      | TRUE       | BibLaTeX                                              |
| copac            | copac       | TRUE      | FALSE      | COPAC format references                               |
| ebi              | ebi         | TRUE      | FALSE      | EBI XML                                               |
| end              | end         | TRUE      | TRUE       | EndNote (Refer format)                                |
| endx             | endx        | TRUE      | FALSE      | EndNote XML                                           |
| isi              | isi         | TRUE      | TRUE       | ISI web of science                                    |
| med              | med         | TRUE      | FALSE      | Pubmed XML references                                 |
| nbib             | nbib        | TRUE      | TRUE       | Pubmed/National Library of Medicine nbib format       |
| ris              | ris         | TRUE      | TRUE       | RIS format                                            |
| R, r, Rstyle     | R           | TRUE      | TRUE       | R source file containing bibentry commands            |
| rds              | rds         | TRUE      | TRUE       | bibentry object in a binary file created by saveRDS() |
| xml              | xml         | TRUE      | TRUE       | MODS XML intermediate                                 |
| wordbib          | wordbib     | TRUE      | TRUE       | Word 2007 bibliography format                         |

The file `"easyPubMedvig.xml"` used in the examples for Pubmed XML
(`"med"`) was obtained using code from the vignette in package
easyPubMed (Fantini 2019).

## Value

The function is used for the side effect of creating a file in the
requested format. It returns a list, currently containing the following
components:

- infile:

  name of the input file,

- outfile:

  name of the output file,

- nref_in:

  number of references read from the input file,

- nref_out:

  number of references written to the output file.

- bib:

  when `outformat` is one of `"R"`, `"r"` or `"bibentry"`), a bibentry
  object obtained by reading `outfile`; otherwise not present.

Normally, `nref_in` and `nref_out` are the same. If some references were
imported successfully but failed on export, `nref_out` may be smaller
than `nref_in`. In such cases informative messages are printed during
processing. (If this happens silently, it is probably a bug and please
create an issue on Github.)

## Author

Georgi N. Boshnakov

## References

Damiano Fantini (2019). “easyPubMed: Search and Retrieve Scientific
Publication Records from PubMed.” R package version 2.13,
<https://CRAN.R-project.org/package=easyPubMed>.

Chris Putnam (2020). “Library bibutils, version 6.10.”
<https://sourceforge.net/projects/bibutils/>.

## See also

[`readBib`](https://geobosh.github.io/rbibutils/reference/readBib.md),

[`charToBib`](https://geobosh.github.io/rbibutils/reference/readBib.md),

[`readBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md),
[`writeBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)

## Examples

``` r
fn_biblatex <- system.file("bib", "ex0.biblatex",  package = "rbibutils")
fn_biblatex
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/ex0.biblatex"
## file.show(fn_biblatex)

## convert a biblatex file to xml
modl <- tempfile(fileext = ".xml")
bibConvert(infile = fn_biblatex, outfile = modl, informat = "biblatex", outformat = "xml")
#> $infile
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/ex0.biblatex"
#> 
#> $outfile
#> [1] "/tmp/RtmpK0WFVt/file1ecd729430b6.xml"
#> 
#> $nref_in
#> [1] 3
#> 
#> $nref_out
#> [1] 3
#> 
## file.show(modl)

## convert a biblatex file to bibtex
bib <- tempfile(fileext = ".bib")
bib2 <- tempfile(fileext = ".bib")
bibConvert(infile = fn_biblatex, outfile = bib, informat = "biblatex", outformat = "bib")
#> $infile
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/ex0.biblatex"
#> 
#> $outfile
#> [1] "/tmp/RtmpK0WFVt/file1ecd3d45dc2a.bib"
#> 
#> $nref_in
#> [1] 3
#> 
#> $nref_out
#> [1] 3
#> 
## file.show(bib)

## convert a biblatex file to bibentry
rds <- tempfile(fileext = ".rds")
fn_biblatex
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/ex0.biblatex"
rds
#> [1] "/tmp/RtmpK0WFVt/file1ecd1f0f3660.rds"
be <- bibConvert(fn_biblatex, rds, "biblatex", "bibentry")
bea <- bibConvert(fn_biblatex, rds, "biblatex") # same
readRDS(rds)
#> Beach CM, Davidson R (1983). “Distribution-free statistical inference
#> with Lorenz curves and income shares.” _Review of Economic Studies_,
#> *50*, 723-735.
#> 
#> Beach CM, Kaliski SF (1986). “Lorenz curve inference with sample
#> weights: an application to the distribution of unemployment
#> experience.” _Appl. Statist._, *35*, 38-45.
#> 
#> Lambert PJ (1993). _The distribution and redistribution of income. A
#> mathematical analysis_, 2 edition. Manchester University Press,
#> Manchester.

## convert to R source file
r <- tempfile(fileext = ".R")
bibConvert(fn_biblatex, r, "biblatex")
#> $infile
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/ex0.biblatex"
#> 
#> $outfile
#> [1] "/tmp/RtmpK0WFVt/file1ecd130427c3.R"
#> 
#> $nref_in
#> [1] 3
#> 
#> $nref_out
#> [1] 3
#> 
#> $bib
#> Beach CM, Davidson R (1983). “Distribution-free statistical inference
#> with Lorenz curves and income shares.” _Review of Economic Studies_,
#> *50*, 723-735.
#> 
#> Beach CM, Kaliski SF (1986). “Lorenz curve inference with sample
#> weights: an application to the distribution of unemployment
#> experience.” _Appl. Statist._, *35*, 38-45.
#> 
#> Lambert PJ (1993). _The distribution and redistribution of income. A
#> mathematical analysis_, 2 edition. Manchester University Press,
#> Manchester.
#> 
## file.show(r)
cat(readLines(r), sep = "\n")
#> bibentry(bibtype = "Article",
#>          key = "beachanddavidson1983",
#>          author = c(person(given = c("Charles", "M"),
#>                            family = "Beach"),
#>                     person(given = "Russell",
#>                            family = "Davidson")),
#>          title = "Distribution-free statistical inference with Lorenz curves and income shares",
#>          journal = "Review of Economic Studies",
#>          year = "1983",
#>          month = "Feb",
#>          volume = "50",
#>          pages = "723--735")
#> 
#> bibentry(bibtype = "Article",
#>          key = "beachandkaliski1986a",
#>          author = c(person(given = c("C", "M"),
#>                            family = "Beach"),
#>                     person(given = c("S", "F"),
#>                            family = "Kaliski")),
#>          title = "Lorenz curve inference with sample weights: an application to the distribution of unemployment experience",
#>          journal = "Appl. Statist.",
#>          year = "1986",
#>          volume = "35",
#>          pages = "38--45")
#> 
#> bibentry(bibtype = "Book",
#>          key = "lambert1993",
#>          author = person(given = c("Peter", "J"),
#>                          family = "Lambert"),
#>          title = "The distribution and redistribution of income. A mathematical analysis",
#>          year = "1993",
#>          edition = "2",
#>          publisher = "Manchester University Press",
#>          address = "Manchester")

fn_cyr_utf8 <- system.file("bib", "cyr_utf8.bib",  package = "rbibutils")

## Can't have files with different encodings in the package, so below
## first convert a UTF-8 file to something else.
##
## input here contains cyrillic (UTF-8) output to Windows Cyrillic,
## notice the "no_latex" option
a <- bibConvert(fn_cyr_utf8, bib, encoding = c("utf8", "cp1251"), tex = "no_latex")

## now take the bib file and convert it to UTF-8
bibConvert(bib, bib2, encoding = c("cp1251", "utf8"))
#> $infile
#> [1] "/tmp/RtmpK0WFVt/file1ecd3d45dc2a.bib"
#> 
#> $outfile
#> [1] "/tmp/RtmpK0WFVt/file1ecd6c0e2c5.bib"
#> 
#> $nref_in
#> [1] 2
#> 
#> $nref_out
#> [1] 2
#> 

# \donttest{
## Latin-1 example: Author and Title fileds contain Latin-1 accented
##   characters, not real names. As above, the file is in UTF-8
fn_latin1_utf8  <- system.file("bib", "latin1accents_utf8.bib", package = "rbibutils")
## convert to Latin-1, by default the accents are converted to TeX combinations:
b <- bibConvert(fn_latin1_utf8, bib , encoding = c("utf8", "latin1"))
cat(readLines(bib), sep = "\n")
#> @Article{test1,
#> author="L{\^e}{\'a}{\'e}, K{\`a}r{\`e}",
#> title="K{\`a}r{\`e} {\"u}{\"a} {\L}{\l}{\^e}{\'a}{\'e}",
#> journal="Latin-1 accents",
#> year="2020",
#> note="Having the similar title and author is helpful for debugging since authors are processed differently from other fields. Here is a formula: $\tan(\alpha)$, $\sin(\delta)/\delta \to 1$, $\sqrt{x}$."
#> }
#> 
## use "no_latex" option to keep them Latin1:
c <- bibConvert(fn_latin1_utf8, bib , encoding = c("utf8", "latin1"), tex = "no_latex")
## this will show properly in Latin-1 locale (or suitable text editor):
##cat(readLines(bib), sep = "\n")

## gb18030 example (Chinese)
##
## prepare some filenames for the examples below:
xeCJK_utf8    <- system.file("bib/xeCJK_utf8.bib", package = "rbibutils")
xeCJK_gb18030 <- system.file("bib/xeCJK_gb18030.bib", package = "rbibutils")
fn_gb18030 <- tempfile(fileext = ".bib")
fn_rds <- tempfile(fileext = ".rds")

## input bib file utf8, output bib file gb18030:
bibConvert(xeCJK_utf8, fn_gb18030, encoding = c("utf8", "gb18030"))
#> $infile
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/xeCJK_utf8.bib"
#> 
#> $outfile
#> [1] "/tmp/RtmpK0WFVt/file1ecd18dd6dc2.bib"
#> 
#> $nref_in
#> [1] 1
#> 
#> $nref_out
#> [1] 1
#> 

## input bib file utf8, output file rds (and the rds object is returned
bibConvert(xeCJK_utf8, fn_rds)
#> $infile
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/xeCJK_utf8.bib"
#> 
#> $outfile
#> [1] "/tmp/RtmpK0WFVt/file1ecd353d36a1.rds"
#> 
#> $nref_in
#> [1] 1
#> 
#> $nref_out
#> [1] 1
#> 
#> $bib
#> 陈骁, 黄声华, 万山明, 庞珽 (2012).
#> “基于电无级变速器的内燃机最优控制策略及整车能量管理.” _电工技术学报_,
#> *27*(2), 133-138.
#> 
unlink(fn_gb18030)
unlink(fn_rds)

## a Pubmed file
fn_med <- system.file("bib/easyPubMedvig.xml", package = "rbibutils")
## convert a Pubmed file to bibtex:
bibConvert(fn_med, bib, informat = "med")
#> $infile
#> [1] "/home/runner/work/_temp/Library/rbibutils/bib/easyPubMedvig.xml"
#> 
#> $outfile
#> [1] "/tmp/RtmpK0WFVt/file1ecd3d45dc2a.bib"
#> 
#> $nref_in
#> [1] 6
#> 
#> $nref_out
#> [1] 6
#> 
## convert a Pubmed file to rds and import:
#bibConvert(fn_med, rds, informat = "med")
# }

unlink(c(modl, bib, bib2, r, rds))
```
