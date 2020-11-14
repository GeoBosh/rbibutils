\name{readBib}
\alias{readBib}
\alias{writeBib}
\title{Read and write bibtex files}
\description{Read and write bibtex files.}
\usage{
readBib(file, encoding)

writeBib(object, con = stdout(), append = FALSE)
}
\arguments{
  \item{file}{name or path to the file, a character string.}
  \item{encoding}{the encoding of \code{file}, a character string.}
  \item{object}{a \code{bibentry} object.}
  \item{con}{filename (a character string) or a text connection}
  \item{append}{if \code{TRUE} append to the file.}
}
\details{

  \code{readBib} is wrapper around \code{bibConvert} for import of
  bibtex files into bibentry objects.

  \code{writeBib} writes a \code{bibentry} object to a bibtex file.

}
\value{
  for \code{readBib}, a \code{bibentry} object

  for \code{writeBib}, the \code{bibentry} object (invisibly)
}
% \references{
% %% ~put references to the literature/web site here ~
% }
\author{Georgi N. Boshnakov}
% \note{
%   This function is experimental and should be considered internal.
% }
%% ~Make other sections like Warning with \section{Warning }{....} ~
\seealso{
  \code{\link{readBibentry}} and \code{\link{writeBibentry}} for
  import/export to R code.
  
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
