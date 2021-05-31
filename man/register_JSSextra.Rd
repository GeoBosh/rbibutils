\name{register_JSSextra}
\alias{register_JSSextra}
\title{Create and register bibstyle JSSextra}
\description{Create and register bibstyle JSSextra.}
\usage{
register_JSSextra(make_default = FALSE, reset = FALSE, parent_style = "JSS")
}
\arguments{
  \item{make_default}{
    if \code{TRUE} make \code{"JSSextra"} default.
  }
  \item{reset}{
    recreate bibstyle \code{"JSSextra"}.
  }
  \item{parent_style}{
    the style from which to derive \code{"JSSextra"}.
  }
}
\details{

  \code{register_JSSextra} creates \code{"JSSextra"} and registers it
  for use in the current \R session. This means that it can be specified
  for functions which accept a bibstyle argument, most notably printing
  objects from class  \code{"bibentry"} and \code{"bibentryExtra"}.
  In normal use \code{register_JSSextra} is called once in a
  session. 

  Functions accepting a bibstyle argument use a default style if such an
  argument is not provided. In most cases it is \code{"JSS"}.

  The default style can be changed at any time to any of the styles
  currently registered in the session using
  \code{tools::bibstyle()}. A list of these styles can be obtained
  with \code{tools::getBibstyle(TRUE)}. The currently default style can
  be seen with \code{tools::getBibstyle()}.
  As a convenience \code{register_JSSextra(TRUE)} makes and registers
  \code{"JSSextra"} as the default style.

  The remaining arguments should rarely be needed in normal
  circumstances.

  \code{register_JSSextra} stores the bibstyle object it creates and
  just uses it when called again. \code{reset = TRUE} can be used to
  force a fresh copy of \code{"JSSextra"} to be created.

  By default \code{"JSSextra"} is derived from \code{"JSS"}. To base it
  on a different style, use argument \code{parent_style}.
  
}
\value{
  
  \code{register_JSSextra} is used mainly for the side effect of
  registering and setting the style as default.  It returns the created
  style (an environment) but it can be discarded.
}
%\references{
%%% ~put references to the literature/web site here ~
%}
\author{Georgi N. Boshnakov}
%\note{
%%%  ~~further notes~~
%}

%% ~Make other sections like Warning with \section{Warning }{....} ~

%\seealso{
%%% ~~objects to See Also as \code{\link{help}}, ~~~
%}
\examples{
## current default style
tools::getBibstyle()
tools::getBibstyle(TRUE)  # all styles, currently "JSS" only

register_JSSextra()       # register "JSSextra"
tools::getBibstyle(TRUE)  # now it is available
tools::getBibstyle()      # ... but not default

register_JSSextra(TRUE)  # this makes it default
tools::getBibstyle()

## setting default style with bibstyle():
tools::bibstyle("JSS", .default = TRUE)
tools::getBibstyle()
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
\keyword{documentation}
% \keyword{ ~kwd2 }
% Use only one keyword per line.
% For non-standard keywords, use \concept instead of \keyword:
% \concept{ ~cpt1 }
% \concept{ ~cpt2 }
% Use only one concept per line.
