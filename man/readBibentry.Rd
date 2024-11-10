\name{readBibentry}
\alias{readBibentry}

\alias{writeBibentry}


\concept{bibtex}
\concept{biblatex}
\concept{bibentry}


\title{Read and write bibentry files or read bibtex strings}
\description{Read and write bibentry files.}
\usage{
readBibentry(file, extra = FALSE, fbibentry = NULL)

writeBibentry(be, file = stdout(), style = c("Rstyle", "loose"))
}
\arguments{
  \item{be}{a bibentry object.}
  \item{file}{

    filename, a character string or a connection. For
    \code{readBibentry}, input from the console can be specified by
    \code{file = ""}.  The default for \code{writeBibentry} is
    \code{stdout} (effectively, to write on the screen).

  }
  \item{extra}{if \code{TRUE} allow non-standard bibtex types.}
  \item{style}{
    if \code{"Rstyle"} (default), wrap in \code{c()}, otherwise don't
    wrap and don't put commas between the entries, see Details section.
  }
  \item{fbibentry}{

    a function to use for generating bib objects. The default is
    \code{utils::bibentry()}.
    
  }
}
\details{

  These functions read/write \code{bibentry} objects from/to R source
  files. Two styles are supported. \code{"Rstyle"} is the format used by
  \code{print(be, style = "R")}, which writes the \code{bibentry} calls
  as a comma separated sequence wrapped in \code{c()} (i.e., the file
  contains a single R expression). Style \code{"loose"} writes the
  entries without separators and no wrapping.

  \code{writeBibentry} writes the object to the specified file in the
  requested style (default is \code{"Rstyle"}). The file is neatly
  formatted for humans to read and edit.

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
\author{Georgi N. Boshnakov}
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

  \code{\link{charToBib}} for reading from a character vector  
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
