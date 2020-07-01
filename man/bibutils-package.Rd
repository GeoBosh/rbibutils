\name{bibutils-package}
\alias{bibutils-package}
\alias{bibutils}
\docType{package}
\title{
\packageTitle{bibutils}
}
\description{
\packageDescription{bibutils}
}
\details{

The DESCRIPTION file:
\packageDESCRIPTION{bibutils}
\packageIndices{bibutils}

  Package \pkg{bibutils} provides an \R port of the \code{bibutils}
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
  
  This package will be kept lean. The only external dependency is
  package \pkg{xml2} and it is necessary only for conversion to Bibentry
  and xml manipulation, so could be moved to the suggested packages in a
  production release. Similar notes hold for \pkg{util} and \code{tools}.

}
\author{
\packageAuthor{bibutils}

Maintainer: \packageMaintainer{bibutils}
}
\references{
  Chris Putnam, Library \code{bibutils},
  \url{https://sourceforge.net/projects/bibutils/}.
}
\keyword{ package }
%\seealso{
%%~~ \code{\link[<pkg>:<pkg>-package]{<pkg>}} ~~
%}
%\examples{
%}
