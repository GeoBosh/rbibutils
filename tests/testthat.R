if(require("testthat")) {
    library(testthat)
    library(rbibutils)
    test_check("rbibutils")
} else
    warning("package 'testthat' required for 'rbibutils\'' tests")
