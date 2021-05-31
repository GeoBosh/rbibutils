test_that("bibstyleExtra is ok", {
    ostyle <- tools::getBibstyle()
    on.exit(tools::bibstyle(ostyle, .default = TRUE))
    
    tools::bibstyle("JSS", .default = TRUE)
    curstyle <- tools::getBibstyle()
    expect_equal(tools::getBibstyle(), "JSS")
    ## tools::getBibstyle(TRUE)  # all styles, currently "JSS" only

    register_JSSextra()
    if(curstyle == "JSS")
        expect_equal(tools::getBibstyle(), curstyle)
    
    expect_true( "JSSextra" %in% tools::getBibstyle(TRUE))
    ##tools::getBibstyle()      # ... but not default

    register_JSSextra(TRUE)  # this makes it default
    expect_equal(tools::getBibstyle(), "JSSextra")

})    
