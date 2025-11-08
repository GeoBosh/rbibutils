# Work with 'bibentryExtra' objects

Objects from class `"bibentryExtra"` represent a collection of
bibliographic references. This page documents functions to create such
objects or convert other compatible objects to `"bibentryExtra"`, as
well as methods for subsetting and replacing parts of them.

## Usage

``` r
bibentryExtra(bibtype = NULL, ...)

as.bibentryExtra(x, ...)

# S3 method for class 'bibentryExtra'
x[[i, j, drop = TRUE]]

# S3 method for class 'bibentryExtra'
x[[i]] <- value

# S3 method for class 'bibentryExtra'
x[i, j, drop = TRUE]

# S3 method for class 'bibentryExtra'
x$name <- value
```

## Arguments

- bibtype:

  a character string specifying the type of the bib entry. Can also be a
  character vector to create an object containing more than one entry,
  see [`bibentry`](https://rdrr.io/r/utils/bibentry.html).

- x:

  for `bibentryExtra`, an object to be converted; otherwise an object
  from class `"bibentryExtra"`.

- i:

  the bib entry to extract or assign to, a character string (the key), a
  singe integer number (position), or a list of length 2. For the
  extractor, `"[["`, `i` can also be of length more than 1 (character or
  integer) when `j` is missing completely. See section ‘Details’ for
  complete details.

- j:

  field(s) to extract, a character vector or missing, see section
  ‘Details’.

- name:

  field to extract.

- drop:

  if `TRUE`, each entry in the returned list will contain the attributes
  (e.g., `"bibtype"` and `"key"`).

- value:

  value(s) to use for replacement, a list. For `"[<-"`, an object
  inheriting from `"bibentry"` (so, including `"bibentryExtra"`),
  containing exactly one bib item. Alternatively, a character vector or
  a list, see section ‘Details’.

- ...:

  for `bibentryExtra`, any arguments that
  [`bibentry`](https://rdrr.io/r/utils/bibentry.html) accepts.

## Details

`bibentryExtra` creates a `"bibentryExtra"` object. It has the same
arguments as [`bibentry`](https://rdrr.io/r/utils/bibentry.html), see
its help page for full details. The main difference is that for
`bibentryExtra`, the bibentry type, is not restricted to have values
from the list of standard Bibtex types (which is the case for
`bibtype`).

`bibentryExtra` sets the `"names"` attribute to the keys of the bib
entries in it. However, further changes in the names and/or the keys can
make them different. If you want to keep the names always consistent
with the keys, set `names(bee) <- NULL`. This will cause `names` to
dynamically collect the keys when called.

`as.bibentryExtra` is a generic function for conversion of objects to
class `"bibentryExtra"`, most notably from class `"bibentry"`.

Details on the subsetting methods are given below. The main thing that
needs to be pointed out is that the bracket operators take as first
argument the values of one or more keys, while for the dollar operators
the argument is the name of a field (e.g., `journal`). This is
convenient but can cause confusion, since usually `$` and `[[` are
(mostly) equivalent. This can be avoided by using argument `j`, since in
the (i,j) pair `i` is always a key (or keys) and `j` the names of
fields.

The subsetting methods aim to provide convenient access to components of
`"bibentryExtra"` objects. In comparison to the corresponding methods
for `"bibenry"` objects, the methods for `"bibentryExtra"` provide some
additional features. Most notably, some of them include argument `j`,
see the details below. If you want similar access for `"bibenry"`
objects, just convert them to `"bibentryExtra"` using
`as.bibentryExtra`.

It is convenient to think of the `"bibentryExtra"` as a ragged array
with ‘rows’ (the bib entries) and ‘columns’ (the fields in the items).
This is a peculiar ragged array, where each bib item (row) may have a
different collection of fields.

Additional functionality for the subscript operators that admit argument
`j` is provided for `"bibentryExtra"` objects. We will say that argument
`j` *is missing completely* if it is missing and there is no placeholder
for it in the call. For example `be[i]` and `be[i, drop = FALSE]` are
examples when `j` is missing completely. On the other hand, `be[i, ]`
and `be[i, , drop = FALSE]` are examples when `j` is missing, but not
completely.

When `j` is completely missing, the subscript operations for
`"bibentryExtra"` objects work exactly as for `bibentry` objects (the
latter don't have methods that use `j`). Alternatively, rather than use
both `i` and `j`, one can set `i` to be a list of length 2, whose two
components stand for `i` and `j`, respectively. The latter syntax can be
used also for the methods that don't have `j` as an argument (but do
have `i`).

### The methods for the dollar operator

`"$"` extracts the specified field. `"bibentryExtra"` inherits the
`"bibentry"` method for `"$"`. The result is a list with one component
for each bib entry.

Note that the result is a list for several reasons. First, some fields
may have more than one element. Second, some values are likely to be
`NULL`. Third, some fields may be compound objects, e.g. ‘Author’, which
is from class `"person"`. However, if the returned list has one element,
the enlosing list is removed (similarly to matrices with one row).

The `"bibentryExtra"` method for subset-assigment, `"$<-"`, assigns a
new value to the specified field. In most cases, `value` should be a
list of the same length as `x`. Otherwise, it will be wrapped in a list
If it is not a list, the result may be unexpected. For example, a
character vector will replace the specified field in all bib entries.

To change the values of the keys, specify field `"key"`. Again, remember
that you get a list of all keys, unless there is only one bib entry in
`x`. So, `value` should be a list of the same length.

### The double bracket extractor method

The method for `"[["` accepts, unusually, two indices, `i` and `j`,
reflecting the above interpretation.

`i` is typically a character string or a positive integer number
identifying the bib item to extract, while `j` is a character vector
specifying the required fields of that bib item. The result is a list.
If `drop` is `FALSE`, then the attributes of this list are set to those
of the bib item (e.g., `bibtype` and `key`).

If `j` is missing, all fields of the item are included in the result.
However it depends on how `j` is missing. If it is missing as in
`x[[i, ]]` or `x[[i, , drop = FALSE]]`, the result is as above and
includes all entries in bib item `i`.

Alternatively, `i` can be a list of length 2 and `j` omitted. This is
equivalent to a call with `i = i[[1]]` and `j = i[[2]]`.

If `j` is missing completely (i.e., there is no redundant comma in the
call), as in `x[[i]]` or `x[[i, drop = FALSE]]`, then `i` must be a
single index value (positive integer number or character string). The
result is a `"bibentryExtra"` object. This case is compatible with the
method for `"bibentry"` objects, which does not have argument `j`. Note
though that for the `"bibentry"` method `i` can be of length more than
1.

### The double bracket assignment method

The assignment version of `"[["` does not have argument `j`, so the
two-element list form for `i` is used when fields are needed, see above.

If `value` inherits from `"bibentry"` (in particular, it can be a
`"bibentryExtra"` object), then `i` must be a single character string or
a positive integer specifying the bibitem to replace with `value`.
Notice that the new item may have a different key.

Otherwise, `value` should be a named list and `i` a list of length 2. In
this case, `i[[1]]` should be a character string of a positive integer
identifyng the bib item on which replacement will take place, while
`i[[2]]` is a character vector specifying the fields to replace. As a
special case, `i[[2]]` can be the character string `"*"`, which
specifies that all elements of `value` should be used.

Usually `value` has names and these are interpreted as names of fields.
In this case, the fields specified by `i[[2]]` are replaced by the
corresponding fields in `value`.

If `value` has no names and `i[[2]]` is not equal to `"*"` (see above),
`value` must have the same length as `i[[2]]` and its names are set to
`i[[2]]`.

### The single bracket extractor method

For `"["`, `i` is an index vector specifying which bib entries to
extract. If `j` is missing completely, the bib entries are extracted and
returned as a `"bibentryExtra"` object.

If `j` is used, it is a character vector specifying which fields to
keep. For example, this could be a list of all standard bibtex fields.
Only fields from this list are kept. Note that this may leave some bib
entries invalid (i.e., missing compulsory fields).

Note that in any case the returned object has class `"bibentryExtra"`.

## Value

for `as.bibentryExtra` and `bibentryExtra`, a `"bibentryExtra"` object;

for `"$"`, a list containing the requested fields with one list
component for each key. If only one key was specified, the outer list is
dropped;

for `"[["`, typically a list, but if argument `x` is missing completely,
the result is a `"bibentryExtra"` object, as described in section ‘The
double bracket extractor method’;

for `"["`, a `"bibentryExtra"` object;

for the assignment operators (`"[[<-"`, `"[<-"` and `"$<-"`), the
modified object `x`.

for `names`, always a character vector. If attribute `names` is `NULL`,
the keys are put in a character vector and returned.

## Author

Georgi N. Boshnakov

## Note

This is somewhat experimental but incompatible changes are unlikely.

## See also

the vignette,

[`readBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
for importing bibtex files,

[`readBibentry`](https://geobosh.github.io/rbibutils/reference/readBibentry.md)
for importing from R source files containing `bibentry` expressions,

[`charToBib`](https://geobosh.github.io/rbibutils/reference/readBib.md)
for converting character vectors containing `bibentry` expressions.

## Examples

``` r
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
#> $title
#> [1] "boot: Bootstrap R (S-PLUS) Functions"
#> 
#> $author
#> [1] "Angelo Canty [aut] (S original)"                                                             
#> [2] "Brian D. Ripley <ripley@stats.ox.ac.uk> [aut, trl, cre] (R port, author of parallel support)"
#> 

## now it does:
brefExtra[[1, "title", drop = FALSE]]
#> $title
#> [1] "boot: Bootstrap R (S-PLUS) Functions"
#> 
#> attr(,"bibtype")
#> [1] "Manual"
#> attr(,"key")
#> [1] "boot-package"
brefExtra[["boot-package", "title", drop = FALSE]]
#> $title
#> [1] "boot: Bootstrap R (S-PLUS) Functions"
#> 
#> attr(,"bibtype")
#> [1] "Manual"
#> attr(,"key")
#> [1] "boot-package"

brefExtra[["boot-book", ]]
#> $title
#> [1] "Bootstrap Methods and Their Applications"
#> 
#> $author
#> [1] "Anthony C. Davison [aut]" "David V. Hinkley [aut]"  
#> 
#> $year
#> [1] "1997"
#> 
#> $publisher
#> [1] "Cambridge University Press"
#> 
#> $address
#> [1] "Cambridge"
#> 
#> $isbn
#> [1] "0-521-57391-2"
#> 
#> $url
#> [1] "http://statwww.epfl.ch/davison/BMA/"
#> 
#> attr(,"bibtype")
#> [1] "Book"
#> attr(,"key")
#> [1] "boot-book"
brefExtra[["boot-book"]]
#> Davison AC, Hinkley DV (1997). _Bootstrap Methods and Their
#> Applications_. Cambridge University Press, Cambridge. ISBN
#> 0-521-57391-2, <http://statwww.epfl.ch/davison/BMA/>.


## assignment "[[<-"
b2 <- brefExtra
b2
#> Canty A, Ripley BD (2012). _boot: Bootstrap R (S-PLUS) Functions_. R
#> package version 1.3-4, <https://CRAN.R-project.org/package=boot>.
#> 
#> Davison AC, Hinkley DV (1997). _Bootstrap Methods and Their
#> Applications_. Cambridge University Press, Cambridge. ISBN
#> 0-521-57391-2, <http://statwww.epfl.ch/davison/BMA/>.

## use all elements of 'value'
b2[[list(1, "*")]] <- list(title = "New title", note = "a new note")

## replace title
b2[[list(1, "title")]] <- list(title = "New title A")
b2
#> Canty A, Ripley BD (2012). _New title A_. a new note,
#> <https://CRAN.R-project.org/package=boot>.
#> 
#> Davison AC, Hinkley DV (1997). _Bootstrap Methods and Their
#> Applications_. Cambridge University Press, Cambridge. ISBN
#> 0-521-57391-2, <http://statwww.epfl.ch/davison/BMA/>.

## no change,'year' is not in 'value'
b2[[list(1, "year")]] <- list(title = "New title A2") 
b2
#> Canty A, Ripley BD (2012). _New title A_. a new note,
#> <https://CRAN.R-project.org/package=boot>.
#> 
#> Davison AC, Hinkley DV (1997). _Bootstrap Methods and Their
#> Applications_. Cambridge University Press, Cambridge. ISBN
#> 0-521-57391-2, <http://statwww.epfl.ch/davison/BMA/>.

## remove 'year'
b2[[list(1, "year")]] <- list(title = "New title A", year = NULL) ## removes 'year'
b2
#> Canty A, Ripley BD (????). _New title A_. a new note,
#> <https://CRAN.R-project.org/package=boot>.
#> 
#> Davison AC, Hinkley DV (1997). _Bootstrap Methods and Their
#> Applications_. Cambridge University Press, Cambridge. ISBN
#> 0-521-57391-2, <http://statwww.epfl.ch/davison/BMA/>.

## a bibentry 'value'
b2[[2]] <- bibentry(bibtype = "Misc", title = "Dummy title",
                    author = "A A Dummy", organization = "none")
b2
#> Canty A, Ripley BD (????). _New title A_. a new note,
#> <https://CRAN.R-project.org/package=boot>.
#> 
#> Dummy AA (????). “Dummy title.”
```
