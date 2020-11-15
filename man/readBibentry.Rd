\name{readBibentry}
\alias{readBibentry}
\alias{writeBibentry}
\title{Read and write bibentry files}
\description{Read and write bibentry files.}
\usage{
readBibentry(file)

writeBibentry(be, file, style = c("Rstyle", "loose"))
}
\arguments{
  \item{be}{a bibentry object.}
  \item{file}{filename, a character string.}
  \item{style}{
    if \code{"Rstyle"} (default), wrap in \code{c()}, otherwise don't
    wrap and don't put commas between the entries, see Details section.
  }
}
\details{

  These functions read/write \code{bibentry} objects from/to R source
  files. Two styles are supported. \code{"Rstyle"} is the format used by
  \code{print(be, style = "R")}, which writes the \code{bibentry} calls
  as a comma separated sequence wrapped in \code{c()}. Style
  \code{"loose"} writes the entries without separators and no wrapping.

  \code{writeBibentry} writes the object to the specified file in the
  requested style (default is \code{"Rstyle"}).

  \code{readBibentry} reads the file and creates a \code{bibentry}
  object. It doesn't have argument for style, since that is inferred
  from the contents of the file. 

  \code{bibentry()} calls that throw errors are not included in the
  returned object. The errors are intercepted and converted to warnings,
  identifying the corresponding \code{bibentry()} calls by their keys,
  if present (otherwise the text of the whole bibentry is shown).
  
}
\value{
  for \code{writeBibentry}, \code{NULL} (invisibly)

  for \code{readBibentry}, a \code{bibentry} object with the keys as
  names
}
%\references{
%%% ~put references to the literature/web site here ~
%}
\author{Georgi n. Boshnakov}
%\note{
%%%  ~~further notes~~
%}
%
%%% ~Make other sections like Warning with \section{Warning }{....} ~
%
\seealso{
  \code{\link{readBib}} and \code{\link{writeBib}} for reading/writing
  bib files,
  
  \code{\link{bibConvert}}
}
\examples{
bibs <- readBib(system.file("REFERENCES.bib", package = "rbibutils"),
                encoding = "UTF-8")
fn <- tempfile(fileext = ".bib")

writeBibentry(bibs, file = fn) # style = "Rstyle" (default)
cat(readLines(fn), sep = "\n")

writeBibentry(bibs, file = fn, style = "loose")
cat(readLines(fn), sep = "\n")

unlink(fn)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
% \keyword{ ~kwd1 }
% \keyword{ ~kwd2 }
% Use only one keyword per line.
% For non-standard keywords, use \concept instead of \keyword:
% \concept{ ~cpt1 }
% \concept{ ~cpt2 }
% Use only one concept per line.


