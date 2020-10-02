\name{readBib}
\alias{readBib}
\title{Import bibtex files}
\description{Import bibtex files.}
\usage{
readBib(file, encoding)
}
\arguments{
  \item{file}{name or path to the file, a character string.}
  \item{encoding}{the encoding of \code{file}, a character string.}
}
\details{

  Reads a bibtex file and converts the result to \code{bibentry}.

}
\value{
  a \code{bibentry} object
}
% \references{
% %% ~put references to the literature/web site here ~
% }
\author{Georgi N. Boshnakov}
\note{
  This function is experimental and should be considered internal.
}
%% ~Make other sections like Warning with \section{Warning }{....} ~
\seealso{
  \code{\link{bibConvert}}
}
% \examples{
% }
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
% \keyword{ ~kwd1 }
% \keyword{ ~kwd2 }
% Use only one keyword per line.
% For non-standard keywords, use \concept instead of \keyword:
% \concept{ ~cpt1 }
% \concept{ ~cpt2 }
% Use only one concept per line.
\keyword{ internal }
