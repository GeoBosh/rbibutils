
Convert bibliography files between various formats, including BibTeX and
BibLaTeX. This is an R port of the bibutils utilities.


# Installing rbibutils

Install the development version of `rbibutils` from Github:

    library(devtools)
    install_github("GeoBosh/rbibutils")


# Overview

The functionality is incomplete but the package is already usable. UTF-8 and
ASCII encodings are currently supported (this is what I am testing during development).

`bibConvert()` takes an input bibliography file in one of the supported formats,
converts its contents to another format, and writes the result to a file.

Eventually all formats supported by the `bibutils` C library will be available
plus `bibentry` (the R native representation based on Bibtex).

Currently the following formats are available for *input*:

    Bibtex, Biblatex, XML intermediate.

Currently the following formats are available for *output*:

    Bibtex, Biblatex, XML intermediate, Bibentry.

Bibentry objects are output in R's binary `rds` format for saving objects and
can be read into R with `readRDS()`. The other files are text files.

`bibConvert()` ties to infer the format from the file extentions. There is
ambiguity abput `bib` files, which can be either Bibtex or Biblatex. Currently,
Bibtex is assumed if the format is not specified.

# Examples:

Convert Bibtex file `myfile.bib` to a `bibentry` object and save the latter to
`"myfile.rds":

    bibConvert("myfile.bib", "myfile.rds", informat = "bibtex", outformat = "bibentry")
    bibConvert("myfile.bib", "myfile.rds")

Convert Bibtex file `myfile.bib` to a Biblatex save to `"biblatex.bib":

    bibConvert("myfile.bib", "biblatex.bib", "bibtex", "biblatex")
    bibConvert("myfile.bib", "biblatex.bib", outfile = "biblatex")

Assuming `myfile.bib` is a Biblatex file, convert it to Bibtex and save to  `bibtex.bib`:

    bibConvert("myfile.bib", "bibtex.bib", "biblatex", "bibtex")
    bibConvert("myfile.bib", "bibtex.bib", "biblatex")


