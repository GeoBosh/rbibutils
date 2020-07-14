\name{bibConvert}
\alias{bibConvert}
\title{Convert between bibliography formats}
\description{

  Read a bibliography file in one of the supported formats, convert it
  to nnother format, and write it to a file.

}
\usage{
bibConvert(infile, outfile, informat, outformat, \dots, tex, encoding, 
           options)
}
\arguments{
  \item{infile}{input file, a character string.}
  \item{outfile}{output file, a character string.}
  \item{informat}{input format, a character string, see Details.}
  \item{outformat}{output format, a character string, see Details.}
  \item{...}{not used.}
  \item{tex}{TeX specific options, see Details, a character vector.}
  \item{encoding}{

    \code{character(2)}, a length two vector specifying input and output
    encodings. Default to both is \code{"utf8"}, see Details.

  }
  \item{options}{
    mainly for debugging: additional options for the converters, see
    Details.
  }
}
\details{

  Arguments \code{informat} and \code{outformat} can usually be omitted,
  since \code{bibConvert} infers them from the extensions of the names
  of the input and output files.
  However, there is ambiguity for the extension \code{"bib"}, since it
  is used for Bibtex and Biblatex entries. For this extension, the
  default for both, \code{informat} and \code{outformat}, is
  \code{"bibtex"}.

  Package \pkg{rbibutils} supports format \code{"bibentry"}, in addition
  to the formats supported by the bibutils library. A \code{bibentry}
  objects contains one or more references.  Two formats are supported
  for \code{"bibentry"} for both input and output. A bibentry object
  previously saved to a file using \code{saveRDS} (default extension
  \code{"rds"}) or an R source file containing one or more
  \code{bibentry} commands. The \code{"rds"} file is just read in and
  should contain a \code{bibentry} object.

  When \code{bibconvert} outputs to an R source file, there is one
  \code{bibentry} call for each reference, just as in a Bibtex file,
  each reference is a single entry. Such a file can be used as input to
  \code{bibConvert}. For input \code{bibConvert} accepts also R code
  containing additional instructions. The input file is (effectively)
  \code{source}'d, all bibentry objects created by it are collected and
  merged into a single \code{bibentry} object.

  Note that when the input format and output format are identical, the
  conversion is not necessarilly a null operation (except for
  \verb{xml}, and even that may change).  It should be remembered also
  that there may be loss of information when converting from one format
  to another.

  For complete list of supported bibliography formats, see the
  documentation of the original bibutils library.

  Argument \code{encoding} is a character vector containing 2 elements,
  specifying the encoding of the input and output files.  If the
  encodings are the same, a length one vector can be supplied. The
  default encodings are UTF-8 for input and output. A large number of
  familiar encodings are supported, e.g. \code{"latin1"} and
  \code{"cp1251"} (Windows Cyrillic). Some encodings have two or more
  aliases and they are also accepted. If an unknown encodingis
  requested, a list of all supported encodings will be printed.

  Argument \code{tex} is an unnamed character vector containing switches
  for bibtex input and output (mostly output). Currently, the following
  are available:

  \describe{
    \item{uppercase}{write bibtex tags/types in upper case.}
    \item{no_latex}{
      do not convert latex-style character combinations to letters.
    }
    \item{brackets}{use brackets, not quotation marks surrounding data.}
    \item{dash}{
      use one dash \code{"-"}, not two \code{"--"}, in page ranges.
    }
    \item{fc}{add final comma to bibtex output.}
  }

  By default latex encodings for accented characters are converted to
  letters. This may be a problem if the output encoding is not UTF-8,
  since some characters created by this process may be invalid in that
  encoding. For example, a Bibtex file which otherwise contain only
  cyriilic and latin characters may have a few entries with authors
  containing latin accented characters represented using the TeX
  convention.  If those characters are not converted to Unicode letters,
  they can be exported to \code{"cp1251"} (Windows Cyrillic) for
  example. Specifying the option \code{no_latex} should solve the
  problem in such cases.
  
  Argument \code{options} is mostly for debugging and mimics the command
  line options of the bibutils' binaries. The argument is a named
  character vector and is supplied as \code{c(tag1= val1, tag2 = val2,
  ...)}, where each tag is the name of an option and the value is the
  corresponding value. The value for options that do not require one is
  ignored and can be set to \code{""}. Some of the available options
  are:
  
  \describe{

    \item{h}{help, show all available options.}
    \item{nb}{do not write Byte Order Mark in UTF8 output.}

    \item{verbose}{print intermediate output.}
    \item{debug}{print even more intermediate output.}
  }

  
}
\value{
  The function is used for the side effect of creating a file in the
  requested format. It returns a list, currently containing the
  following components:

  \item{infile}{name of the input file,}
  \item{outfile}{name of the output file,}
  \item{nref_in}{number of references read from the input file,}
  \item{nref_out}{number of references written to the output file.}
  
}
\author{Georgi N. Boshnakov}
%\note{
%%%  ~~further notes~~
%}



