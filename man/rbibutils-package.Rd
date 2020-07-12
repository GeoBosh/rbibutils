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
  programs. The main function, \code{bibConvert}, offers all conversions
  between bibliography formats supported by library \code{bibutils}.  In
  addition, package \pkg{rbibutils} converts to and from \R's
  \code{bibentry} Bibtex-based bibliography format.

  The core functionality is provided by the \code{bibutils} programs
  which convert between various bibliography formats using a common MODS
  XML intermediate format, see the source cited below.
  
  Currently we provide the function \code{\link{bibConvert}}
  for conversion between supported bibliography formats. For complete
  list of formats supported by the package, see the documentation of
  the original bibutils library.

  Further functionality may be provided in future releases, in
  particular, the underlying C functions could be exposed to package
  authors. Further R wrappers may be added, as well. However, the scope
  of the package will remain conversion between formats based on
  \verb{bibutils}. \pkg{rbibutils} can be used as an alternative to
  package \pkg{bibtex} (Francois 2020). For bibliography management, see
  package \pkg{RefManager} (McLean 2017). For citations in R
  documentation (Rd or roxygen2) see package \pkg{Rdpack} (Boshnakov
  2020).
  
  The only external dependency (import) is package \pkg{xml2} and
  conversion between formats not involving bibentry do not need it, so
  it could be made \dQuote{suggested} if there is a strong use case for
  that.

}

\author{Georgi N. Boshnakov (R code and R port of bibutils),
  Chris Putnam (author of bibutils library)
}
  
% Maintainer: Georgi N. Boshnakov


\note{

  The \code{bibutils} library is included in a number of software
  packages. Thesinclude include pandocand a library for
  Haskell. Executable programs for conversion are available for Linux
  distributions but seem not easily available for Windows. Executable
  and libraries can also be generated out-of-the-box from the
  \code{bibutils} disribution (on Windows under MSYS).

}

\references{

Boshnakov GN (2020). “Rdpack: Update and Manipulate Rd Documentation
Objects.” \doi{10.5281/zenodo.3925612} (URL:
\url{https://doi.org/10.5281/zenodo.3925612}), R package version 1.0.0.

Francois R. (2020). bibtex: Bibtex Parser. R package version
0.4.2.2. \url{https://CRAN.R-project.org/package=bibtex}.

McLean MW (2017). “RefManageR: Import and Manage BibTeX and BibLaTeX
References in R.” _The Journal of Open Source Software_.
\doi{10.21105/joss.00338} (URL: \url{https://doi.org/10.21105/joss.00338}).

Putnam C. (2003--2020), Library \code{bibutils},
  \url{https://sourceforge.net/projects/bibutils/}.

}

\keyword{ package }
\seealso{
  \code{\link{bibConvert} for further details and examples}
}
%\examples{
%}
