\name{rbibutils-package}
\alias{rbibutils-package}
\alias{rbibutils}
\docType{package}
\title{
\packageTitle{rbibutils}
}
\description{
\packageDescription{rbibutils}
}
\details{

  Package \pkg{rbibutils} provides an \R port of the \code{bibutils}
  programs and functions to convert to \R's \code{bibentry} Bibtex-based
  bibliography format.

  The core functionality is provided by the \code{bibutils} programs
  which convert between various bibliography formats using a common
  intermediate MODS XML format, see the source cited below.
  The \code{bibutils} library is included in a number of software
  packages, including pandoc. Executable programs for conversion are
  available for Linux distributions but are not easily available for
  Windows. Executable and libraries can also be generated out-of-the-box
  from the \code{bibutils} disribution (on Windows under MSYS).

  Currently we provide the function \code{\link{bibConvert}}
  for conversion between supported bibliography formats. For complete
  list of formats supported by the package, see the documentation of
  the original bibutils library.

  Further functiionality may be provided in future releases, in
  particular, the underlying C functions could be exposed to package
  authors. Further R wrappers may be added, as well. 
  
  The only external dependency (import) is package \pkg{xml2} and
  conversion between formats not involving bibentry do not need it, so
  it could be made \dQuote{suggested} if there is a strong use case for
  that.

}

\author{Georgi N. Boshnakov (R code and R port of bibutils),
  Chris Putnam (author of bibutils library)
}
  
% Maintainer: Georgi N. Boshnakov

\references{
  Chris Putnam, Library \code{bibutils},
  \url{https://sourceforge.net/projects/bibutils/}.
}
\keyword{ package }
\seealso{
  \code{\link{bibConvert}}
}
%\examples{
%}
