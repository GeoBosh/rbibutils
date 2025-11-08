# Create and register bibstyle JSSextra

Create and register bibstyle JSSextra.

## Usage

``` r
register_JSSextra(make_default = FALSE, reset = FALSE, parent_style = "JSS")
```

## Arguments

- make_default:

  if `TRUE` make `"JSSextra"` default.

- reset:

  recreate bibstyle `"JSSextra"`.

- parent_style:

  the style from which to derive `"JSSextra"`.

## Details

`register_JSSextra` creates style `"JSSextra"` and registers it for use
in the current R session. This means that it can be specified for
functions which accept a bibstyle argument, most notably printing
objects from class `"bibentry"` and `"bibentryExtra"`. In normal use
`register_JSSextra` is called once in a session.

Functions accepting a bibstyle argument use a default style if such an
argument is not provided. In most cases it is `"JSS"`.

Using [`tools::bibstyle()`](https://rdrr.io/r/tools/bibstyle.html) the
default style can be changed at any time to any of the styles currently
registered in the session. A list of these styles can be obtained with
`tools::getBibstyle(TRUE)`. The currently default style can be seen with
[`tools::getBibstyle()`](https://rdrr.io/r/tools/bibstyle.html). As a
convenience `register_JSSextra(TRUE)` makes and registers `"JSSextra"`
as the default style.

The remaining arguments should rarely be needed in normal circumstances.

`register_JSSextra` stores the bibstyle object it creates and just uses
it when called again. `reset = TRUE` can be used to force a fresh copy
of `"JSSextra"` to be created.

By default `"JSSextra"` is derived from `"JSS"`. To base it on a
different style, use argument `parent_style`.

## Value

`register_JSSextra` is used mainly for the side effect of registering
and setting the style as default. It returns the created style (an
environment) but it can be discarded.

## Author

Georgi N. Boshnakov

## Examples

``` r
## current default style
tools::getBibstyle()
#> [1] "JSSextra"
tools::getBibstyle(TRUE)  # all styles, currently "JSS" only
#> [1] "JSS"               "JSSextra"          "JSSextraLongNames"

register_JSSextra()       # register "JSSextra"
#> Warning: No longer needed. to make JSSextra default, use tools::bibentry("JSSextra", .default = TRUE)
#> <environment: 0x5645aaca29c0>
tools::getBibstyle(TRUE)  # now it is available
#> [1] "JSS"               "JSSextra"          "JSSextraLongNames"
tools::getBibstyle()      # ... but not default
#> [1] "JSSextra"

register_JSSextra(TRUE)  # this makes it default
#> Warning: No longer needed. to make JSSextra default, use tools::bibentry("JSSextra", .default = TRUE)
#> <environment: 0x5645aaca29c0>
tools::getBibstyle()
#> [1] "JSSextra"

## setting default style with bibstyle():
tools::bibstyle("JSS", .default = TRUE)
#> <environment: 0x5645a9e397b0>
tools::getBibstyle()
#> [1] "JSS"
```
