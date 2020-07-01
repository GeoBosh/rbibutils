\name{bibConvert}
\alias{bibConvert}
\title{Convert between bibliography formats}
\description{

  Read a bibliography file in one of the supported formats, convert it
  to annother format, and write it to a file.

}
\usage{
bibConvert(infile, outfile, informat, outformat)
}
\arguments{
  \item{infile}{input file, a character string.}
  \item{outfile}{output file, a character string.}
  \item{informat}{input format, see Details.}
  \item{outformat}{output format, see Details.}
}
\details{

  Arguments \code{informat} and \code{outformat} can usually be omitted,
  since \code{bibConvert} infers them from the extensions of the names
  of the input and output files.
  However, there is ambiguity for the extension \code{"bib"}, since it
  is used for Bibtex and Biblatex entries. For this extension, the
  default for both , \code{informat} and \code{outformat}, is
  \code{"bibtex"}.

  Package \pkg{bibutils} supports format \code{"bibentry"}, in addition to the
  formats supported by the bibutil library. On output, \code{"bibentry"}
  is saved using \code{saveRDS} with default extension \code{"rds"}.

  For complete list of supported formats, see the documentation of the
  original bibutils library.

  It should be remembered that there may be loss of information when
  converting from one format to another.
  
}
\value{
  The function is used for the side effect of creating a file in the
  requested format.
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
fn_biblatex <- system.file("bib", "ex0.biblatex",  package = "bibutils")
fn_biblatex
## file.show(fn_biblatex)

## convert a biblatex file to xml
modl <- tempfile(fileext = ".xml")
bibConvert(infile = fn_biblatex, outfile = modl, informat = "biblatex", outformat = "xml")
## file.show(modl)

## convert a biblatex file to bibtex
bib <- tempfile(fileext = ".bib")
bibConvert(infile = fn_biblatex, outfile = bib, informat = "biblatex", outformat = "bib")
## file.show(bib)

## convert a biblatex file to bibentry
rds <- tempfile(fileext = ".rds")
fn_biblatex
rds
be <- bibConvert(fn_biblatex, rds, "biblatex", "bibentry")
bea <- bibConvert(fn_biblatex, rds, "biblatex") # same
readRDS(rds)

unlink(modl)
unlink(bib)
}
\keyword{documentation}
% use one of  RShowDoc("KEYWORDS")
