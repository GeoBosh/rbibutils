[![CRANStatusBadge](http://www.r-pkg.org/badges/version/rbibutils)](https://cran.r-project.org/package=rbibutils)
[![CRAN RStudio mirror downloads](https://cranlogs.r-pkg.org/badges/rbibutils)](https://www.r-pkg.org/pkg/rbibutils)

Convert bibliography files between various formats, including BibTeX, BibLaTeX
and Bibentry. Includes an R port of the `bibutils` utilities.


# Installing rbibutils

Install the  [latest stable version](https://cran.r-project.org/package=rbibutils) from CRAN:

    install.packages("rbibutils")

You can also install the [development version](https://github.com/GeoBosh/rbibutils) of `rbibutils` from Github:

    library(devtools)
    install_github("GeoBosh/rbibutils")



# Overview

Convert bibliography files between various formats.  All formats supported by
the `bibutils` utilities are available.  In addition, conversion from and to
`bibentry`, the R native representation based on Bibtex, is supported.

The main function is `bibConvert()`. It takes an input bibliography file in one
of the supported formats, converts its contents to another format, and writes
the result to a file. All formats, except for `rds` (see below) are plain text
files. `bibConvert()` tries to infer the input/output formats from the file
extentions. There is ambiguity however about `bib` files, which can be either
Bibtex or Biblatex. Bibtex is assumed if the format is not specified.

The default encoding is UTF-8 for both, input and output. All encodings handled
by `bibutils` are supported. Besides UTF-8, these include `gb18030` (Chinese),
ISO encodings such as `iso8859_1`, Windows code pages (e.g. `cp1251` for Windows
Cyrillic) and many others. Common alternative names are also accepted
(e.g. `latin1`).

Bibentry objects can be input from an `R` source file or from an `rds` file. The
`rds` file should contain a `bibentry` R object, saved from R with `saveRDS()`.
The `rds` format is a compressed binary format`. Alternatively, an R source file
containing one or more bibentry instructions and maybe other commands can be used.
The R file is sourced and all bibentry objects created by it are collected. 



# Examples:

Convert Bibtex file `myfile.bib` to a `bibentry` object and save the latter to
`"myfile.rds":

    bibConvert("myfile.bib", "myfile.rds", informat = "bibtex", outformat = "bibentry")
    bibConvert("myfile.bib", "myfile.rds")

Convert Bibtex file `myfile.bib` to a Biblatex save to `"biblatex.bib":

    bibConvert("myfile.bib", "biblatex.bib", "bibtex", "biblatex")
    bibConvert("myfile.bib", "biblatex.bib", outfile = "biblatex")

Convert Bibtex file `myfile.bib` to Bibentry and save as `rds` or `R`:

    bibConvert("myfile.bib", "myfile.rds")
    bibConvert("myfile.bib", "myfile.R")

Read back the above files and/or convert them to other formats:

    file.show("myfile.R")
    readRDS("myfile.rds")
    bibConvert("myfile.rds", "myfile.bib")
    bibConvert("myfile.R", "myfile.bib")


Assuming `myfile.bib` is a Biblatex file, convert it to Bibtex and save to  `bibtex.bib`:

    bibConvert("myfile.bib", "bibtex.bib", "biblatex", "bibtex")
    bibConvert("myfile.bib", "bibtex.bib", "biblatex")
