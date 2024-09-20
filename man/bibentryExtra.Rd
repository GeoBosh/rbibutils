\name{bibentryExtra}
\alias{bibentryExtra}

\alias{as.bibentryExtra}

\alias{names.bibentryExtra}
\alias{names}

\alias{[[.bibentryExtra}
\alias{[.bibentryExtra}

\alias{$<-.bibentryExtra}
\alias{[[<-.bibentryExtra}


\title{Work with 'bibentryExtra' objects}

\description{

  Objects from class \code{"bibentryExtra"} represent a collection of
  bibliographic references. This page documents functions to create such
  objects or convert other compatible objects to \code{"bibentryExtra"},
  as well as methods for subsetting and replacing parts of them.

}

\usage{
bibentryExtra(bibtype = NULL, \dots)

as.bibentryExtra(x, \dots)

\method{[[}{bibentryExtra}(x, i, j, drop = TRUE)

\method{[[}{bibentryExtra}(x, i) <- value

\method{[}{bibentryExtra}(x, i, j, drop = TRUE)

\method{$}{bibentryExtra}(x, name) <- value
}

% \method{[<-}{bibentryExtra}(x, i, value) % no such method, yet
%                                          % neither does 'bibentry'
%
%% [.bibentry and [[.bibentry are the same function:
%
% > identical(utils:::`[.bibentry`, utils:::`[[.bibentry`)
% [1] TRUE

% > utils:::`[.<-bibentry`
% Error: object '[.<-bibentry' not found
% > utils:::`[[.<-bibentry`
% Error: object '[[.<-bibentry' not found

% > utils:::`$.bibentry`
% function (x, name) 
% {
%     if (!length(x)) 
%         return(NULL)
%     is_attribute <- name %in% bibentry_attribute_names
%     rval <- if (is_attribute) 
%         lapply(unclass(x), attr, name)
%     else lapply(unclass(x), `[[`, tolower(name))
%     if (length(rval) == 1L) 
%         rval <- rval[[1L]]
%     rval
% }
% <bytecode: 0x56265b3ceea8>
% <environment: namespace:utils>
% > utils:::`$<-.bibentry`
% function (x, name, value) 
% {
%     is_attribute <- name %in% bibentry_attribute_names
%     x <- unclass(x)
%     if (!is_attribute) 
%         name <- tolower(name)
%     value <- rep_len(.listify(value), length(x))
%     if (name == "bibtype") {
%         stopifnot(all(lengths(value) == 1L))
%         BibTeX_names <- names(tools:::BibTeX_entry_field_db)
%         value <- unlist(value)
%         pos <- match(tolower(value), tolower(BibTeX_names))
%         if (anyNA(pos)) 
%             stop(gettextf("%s has to be one of %s", sQuote("bibtype"), 
%                 paste(BibTeX_names, collapse = ", ")), domain = NA)
%         value <- as.list(BibTeX_names[pos])
%     }
%     for (i in seq_along(x)) {
%         if (is_attribute) {
%             attr(x[[i]], name) <- if (is.null(value[[i]])) 
%                 NULL
%             else paste(value[[i]])
%         }
%         else {
%             x[[i]][[name]] <- if (is.null(value[[i]])) 
%                 NULL
%             else {
%                 if (name %in% c("author", "editor")) 
%                   as.person(value[[i]])
%                 else paste(value[[i]])
%             }
%         }
%     }
%     for (i in seq_along(x)) .bibentry_check_bibentry1(x[[i]])
%     .bibentry(x)
% }
% <bytecode: 0x56265e3fdb68>
% <environment: namespace:utils>
% > utils:::.listify
% function (x) 
% if (inherits(x, "list")) x else list(x)
% <environment: namespace:utils>


\arguments{
  \item{bibtype}{
      
    a character string specifying the type of the bib entry. Can also be
    a character vector to create an object containing more than one
    entry, see \code{\link{bibentry}}.

  }
  \item{x}{

    for \code{bibentryExtra}, an object to be converted; otherwise an
    object from class \code{"bibentryExtra"}.

  }
  \item{i}{

    the bib entry to extract or assign to, a character string (the key),
    a singe integer number (position), or a list of length 2.  For the
    extractor, \code{"[["}, \code{i} can also be of length more than 1
    (character or integer) when \code{j} is missing completely. See
    section \sQuote{Details} for complete details.

  }

  \item{j}{

    field(s) to extract, a character vector or missing, see section
    \sQuote{Details}.

  }
  \item{name}{
    field to extract.
  }
  \item{drop}{

    if \code{TRUE}, each entry in the returned list will contain the
    attributes (e.g., \code{"bibtype"} and \code{"key"}).

  }
  \item{value}{

    value(s) to use for replacement, a list.
    For \code{"[<-"}, an object inheriting from \code{"bibentry"} (so,
    including \code{"bibentryExtra"}), containing exactly one bib
    item. Alternatively, a character vector or a list, see section
    \sQuote{Details}.

  }
  \item{...}{

    for \code{bibentryExtra}, any arguments that
    \code{\link{bibentry}} accepts.

  }
}

\details{

  \code{bibentryExtra} creates a \code{"bibentryExtra"} object.  It has
  the same arguments as \code{\link{bibentry}}, see its help page for
  full details. The main difference is that for \code{bibentryExtra}, the
  bibentry type, is not restricted to have values from the list of
  standard Bibtex types (which is the case for \code{bibtype}).

  \code{bibentryExtra} sets the \code{"names"} attribute to the keys of
  the bib entries in it. However, further changes in the names and/or
  the keys can make them different. If you want to keep the names always
    consistent with the keys, set \code{names(bee) <- NULL}. This will
  cause \code{names} to dynamically create collect the keys when called.

    
  \code{as.bibentryExtra} is a generic function for conversion of
  objects to class \code{"bibentryExtra"}, most notably from class
  \code{"bibentry"}.

    
  
  Details on the subsetting methods are given below. The main think that
  needs to be pointed out is that the bracket operators take as first
  argument the values of one or more keys, while for the dollar
  operators the argument is the name of a field (e.g., \code{journal}).
  This is convenient but can cause confusion, since usually \code{$} and
  \code{[[} are (mostly) equivalent. This can be avoided by using
  argument \code{j}, since in the (i,j) pair \code{i} is always a key
  (or keys) and \code{j} the names of fields.
    
  The subsetting methods aim to provide convenient access to components
  of \code{"bibentryExtra"} objects.  In comparison to the corresponding
  methods for \code{"bibenry"} objects, the methods for
  \code{"bibentryExtra"} provide some additional features. Most notably,
  some of them include argument \code{j}, see the details below.  If you
  want similar access for \code{"bibenry"} objects, just convert them to
  \code{"bibentryExtra"} using \code{as.bibentryExtra}.

  It is convenient to think of the \code{"bibentryExtra"} as a ragged
  array with \sQuote{rows} (the bib entries) and \sQuote{columns} (the
  fields in the items). This is a peculiar ragged array, where each bib
  item (row) may have a different collection of fields.

  Additional functionality for the subscript operators that admit
  argument \code{j} is provided for \code{"bibentryExtra"} objects.  We
  will say that argument \code{j} \emph{is missing completely} if it is
  missing and there is no placeholder for it in the call. For example
  \code{be[i]} and \code{be[i, drop = FALSE]} are examples when \code{j}
  is missing completely. On the other hand, \code{be[i, ]} and
  \code{be[i, , drop = FALSE]} are examples when \code{j} is missing,
  but not completely.

  When \code{j} is completely missing, the subscript operations for
  \code{"bibentryExtra"} objects work exactly as for \code{bibentry}
  objects (they don't have methods that use \code{j}).
  Alternatively, rather than use both \code{i} and \code{j}, one can set
  \code{i} to be a list of length 2, whose two components stand for
  \code{i} and \code{j}, respectively. The latter syntax can be used also
  for the methods that don't have \code{j} as an argument
  (but do have \code{i}).


  \subsection{The methods for the dollar operator}{

      \code{"$"} extracts the specified field. \code{"bibentryExtra"}
      inherits the \code{"bibentry"} method for \code{"$"}.  The result is
      a list with one component for each bib entry.

      Note that the result is a list for several reasons. First, some
      fields may have more than one element. Second, some values are
      likely to be \code{NULL}. Third, some fields may be compound
      objects, e.g. \sQuote{Author}, which is from class \code{"person"}.
      However, if the the returned list has one element, the enlosing list
      is removed (similarly to matrices with one row).

      The \code{"bibentryExtra"} method for subset-assigment,
      \code{"$<-"}, assigns a new value to the specified field.  In most
      cases, \code{value} should be a list of the same length as \code{x}.
      Otherwise, it will be wrapped in a list If it is not a list, the
      result may be unexpected. For example, a character vector will
      replace the specified field in all bib entries.

      To change the values of the keys, specify field \code{"key"}. Again,
      remember that you get a list of all keys, unless there is only one
      bib entry in \code{x}. So, \code{value} should be a list of the same
      length.

  }
  
  \subsection{The double bracket extractor method}{

      The method for \code{"[["} accepts, unusually, two indices, \code{i}
      and \code{j}, reflecting the above interpretation.
      
      \code{i} is typically a character string or a positive integer
      number identifying the bib item to extract, while \code{j} is a
      character vector specifying the required fields of that bib
      item. The result is a list. If \code{drop} is \code{FALSE}, then
      the attributes of this list are set to those of the bib item
      (e.g., \code{bibtype} and \code{key}).
      
      If \code{j} is missing, all fields of the item are included in the
      result. However it depends on how \code{j} is missing. If it is
      missing as in \code{x[[i, ]]} or \code{x[[i, , drop = FALSE]]}, the
      result is as above and includes all entries in bib item \code{i}.
      
      Alternatively, \code{i} can be a list of length 2 and \code{j}
      omitted. This is equivalent to a call with \code{i = i[[1]]} and
      \code{j = i[[2]]}.
      
      If there is no redundant comma in the call, as in \code{x[[i]]} or
      \code{x[[i, drop = FALSE]]}, then \code{i} must be a single index
      value (positive integer number or character string).  The result is
      a \code{"bibentryExtra"} object. This case is compatible with the
      method for \code{"bibentry"} objects, which does not have argument
      \code{j}. Note though that for the \code{"bibentry"} method \code{i}
      can be of length more than 1.
  }

  \subsection{The double bracket assignment method}{

      The assignment version of \code{"[["} does not have argument
      \code{j}, so the two-element list form for \code{i} is used when
      fields are needed, see above.

      If \code{value} inherits from \code{"bibentry"} (in particular, it can
          be a \code{"bibentryExtra"} object), then \code{i} must be a single
      character string or a positive integer specifying the bibitem to
      replace with \code{value}. Notice that the new item may have a
      different key.

      Otherwise, \code{value} should be a named list and \code{i} a list
      of length 2.      
      In this case, \code{i[[1]]} should be a character string of a
      positive integer identifyng the bib item on which replacement will
      take place, while \code{i[[2]]} is a character vector specifying the
      fields to replace.  As a special case, \code{i[[2]]} can be the
      character string \code{"*"}, which specifies that all elements of
      \code{value} should be used.

      Usually \code{value} has names and these are interpreted as names of
      fields.  In this case, the fields specified by \code{i[[2]]} are
      replaced by the corresponding fields in \code{value}.

      If \code{value} has no names and \code{i[[2]]} is not equal to
      \code{"*"} (see above), \code{value} must have the same length as
      \code{i[[2]]} and its names are set to \code{i[[2]]}.

  }

  \subsection{The single bracket extractor method}{

      For \code{"["}, \code{i} is an index vector specifying which bib
      entries to extract. If \code{j} is missing completely, the bib
      entries are extracted and returned as a \code{"bibentryExtra"}
      object.

      If \code{j} is used, it is a character vector specifying which
      fields to keep. For example, this could be a list of all standard
      bibtex fields. Only fields from this list are kept. Note that this
      may leave some bib entries invalid (i.e., missing compulsory
                                          fields).

      Note that in any case the returned object has class
      \code{"bibentryExtra"}.

  }

  }

\value{

  for \code{as.bibentryExtra} and \code{bibentryExtra},
  a \code{"bibentryExtra"} object;

  for \code{"$"}, a list containing the requested fields with one list
  component for each key.  If only one key was specified, the outer list
  is dropped;

  for \code{"[["}, typically a list, but if argument \code{x} is
  missing completely, the result is a \code{"bibentryExtra"} object,
  as described in section \sQuote{The double bracket extractor
  method};

  for \code{"["}, a \code{"bibentryExtra"} object;

  for the assignment operators (\code{"[[<-"}, \code{"[<-"} and \code{"$<-"}),
  the modified object \code{x}.

  for \code{names}, always a character vector. If attribute \code{names}
  is \code{NULL}, the keys are put in a character vector and returned.
    
}

\author{Georgi N. Boshnakov}

\note{

  This is somewhat experimental but incompatible changes are unlikely.

}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{

  the vignette,
    
  \code{\link{readBib}} for importing bibtex files,

  \code{\link{readBibentry}} for importing from R source files
  containing \code{bibentry} expressions,
  
  \code{\link{charToBib}} for converting character vectors containing
  \code{bibentry} expressions.
    
}

\examples{
## example bib from ?bibentry
bref <- c(
    bibentry(
        bibtype = "Manual",
        title = "boot: Bootstrap R (S-PLUS) Functions",
        author = c(
            person("Angelo", "Canty", role = "aut",
                   comment = "S original"),
            person(c("Brian", "D."), "Ripley", role = c("aut", "trl", "cre"),
                   comment = "R port, author of parallel support",
                   email = "ripley@stats.ox.ac.uk")
        ),
        year = "2012",
        note = "R package version 1.3-4",
        url = "https://CRAN.R-project.org/package=boot",
        key = "boot-package"
    ),
 
    bibentry(
        bibtype = "Book",
        title = "Bootstrap Methods and Their Applications",
        author = as.person("Anthony C. Davison [aut], David V. Hinkley [aut]"),
        year = "1997",
        publisher = "Cambridge University Press",
        address = "Cambridge",
        isbn = "0-521-57391-2",
        url = "http://statwww.epfl.ch/davison/BMA/",
        key = "boot-book"
    )
  )

brefExtra <- as.bibentryExtra(bref)

## error: j is present, so i must have length 1:
## brefExtra[[1:2, "title"]]

## the returned list doesn't have attributes:
                                  brefExtra[[1, c("title", "author")]] # drop = TRUE by default

## now it does:
brefExtra[[1, "title", drop = FALSE]]
brefExtra[["boot-package", "title", drop = FALSE]]

brefExtra[["boot-book", ]]
brefExtra[["boot-book"]]


## assignment "[[<-"
b2 <- brefExtra
b2

## use all elements of 'value'
b2[[list(1, "*")]] <- list(title = "New title", note = "a new note")

## replace title
b2[[list(1, "title")]] <- list(title = "New title A")
b2

## no change,'year' is not in 'value'
b2[[list(1, "year")]] <- list(title = "New title A2") 
b2

## remove 'year'
b2[[list(1, "year")]] <- list(title = "New title A", year = NULL) ## removes 'year'
b2

## a bibentry 'value'
b2[[2]] <- bibentry(bibtype = "Misc", title = "Dummy title",
                    author = "A A Dummy", organization = "none")
b2
}

% TODO: edit this and set as example:
%
% > ebref$key <- c("Rpackage:boot", "DavisonHinkley1997")
% > ebref$key
% [[1]]
% [1] "Rpackage:boot"      "DavisonHinkley1997"
% 
% [[2]]
% [1] "Rpackage:boot"      "DavisonHinkley1997"
% 
% > ebref$key <- list("Rpackage:boot", "DavisonHinkley1997")
% > ebref$key
% [[1]]
% [1] "Rpackage:boot"
% 
% [[2]]
% [1] "DavisonHinkley1997"

