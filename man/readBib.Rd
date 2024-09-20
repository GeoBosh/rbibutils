\name{readBib}
\alias{readBib}

\alias{writeBib}
\alias{charToBib}


\concept{bibtex}
\concept{biblatex}
\concept{bibentry}


\title{Read and write bibtex files}
\description{Read and write bibtex files.}
\usage{
readBib(file, encoding = NULL, \dots, direct = FALSE, 
        texChars = c("keep", "convert", "export", "Rdpack"), 
        macros = NULL, extra = FALSE, key, fbibentry = NULL)

writeBib(object, con = stdout(), append = FALSE)

charToBib(text, informat, \dots)
}
\arguments{
  \item{file}{name or path to the file, a character string.}
  \item{encoding}{the encoding of \code{file}, a character string.}
  \item{direct}{
    If \code{TRUE} parse \code{file} directly to \code{bibentry},
    otherwise convert first to intermediate XML, then to
    \code{bibentry}.
  }
  \item{texChars}{
    
    What to do with characters represented by TeX commands  (for
    example, accented Latin charaters? If \code{"export"},
    export as TeX escapes when possible. If \code{"convert"}, convert
    to the target encoding. If \code{"keep"}, output the characters as
    they were in the input file, like \code{"export"}, but don't convert
    normal characters to TeX escapes.

    \code{"Rdpack"} is mainly for internal use and its actions may be
    changed. It is equivalent to "keep" plus some additional processing,
    see \url{https://github.com/GeoBosh/rbibutils/issues/7#issue-1020385889}.
  }
  \item{macros}{

    additional bib files, usually containing bibtex macros, such as
    journal abbreviations.

  }
  \item{object}{a \code{bibentry} object.}
  \item{con}{filename (a character string) or a text connection}
  \item{append}{if \code{TRUE} append to the file.}
  \item{text}{
    a character vector.
  }
  \item{informat}{
    the input format, defaults to \code{"bibtex"}. 
  }
  \item{key}{

    a character vectors of key(s) to use for entries without cite keys.
    Should have the same number of elements as the number of such entries.
    
  }
  \item{...}{
    
    for \code{charTobib}, arguments to be passed on to \code{readBib} or
    \code{bibConvert}, see Details.
    Not used by \code{readBib} and \code{writeBib} (which throw error to
    avoid silently ignoring unknown arguments).

  }
  \item{extra}{if \code{TRUE}, allow non-standard bibtex types.}

  \item{fbibentry}{a function to use for generating bib objects instead
    of \code{bibentry()}, see Details.}

}
\details{

  \code{readBib} is wrapper around \code{bibConvert} for import of
  bibtex files into bibentry objects.
  
  If \code{direct = FALSE}, the bibtex file is converted first to XML
  intermediate, then the XML file is converted to bibentry. The
  advantage of this is that it gives a standardised representation of
  the bibtex input. Fields that cannot be mapped to the intermediate
  format are generally omitted.

  If \code{direct = TRUE} the input file is converted directly to
  bibentry, without the XML intermediate step. This means that
  non-standard fields in the bib entries are preserved in the bibentry
  object.

  Argument \code{texChars}, currently implemented only for the case
  \code{direct = TRUE}, gives some control over the processing of TeX
  sequences representing characters (such as accented Latin characters):
  If it is \code{"keep"} (the default), such sequences are kept as in
  the input. \code{"convert"} causes them to be converted to the
  characters they represent. Finally, \code{"export"} exports characters
  as TeX sequences, whenever possible.

  The difference between \code{"keep"} and \code{"export"} is that
  \code{"keep"} does not convert normal characters to TeX escapes, while
  \code{"export"} does it if possible. For example, if the input file
  contains the TeX sequence \verb{\\"o} 
  representing the letter o-umlaut, \code{"keep"} and \code{"export"}
  will keep it as TeX sequence, while \code{"convert"} will convert it
  to the character o-umlaut in the output encoding (normally UTF-8).  On
  the othe hand, if the input file contains the character o-umlaut, then
  \code{"keep"} and \code{"convert"} will convert it to the output
  encoding of o-umlaut, while \code{"export"} will export it as
  \verb{\\"o}. 

  % 2021-10-21 commenting out since it is fixed now.
  %
  % Note that \code{"convert"} converts also a large number of
  % mathematical symbols (such as \code{"\\alpha"}) to the target encoding
  % (UTF-8), which may or may not be desirable. Also, some mathematical
  % formulas may cause surprises.

  Currently, \code{texChars = "export"} does not process properly
  mathematical formulas.

  \code{fbibentry}, if supplied, should be a function with arguments
  like bibentry to be used instead of \code{utils::bibentry} for
  creating bib objects in \R.
  
  \code{writeBib} writes a \code{bibentry} object to a bibtex file.

  \code{charTobib} is a convenience function for reading or converting
  bibliography information, accepting the input from a
  character vector rather than a file. If \code{informat} is missing it calls
  \code{readBib}, otherwise \code{bibConvert}. In both cases
  the remaining arguments are passed on and should be suitable for the
  called function.

  The files specified by argument \code{macros} are read in before those
  in \code{file}. Currently this is implemented by concatenating the
  files in the order they appear in \code{c(macros, file)}. It is ok for
  \code{macros} to be \code{character(0)}.
  
}
\value{

  for \code{readBib}, a \code{bibentry} object.
  If \code{extra} is \code{TRUE} it can also
  be \code{bibentryExtra} (which inherits from \code{bibentry}).
  If \code{fbibentry} is a function the return value is whatever it
  returns.

  for \code{writeBib}, the \code{bibentry} object (invisibly)
}
% \references{
% %% ~put references to the literature/web site here ~
% }
\author{Georgi N. Boshnakov}
%% ~Make other sections like Warning with \section{Warning }{....} ~
\seealso{
  \code{\link{readBibentry}} and \code{\link{writeBibentry}} for
  import/export to R code,
  
  \code{\link{bibConvert}}
}
\examples{
## create a bibentry object
bibs <- readBib(system.file("REFERENCES.bib", package = "rbibutils"),
                encoding = "UTF-8")
## write bibs to a file
fn <- tempfile(fileext = ".bib")
writeBib(bibs, fn)

## see the contents of the file
readLines(fn) # or: file.show(fn)

## import a bib file containing Chineese characters encoded with UTF-8:
ch_bib <- readBib(system.file("bib/xeCJK_utf8.bib", package = "rbibutils"))
ch_bib
print(ch_bib, style = "R")

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
print(be, style = "bibtex")
## compare to the input file:
readLines(bibacc)


be1 <- readBib(bibacc, direct = TRUE)
be1a <- readBib(bibacc, direct = TRUE, texChars = "keep") # same
be1
print(be1, style = "R")
print(be1, style = "bibtex")

## export as TeX escapes, when possible
be2 <- readBib(bibacc, direct = TRUE, texChars = "export") ## same
be2
print(be2, style = "R")
print(be2, style = "bibtex")

## in older versions (up to 2.2.4) of rbibutils, "convert" converted
##     a lot of TeX commands representing symbols  to characters.
## This is no longer the case:
be3 <- readBib(bibacc, direct = TRUE, texChars = "convert")
## be3
print(be3, style = "R")
## print(be3, style = "bibtex")

## charToBib
##
## get a bibtex reference for R
Rcore <- format(citation(), style = "bibtex")
## add a citation key
Rcore <- sub("@Manual{", "@Manual{Rcore", Rcore, fixed = TRUE)
cat(Rcore, sep = "\n")
beRcore <- charToBib(Rcore)
beRcore
class(beRcore)
print(beRcore, style = "R")

## bibtex entries generated by citation() don't have cite keys.
## this sets the key to 'Rcore'
beRcore <- charToBib(toBibtex(citation()), key = "Rcore")
beRcore$key == "Rcore"  # TRUE

## this sets two keys
bemore <- charToBib(toBibtex( c(citation(), citation("rbibutils"))),
    key = c("Rcore", "Rpackage:rbibutils"))
all.equal(names(bemore), c("Rcore", "Rpackage:rbibutils"))


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
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
% \keyword{ ~kwd1 }
% Use only one keyword per line.
% For non-standard keywords, use \concept instead of \keyword:
\concept{bibtex}
% Use only one concept per line.