%% ~Make other sections like Warning with \section{Warning }{....} ~
\references{
  Chris Putnam, Library \code{bibutils},
  \url{https://sourceforge.net/projects/bibutils/}.
}
%\seealso{
%%% ~~objects to See Also as \code{\link{help}}, ~~~
%}
\examples{
fn_biblatex <- system.file("bib", "ex0.biblatex",  package = "rbibutils")
fn_biblatex
## file.show(fn_biblatex)

## convert a biblatex file to xml
modl <- tempfile(fileext = ".xml")
bibConvert(infile = fn_biblatex, outfile = modl, informat = "biblatex", outformat = "xml")
## file.show(modl)

## convert a biblatex file to bibtex
bib <- tempfile(fileext = ".bib")
bib2 <- tempfile(fileext = ".bib")
bibConvert(infile = fn_biblatex, outfile = bib, informat = "biblatex", outformat = "bib")
## file.show(bib)

## convert a biblatex file to bibentry
rds <- tempfile(fileext = ".rds")
fn_biblatex
rds
be <- bibConvert(fn_biblatex, rds, "biblatex", "bibentry")
bea <- bibConvert(fn_biblatex, rds, "biblatex") # same
readRDS(rds)

## convert to R source file
r <- tempfile(fileext = ".R")
bibConvert(fn_biblatex, r, "biblatex")
file.show(r)
cat(readLines(r), sep = "\n")

fn_cyr_utf8 <- system.file("bib", "cyr_utf8.bib",  package = "rbibutils")

## Can't have files with different encodings in the package, so below
## first convert a UTF-8 file to something else.
##
## input here contains cyrillic (UTF-8) output to Windows Cyrillic,
## notice the "no_latex" option
a <- bibConvert(fn_cyr_utf8, bib, encoding = c("utf8", "cp1251"), tex = "no_latex")

## now take the bib file and convert it to UTF-8
bibConvert(bib, bib2, encoding = c("cp1251", "utf8"))

## Latin-1 example: Author and Title fileds contain Latin-1 accented
##   characters, not real names. As above, the file is in UTF-8
fn_latin1_utf8  <- system.file("bib", "latin1accents_utf8.bib", package = "rbibutils")
## convert to Latin-1, by default the accents are converted to TeX combinations:
b <- bibConvert(fn_latin1_utf8, bib , encoding = c("utf8", "latin1"))
cat(readLines(bib), sep = "\n")
## use "no_latex" option to keep them Latin1:
c <- bibConvert(fn_latin1_utf8, bib , encoding = c("utf8", "latin1"), tex = "no_latex")
## this will show properly in Latin-1 locale (or suitable text editor):
##cat(readLines(bib), sep = "\n")


unlink(c(modl, bib, bib2, r, rds))
}
\keyword{documentation}
% use one of  RShowDoc("KEYWORDS")
