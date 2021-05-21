if(require("testthat")) {
    library(testthat)
    library(rbibutils)
    test_check("rbibutils")
} else
    warning("package 'testthat' required for 'rbibutils\'' tests")

## temporary for the Windows check problem:
bibdir <- system.file("bib", package = "rbibutils")
fn  <- file.path(bibdir, "litprog280no_macros.bib")
if(!file.exists(fn))
    warning("file ", fn, "doesn't exist!")
