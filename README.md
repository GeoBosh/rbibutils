
Convert bibliography files between various formats, including BibTeX, BibLaTeX
and Bibentry. Includes an R port of the `bibutils` utilities.


# Installing rbibutils

Install the development version of `rbibutils` from Github:

    library(devtools)
    install_github("GeoBosh/rbibutils")


# Overview

Convert bibliography files between various formats.  All formats supported by
the `bibutils` utilities are available.  In addition, conversion from and to
`bibentry`, (the R native representation based on Bibtex, is supported.

The main function is `bibConvert()`. It takes an input bibliography file in one
of the supported formats, converts its contents to another format, and writes
the result to a file. All formats, except for `rds` (see below) are plain text
files.

The default encoding is UTF-8 for both, input and output. All encodings handled
by `bibutils` are supported.

Bibentry objects can be input from an `R` source file or from an `rds` file. The
`rds` file should contain a `bibentry` R object, saved from R with `saveRDS()`.
The `rds` format is a compressed binary format`. Alternatively, an R source file
can contain one or more bibentry instructions and maybe other commands can be used.
The R file is sourced and all bibentry objects created by it are collected. 

`bibConvert()` tries to infer the input/output formats from the file
extentions. There is ambiguity however about `bib` files, which can be either
Bibtex or Biblatex. Bibtex is assumed if the format is not specified.

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
