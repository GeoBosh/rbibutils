# Read and write bibentry files or read bibtex strings

Read and write bibentry files.

## Usage

``` r
readBibentry(file, extra = FALSE, fbibentry = NULL)

writeBibentry(be, file = stdout(), style = c("Rstyle", "loose"))
```

## Arguments

- be:

  a bibentry object.

- file:

  filename, a character string or a connection. For `readBibentry`,
  input from the console can be specified by `file = ""`. The default
  for `writeBibentry` is `stdout` (effectively, to write on the screen).

- extra:

  if `TRUE` allow non-standard bibtex types.

- style:

  if `"Rstyle"` (default), wrap in
  [`c()`](https://rdrr.io/r/base/c.html), otherwise don't wrap and don't
  put commas between the entries, see section “Details”.

- fbibentry:

  a function to use for generating bib objects. The default is
  [`utils::bibentry()`](https://rdrr.io/r/utils/bibentry.html).

## Details

These functions read/write `bibentry` objects from/to R source files.
Two styles are supported. `"Rstyle"` is the format used by
`print(be, style = "R")`, which writes the `bibentry` calls as a comma
separated sequence wrapped in [`c()`](https://rdrr.io/r/base/c.html)
(i.e., the file contains a single R expression). Style `"loose"` writes
the entries without separators and no wrapping.

`writeBibentry` writes the object to the specified file in the requested
style (default is `"Rstyle"`). The file is neatly formatted for humans
to read and edit.

`readBibentry` reads the file and creates a `bibentry` object. It
doesn't have argument for style, since that is inferred from the
contents of the file.

[`bibentry()`](https://rdrr.io/r/utils/bibentry.html) calls that throw
errors are not included in the returned object. The errors are
intercepted and converted to warnings, identifying the corresponding
[`bibentry()`](https://rdrr.io/r/utils/bibentry.html) calls by their
keys, if present (otherwise the text of the whole bibentry is shown).

## Value

for `writeBibentry`, `NULL` (invisibly)

for `readBibentry`, a `bibentry` object with the keys as names

## Author

Georgi N. Boshnakov

## See also

[`readBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
and
[`writeBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
for reading/writing bib files,

[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md)

[`charToBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
for reading from a character vector

## Examples

``` r
bibs <- readBib(system.file("REFERENCES.bib", package = "rbibutils"),
                encoding = "UTF-8")
fn <- tempfile(fileext = ".bib")

writeBibentry(bibs, file = fn) # style = "Rstyle" (default)
cat(readLines(fn), sep = "\n")
#> c(
#>   bibentry(bibtype = "Manual",
#>          key = "Rpackage:bibtex",
#>          author = person(given = "Romain",
#>                          family = "Francois"),
#>          title = "bibtex: bibtex parser",
#>          year = "2014",
#>          note = "R package version 0.4.0"),
#> 
#>   bibentry(bibtype = "Misc",
#>          key = "Rpackage:Rdpack",
#>          author = person(given = c("Georgi", "N"),
#>                          family = "Boshnakov"),
#>          title = "{Rdpack}: Update and Manipulate Rd Documentation Objects",
#>          year = "2020",
#>          note = "R package version 2.0.0",
#>          doi = "10.5281/zenodo.3925612",
#>          url = "https://doi.org/10.5281/zenodo.3925612"),
#> 
#>   bibentry(bibtype = "Article",
#>          key = "Rpackage:RefManageR",
#>          author = person(given = c("Mathew", "William"),
#>                          family = "McLean"),
#>          title = "RefManageR: Import and Manage BibTeX and BibLaTeX References in R",
#>          journal = "The Journal of Open Source Software",
#>          year = "2017",
#>          doi = "10.21105/joss.00338",
#>          url = "https://doi.org/10.21105/joss.00338"),
#> 
#>   bibentry(bibtype = "Misc",
#>          key = "bibutils6.10",
#>          author = person(given = "Chris",
#>                          family = "Putnam"),
#>          title = "Library bibutils, version 6.10",
#>          year = "2020",
#>          url = "https://sourceforge.net/projects/bibutils/"),
#> 
#>   bibentry(bibtype = "Misc",
#>          key = "Rpackage:easyPubMed",
#>          author = person(given = "Damiano",
#>                          family = "Fantini"),
#>          title = "easyPubMed: Search and Retrieve Scientific Publication Records from PubMed",
#>          year = "2019",
#>          note = "R package version 2.13",
#>          url = "https://CRAN.R-project.org/package=easyPubMed")
#> )
#> 

writeBibentry(bibs, file = fn, style = "loose")
cat(readLines(fn), sep = "\n")
#> bibentry(bibtype = "Manual",
#>          key = "Rpackage:bibtex",
#>          author = person(given = "Romain",
#>                          family = "Francois"),
#>          title = "bibtex: bibtex parser",
#>          year = "2014",
#>          note = "R package version 0.4.0")
#> 
#> bibentry(bibtype = "Misc",
#>          key = "Rpackage:Rdpack",
#>          author = person(given = c("Georgi", "N"),
#>                          family = "Boshnakov"),
#>          title = "{Rdpack}: Update and Manipulate Rd Documentation Objects",
#>          year = "2020",
#>          note = "R package version 2.0.0",
#>          doi = "10.5281/zenodo.3925612",
#>          url = "https://doi.org/10.5281/zenodo.3925612")
#> 
#> bibentry(bibtype = "Article",
#>          key = "Rpackage:RefManageR",
#>          author = person(given = c("Mathew", "William"),
#>                          family = "McLean"),
#>          title = "RefManageR: Import and Manage BibTeX and BibLaTeX References in R",
#>          journal = "The Journal of Open Source Software",
#>          year = "2017",
#>          doi = "10.21105/joss.00338",
#>          url = "https://doi.org/10.21105/joss.00338")
#> 
#> bibentry(bibtype = "Misc",
#>          key = "bibutils6.10",
#>          author = person(given = "Chris",
#>                          family = "Putnam"),
#>          title = "Library bibutils, version 6.10",
#>          year = "2020",
#>          url = "https://sourceforge.net/projects/bibutils/")
#> 
#> bibentry(bibtype = "Misc",
#>          key = "Rpackage:easyPubMed",
#>          author = person(given = "Damiano",
#>                          family = "Fantini"),
#>          title = "easyPubMed: Search and Retrieve Scientific Publication Records from PubMed",
#>          year = "2019",
#>          note = "R package version 2.13",
#>          url = "https://CRAN.R-project.org/package=easyPubMed")

unlink(fn)
```
