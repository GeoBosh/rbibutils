# Supported bibliography formats

Supported bibliography formats in package rbibutils.

## Usage

``` r
rbibutils_formats
```

## Format

A data frame with 16 observations on the following 5 variables:

- `Abbreviation`:

  a character vector.

- `FileExt`:

  a character vector.

- `Input`:

  a logical vector.

- `Output`:

  a logical vector.

- `Description`:

  a character vector.

## Details

Each row in `rbibutils_formats` gives information about a supported
bibliography format in package rbibutils.

`Abbreviation` is the name to use in arguments `informat` and
`outformat` in
[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md).

`FileExt` is the default extension used to infer the input/output format
if the corresponding argument is missing.

`Input` (`Output`) is `TRUE` if the format is supported for input
(output).

`Description` gives a brief description of the format.

See
[`bibConvert`](https://geobosh.github.io/rbibutils/reference/bibConvert.md),
section “Supported formats”, for further details.

## Examples

``` r
rbibutils_formats
#>    Abbreviation  FileExt Input Output
#> 1           ads      ads FALSE   TRUE
#> 2           bib      bib  TRUE   TRUE
#> 3        bibtex   bibtex  TRUE   TRUE
#> 4      biblatex biblatex  TRUE   TRUE
#> 5         copac    copac  TRUE  FALSE
#> 6           ebi      ebi  TRUE  FALSE
#> 7           end      end  TRUE   TRUE
#> 8          endx     endx  TRUE  FALSE
#> 9           isi      isi  TRUE   TRUE
#> 10          med      med  TRUE  FALSE
#> 11         nbib     nbib  TRUE   TRUE
#> 12          ris      ris  TRUE   TRUE
#> 13 R, r, Rstyle        R  TRUE   TRUE
#> 14          rds      rds  TRUE   TRUE
#> 15          xml      xml  TRUE   TRUE
#> 16      wordbib  wordbib  TRUE   TRUE
#>                                              Description
#> 1                                   ADS reference format
#> 2                                                 BibTeX
#> 3                                                 BibTeX
#> 4                                               BibLaTeX
#> 5                                COPAC format references
#> 6                                                EBI XML
#> 7                                 EndNote (Refer format)
#> 8                                            EndNote XML
#> 9                                     ISI web of science
#> 10                                 Pubmed XML references
#> 11       Pubmed/National Library of Medicine nbib format
#> 12                                            RIS format
#> 13            R source file containing bibentry commands
#> 14 bibentry object in a binary file created by saveRDS()
#> 15                                 MODS XML intermediate
#> 16                         Word 2007 bibliography format
```
