\name{rbibutils_formats}
\alias{rbibutils_formats}

\docType{data}


\concept{bibliography formats}


\title{Supported bibliography formats}
\description{Supported bibliography formats in package rbibutils.}
\usage{rbibutils_formats}
\format{
  A data frame with 16 observations on the following 5 variables:
  \describe{
    \item{\code{Abbreviation}}{a character vector.}
    \item{\code{FileExt}}{a character vector.}
    \item{\code{Input}}{a logical vector.}
    \item{\code{Output}}{a logical vector.}
    \item{\code{Description}}{a character vector.}
  }
}
\details{

  Each row in \code{rbibutils_formats} gives information about a supported
  bibliography format in package \pkg{rbibutils}. 

  \code{Abbreviation} is the name to use in arguments \code{informat}
  and \code{outformat} in \code{\link{bibConvert}}.

  \code{FileExt} is the default extension used to infer the input/output
  format if the corresponding argument is missing.

  \code{Input} (\code{Output}) is \code{TRUE} if the format is supported
  for input (output).

  \code{Description} gives a brief description of the format.

  See \code{\link{bibConvert}}, section \dQuote{Supported formats}, for
  further details.
  
}
% \source{
% %%  ~~ reference to a publication or URL from which the data were obtained ~~
% }
% \references{
% %%  ~~ possibly secondary sources and usages ~~
% }
%\seealso{
%}
\examples{
rbibutils_formats
}
\keyword{datasets}
