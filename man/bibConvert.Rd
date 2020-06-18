\name{bibConvert}
\alias{bibConvert}
\title{Convert between bibliography formats}
\description{Convert between bibliography formats.}
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

  For complete list of formats supported by the package, see the
  documentation of the original bibutils library.

  Note that bibutils knows two formats for bib files: BibTeX and
  BibLaTeX and hence can be used to nvert between them.

}
\value{
  The functiion is used for the side effect of creating a file in the
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
fn_biblatex <- system.file("inst", "bib", "ex0.biblatex",  package = "bibutils")
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

unlink(modl)
unlink(bib)
}
%\keyword{ ~kwd1 }
% use one of  RShowDoc("KEYWORDS")
