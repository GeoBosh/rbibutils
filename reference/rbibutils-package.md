# Read 'Bibtex' Files and Convert Between Bibliography Formats

Read and write 'Bibtex' files. Convert between bibliography formats,
including 'Bibtex', 'Biblatex', 'PubMed', 'Endnote', and 'Bibentry'.
Includes a port of the 'bibutils' utilities by Chris Putnam
\<https://sourceforge.net/projects/bibutils/\>. Supports all
bibliography formats and character encodings implemented in 'bibutils'.

## Details

Package rbibutils provides an R port of the `bibutils` programs plus
additional facilities. The main function, `bibConvert`, offers all
conversions between bibliography formats supported by library
`bibutils`. In addition, package rbibutils converts to and from R's
`bibentry` Bibtex-based bibliography format.

The core functionality is provided by the `bibutils` programs which
convert between various bibliography formats using a common MODS XML
intermediate format, see the source cited below.

Currently we provide the function
[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md)
for conversion between supported bibliography formats. For complete list
of formats supported by the package, see the documentation of the
original bibutils library.

[`readBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
and
[`writeBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
import/export BiBTeX files.
[`readBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)
and
[`writeBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)
import/export R source files in which the references are represented by
[`bibentry()`](https://rdrr.io/r/utils/bibentry.html) calls. These
functions were originally just wrappers around `bibConvert`.
[`readBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
has acquired additional features, including a direct import (without
going through `bibConvert`) from BiBTeX files.

All encodings supported by the bibutils library are available for
[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md).

Further functionality may be provided in future releases, in particular,
the underlying C functions could be exposed to package authors. Further
R wrappers may be added, as well. However, the scope of the package will
remain conversion between formats based on `bibutils` and manipulation
of the MODS XML intermediate format. rbibutils can be used also as an
alternative to package bibtex (Francois 2020). For bibliography
management see package RefManager (McLean 2017). For citations in R
documentation (Rd or roxygen2) see package Rdpack (Boshnakov 2020).

## Supported input and output formats

Most formats are supported for both input and output, see the listings
below. A format supported for input can be converted to any of the
output formats.

The input is first converted to *MODS XML intermediate*, the latter is
then converted to the requested output format. In rbibutils there are
currently two exceptions to this rule. First, the conversion from bibtex
to bibentry offers the option to bypass the conversion to *MODS XML
intermediate* and parse directly the bibtex file, see
[`readBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
for details. Second, the conversion from `bibentry` to `BibTeX` just
uses a `print` method provided by R.

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

`bibentry` is the native R variant of BibTeX. It can be input directly
from an R source file or from a binary `rds` file. The `"rds"` format is
a compressed binary format. The `rds` file should contain a `bibentry` R
object, saved from R with `saveRDS`. An R source file should contain one
or more bibentry instructions, see
[`readBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)
for details of the contents.

A `bibentry` object can be written to a file as a binary (`"rds"`)
object or as an R source file, see
[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md)
and
[`writeBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
for details.

ADS is the reference format of the Smithsonian Astrophysical Observatory
(SAO) and National Aeronautics and Space Administration (NASA)
Astrophyics Data System.

For COPAC, see `https://en.wikipedia.org/wiki/Copac`.

## Author

Georgi N. Boshnakov (R code and R port of bibutils), Chris Putnam
(author of bibutils library)

## Note

The `bibutils` library is included in a number of software packages.
These include include pandoc and a library for Haskell. Executable
programs for conversion are available for Linux distributions but seem
not easily available for Windows. Executable and libraries can also be
generated out-of-the-box from the `bibutils` disribution (on Windows
under MSYS).

rbibutils adds conversions to/from R's bibentry format and direct
conversion from bibtex, which preserves non-standard fields from the
bibtex source. There is also improved support for mathematical
expressions in bibtex files.

## References

Georgi N Boshnakov (2020). “Rdpack: Update and Manipulate Rd
Documentation Objects.”
[doi:10.5281/zenodo.3925612](https://doi.org/10.5281/zenodo.3925612) , R
package version 2.0.0.

Damiano Fantini (2019). “easyPubMed: Search and Retrieve Scientific
Publication Records from PubMed.” R package version 2.13,
<https://CRAN.R-project.org/package=easyPubMed>.

Romain Francois (2014). *bibtex: bibtex parser*. R package version
0.4.0.

Mathew William McLean (2017). “RefManageR: Import and Manage BibTeX and
BibLaTeX References in R.” *The Journal of Open Source Software*.
[doi:10.21105/joss.00338](https://doi.org/10.21105/joss.00338) .

Chris Putnam (2020). “Library bibutils, version 6.10.”
<https://sourceforge.net/projects/bibutils/>.

## See also

[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md)` for further details and examples`

[`readBib`](https://geobosh.github.io/rbibutils/reference/readBib.md),

[`charToBib`](https://geobosh.github.io/rbibutils/reference/readBib.md),

[`readBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md),
[`writeBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)
