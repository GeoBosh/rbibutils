test_that("bibentryExtra", {
    ## "online" bib entry
    path_WuertzEtalGarch <- system.file("bib", "WuertzEtalGarch.bib", package = "rbibutils")
    bib_WuertzEtalGarch <- readBib(path_WuertzEtalGarch, direct = TRUE)
    
    bib_WuertzEtalGarch
    print(bib_WuertzEtalGarch, style = "R")
    print(bib_WuertzEtalGarch, style = "latex")
    print(bib_WuertzEtalGarch, style = "bibtex")

    bib_WuertzEtalGarch$key
    bib_WuertzEtalGarch[1, c("title", "year")]
    
    ## example from ?bibentry
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

    ## example from ?bibentry, with bibentry() replaced by bibentryExtra()
    brefdirect <- c(
        bibentryExtra(
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
     
        bibentryExtra(
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

    ## bibentryExtra -
    ssss <- format(bib_WuertzEtalGarch, style = "R")
    ssssA <- sub("^bibentry", "bibentryExtra", ssss)
    bib_eval_ssss <- eval(parse(text = ssssA))

    expect_error(as.bibentryExtra("kiki")) # can't convert a string.
    as.bibentryExtra(bibentry())
    
    brefExtra <- as.bibentryExtra(bref)
    bib_mbo <- c(brefExtra, bib_WuertzEtalGarch) # contains non-standard entry ("online")

    invisible(format(bib_mbo))

    bib_mbo[[3]]
    bib_mbo[[3, c("author", "title", "year")]]

    bib_mbo[2:3, ]
    bib_mbo[3, ]
    
              
    ## subset as list
    expect_error(brefExtra[[1:2, "title"]],
                 "length of i should be 1 when j is not missing or omitted")
    expect_equal(brefExtra[[1, "title"]], brefExtra[[list(1, "title")]])
    brefExtra[[1, "title", drop = FALSE]]
    brefExtra[[1, c("title", "author")]]
    brefExtra[[1, c("title", "key")]] # there is no 'key' field in the object
    brefExtra[[1, c("key")]]  

    ## subset as bibentryExtra
    brefExtra[1, "title"]
    brefExtra[1, "title", drop = FALSE]
    brefExtra[1, c("title", "author")]
    brefExtra_ta <- brefExtra[1:2, c("title", "author")]
    brefExtra[ , c("title", "author")] # missing i
    brefExtra[1, c("title", "key")]
    brefExtra[1, c("key")]
    drop_empty_bib_entries(brefExtra[1, c("key")])

    writeBibentry(brefExtra)
    writeBibentry(brefExtra, style = "Rstyle") # same
    writeBibentry(brefExtra, style = "loose")

    tmpfile <- tempfile()
    on.exit(unlink(tmpfile))
    
    writeBibentry(brefExtra, file = tmpfile, style = "Rstyle") ## to a file
    
    tt <- brefExtra["boot-book", "title"]
    
    b1 <- brefExtra

    expect_equal(b1$key, list("boot-package", "boot-book"))
    expect_equal(b1$bibtype, list("Manual", "Book"))
    b1$title
    b1$author

    b1$key <- list("package", "book")


    b1 <- brefExtra
    format(b1)
    #expect_output(print(b1))

    expect_error(b1[[list(1:2, "*")]])
    
    b1[[list(1, "*")]] <- list(title = "New title")
    expect_equal(b1[[1, "title", drop = TRUE]], c(title = "New title"))

    b1[[list(1, "title")]] <- list(title = "New title A")

    expect_equal(all.equal(b1[[1, "title"]], "New title A"), "names for target but not for current")

    
    ## expect_error(b1[[1]] <- list(title = "New title"),  # or:  c(title = "New title")
    ##              "when 'value' is a list, 'i' should be a list of length 2")

    b1[[list(1, "year")]] <- list(title = "New title A") ## no change, 'year' is not in 'value'
    b1[[list(1, "year")]] <- list(title = "New title A", year = NULL) ## removes 'year'
    
    b1[[2]] <- bibentry(bibtype = "Misc", title = "Dummy title", author = "A A Dummy", organization = "none")

    format(brefExtra)
    tt <- format(brefExtra, style = "R")
    
    expect_output(print(brefExtra, style = "R"))

    expect_output(print(b1, style = "latex"))

})
