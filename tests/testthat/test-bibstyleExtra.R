test_that("register_JSSextra is ok", {
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

    register_JSSextra(reset = TRUE)
    
    register_JSSextra(TRUE)  # this makes it default
    expect_equal(tools::getBibstyle(), "JSSextra")

})    


test_that("bibstyleExtra is ok", {
    expect_true(TRUE)
    bibdir <- system.file("bib", package = "rbibutils")
    
    ## standard bibtypes; also has macros and crossreferences
    xampl <- readBib(file.path(bibdir, "xampl_modified.bib"), direct = TRUE)

    ## non-standard bibtypes
    online <- readBib(file.path(bibdir, "superb_n15.bib"), direct = TRUE)
    Periodical <- readBib(file.path(bibdir, "texjourn001.bib"), direct = TRUE)

    ## c()
    combo2 <- c(online, Periodical)
    combo <- c(xampl, online, Periodical)
    
    ## print(), one reference, non-standard
    print(online)
    print(Periodical) # style = "latex"
    print(Periodical, style = "R")
    print(Periodical, style = "bibtex")

    print(combo2) ## more references, all non-standard
    print(combo2, style = "R")
    print(combo2, style = "bibtex")

    print(combo) ## more references, standard and non-standard
    print(combo, style = "R")
    print(combo, style = "bibtex")

    ## toRd
    toRd(Periodical)
    toRd(combo2) 
    toRd(combo) 

    ## '[<' and '[<-'
    Periodical_mod <- Periodical
    Periodical_mod$bibtype
    Periodical_mod$truebibtype

    Periodical_mod$bibtype <- "kikiriki"

    Periodical_mod$truebibtype <- "kikiriki"
    Periodical_mod$truebibtype
    
    
})
