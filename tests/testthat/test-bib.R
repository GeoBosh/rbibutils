
test_that("bibRead works ok", {
    bibacc <- system.file("bib/latin1accents_utf8.bib", package = "rbibutils")
    
    expect_true(file.exists(bibacc))
    
    expect_known_value(
        readBib(bibacc, direct = TRUE, texChars = "keep"),
        "bibacc_1.rds", update = FALSE, version = 2)

    expect_known_value(
        readBib(bibacc, direct = TRUE, texChars = "export"),
        "bibacc_2.rds", update = FALSE )

    expect_known_value(
        readBib(bibacc, direct = TRUE, texChars = "convert"),
        "bibacc_3.rds", update = FALSE )

    nams <- c("author", "editor", "title", "publisher", "volume", "doi", "url", "year",
              "series", "booktitle", "pages", "alias", "annote", "keywords", "key")
    eaf_Grunert01 <- readBib(system.file("bib", "eaf_Grunert01.bib",
                                         package = "rbibutils"), direct = TRUE)
    fn_bib <- tempfile(fileext=".bib")
    on.exit(unlink(fn_bib))
    writeBib(eaf_Grunert01, fn_bib)
    
    expect_identical(sort(names(unclass(eaf_Grunert01)[[1]])), sort(nams))

    ## Uncomment this to create "eaf_Grunert01.rds" for the tests below.
    ## (then comment it out again after a run of devtools::test()!)
    ##
    ##    saveRDS(eaf_Grunert01, file = "eaf_Grunert01.rds", version = 2)
    
    old_eaf_Grunert01 <- readRDS("eaf_Grunert01.rds")
    
    identical(eaf_Grunert01$author   , old_eaf_Grunert01$author   )
    identical(eaf_Grunert01$editor   , old_eaf_Grunert01$editor   )
    identical(eaf_Grunert01$title    , old_eaf_Grunert01$title    )
    identical(eaf_Grunert01$publisher, old_eaf_Grunert01$publisher)
    identical(eaf_Grunert01$volume   , old_eaf_Grunert01$volume   )
    identical(eaf_Grunert01$doi      , old_eaf_Grunert01$doi      )
    identical(eaf_Grunert01$url      , old_eaf_Grunert01$url      )
    identical(eaf_Grunert01$year     , old_eaf_Grunert01$year     )
    identical(eaf_Grunert01$series   , old_eaf_Grunert01$series   )
    identical(eaf_Grunert01$booktitle, old_eaf_Grunert01$booktitle)
    identical(eaf_Grunert01$pages    , old_eaf_Grunert01$pages    )
    identical(eaf_Grunert01$alias    , old_eaf_Grunert01$alias    )
    identical(eaf_Grunert01$annote   , old_eaf_Grunert01$annote   )
    identical(eaf_Grunert01$keywords , old_eaf_Grunert01$keywords )
    identical(eaf_Grunert01$key      , old_eaf_Grunert01$key      )

    readBib(system.file("bib/litprog280.bib", package = "rbibutils"))
    litprog280 <- readBib(system.file("bib/litprog280.bib", package = "rbibutils"),
                          direct = TRUE)

    ## non-syntactic field
    expect_equal(litprog280$"issn-l", "0883-7252") 
    
    ## expanded abbreviation (in the bib file journal = j-J-APPL-ECONOMETRICS)
    expect_equal(litprog280$journal, "Journal of Applied Econometrics")

    ## string concatenation
    expect_equal(litprog280$month, "jan--feb")

    ## litprog <- readBib("/home/georgi/repos/private/rbibutils/data-raw/litprog_no_atpreamble.bib", direct = TRUE)

    texjourn001 <- system.file("bib", "texjourn001.bib", package = "rbibutils")
    obj1 <- readBib(texjourn001, direct = TRUE, extra = TRUE)
    expect_true(inherits(obj1, "bibentryExtra"))
    ## don't these need JSSextra loaded?
    expect_output(print(obj1))
    expect_output(print(obj1, style = "R"))
    expect_output(print(obj1, style = "bibtex"))
    expect_output(print(obj1, style = "latex"))
    expect_output(print(obj1, style = "text"))

    ## current default style
    #tools::getBibstyle()
    #tools::getBibstyle(TRUE)  # all styles, currently "JSS" only

    oldbibstyle <- tools::getBibstyle()

    register_JSSextra()       # register "JSSextra"
    expect_true("JSSextra" %in% tools::getBibstyle(TRUE))
    ## TODO: currently register_JSSextra() doesn't set the style as default.
    ##       the test below confirms that
    if(oldbibstyle != "JSSextra")
        expect_true(tools::getBibstyle() != "JSSextra")
    
    register_JSSextra(TRUE)  # this makes "JSSextra" default
    expect_equal(tools::getBibstyle(), "JSSextra")

    ## setting default style with bibstyle():
    tools::bibstyle("JSS", .default = TRUE)
    expect_equal(tools::getBibstyle(), "JSS")
    
    toRd(obj1)
    toRd(obj1, style = "JSSextra")

    obj2 <- obj1
    #expect_true(obj2[[1, "key"]] == "MAPLETECH")
    #expect_true(obj2[[1, "bibtype"]] == "MAPLETECH")

    ##toBibtex(obj1)

    ## restore the bibstyle that was in effect
    ##    but need a way to unregister JSSextra if it was not registered before this function
    ##    (alternatively, TODO: JSSextra could be registered (but not set default) when loading rbibutils)
    tools::bibstyle(oldbibstyle, .default = TRUE)

    ## bibtype = 'outline' entry from package 'superb'
    bib_n15 <- system.file("bib", "superb_n15.bib", package = "rbibutils")
    readBib(bib_n15, direct = TRUE)
    readBib(bib_n15)

    ## author: Ne{\v s}lehov{\'a}
    genest2017 <- system.file("bib", "cort_genest2017.bib", package = "rbibutils")
    readBib(genest2017, direct = TRUE)
    readBib(genest2017)

    ## author Lu{\'\i}s Paquete
    GruFon2009_emaa <- system.file("bib", "eaf_GruFon2009.bib", package = "rbibutils")
    readBib(GruFon2009_emaa, direct = TRUE)
    readBib(GruFon2009_emaa)

    ## author: Novovi{\v{c}}ov{\'a}
    Pudil1994 <- system.file("bib", "FsinR_Pudil1994.bib", package = "rbibutils")
    readBib(Pudil1994, direct = TRUE)
    readBib(Pudil1994)

    ## {\v Z}iberna, Ale{\v s}}
    Deneud2006 <- system.file("bib", "partitionComparison_Deneud2006.bib", package = "rbibutils")
    readBib(Deneud2006, direct = TRUE)
    readBib(Deneud2006)
    
    ## {\O}ystein S{\o}rensen and Ren{\'{e}} Westerhausen}
    Sorensen2020 <- system.file("bib", "pkg_BayesianLaterality.bib", package = "rbibutils")
    readBib(Sorensen2020, direct = TRUE)
    readBib(Sorensen2020)

    
    ## {\'E}. Picheral
    Cousquer_1991_PCE <- system.file("bib", "Cousquer_1991_PCE.bib", package = "rbibutils")
    readBib(Cousquer_1991_PCE, direct = TRUE)
    readBib(Cousquer_1991_PCE)

    ## \'E. Picheral
    Cousquer_1991_PCE2 <- system.file("bib", "Cousquer_1991_PCE2.bib", package = "rbibutils")
    readBib(Cousquer_1991_PCE2, direct = TRUE)
    readBib(Cousquer_1991_PCE2)

    
    ## Picheral, {\'E}.  |  in title: \TeX
    Cousquer_1991_PCE5 <- system.file("bib", "Cousquer_1991_PCE5.bib", package = "rbibutils")
    readBib(Cousquer_1991_PCE5, direct = TRUE)
    readBib(Cousquer_1991_PCE5)

    ## Picheral, \'E.
    Cousquer_1991_PCE4 <- system.file("bib", "Cousquer_1991_PCE4.bib", package = "rbibutils")
    readBib(Cousquer_1991_PCE4, direct = TRUE)
    readBib(Cousquer_1991_PCE4)


    ## (lowercase 'e' for testing only)
    
    ## {\'e}. Picheral
    Cousquer_1991_PCE_e <- system.file("bib", "Cousquer_1991_PCE_e.bib", package = "rbibutils")
    readBib(Cousquer_1991_PCE_e, direct = TRUE)
    readBib(Cousquer_1991_PCE_e)

    ## \'e. Picheral
    Cousquer_1991_PCE2e <- system.file("bib", "Cousquer_1991_PCE2_e.bib", package = "rbibutils")
    readBib(Cousquer_1991_PCE2e, direct = TRUE)
    readBib(Cousquer_1991_PCE2e)

    ## Picheral, {\'e}.
    Cousquer_1991_PCE5e <- system.file("bib", "Cousquer_1991_PCE5_e.bib", package = "rbibutils")
    readBib(Cousquer_1991_PCE5e, direct = TRUE)
    readBib(Cousquer_1991_PCE5e)

    ## in booktitle: Qu{\'e}bec; in title: $\mu$
    AugBadBroZit2009hv <- system.file("bib", "AugBadBroZit2009hv.bib", package = "rbibutils")
    readBib(AugBadBroZit2009hv, direct = TRUE) # ok
    readBib(AugBadBroZit2009hv) # messes TeX up: $\mu$  =>  \$\backslashmu\$, etc.
                                # Qu{\'e}bec  =>  Qu{\backslash'e}bec
    
    ## 
    BorEly1998online <- system.file("bib", "iridia_BorEly1998online.bib", package = "rbibutils")
    readBib(BorEly1998online, direct = TRUE)
    readBib(BorEly1998online)




    ## bibentryExtra

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

    brefExtra <- bibentryExtra(bref)

    ## subset as list
    expect_error(brefExtra[[1:2, "title"]],
                 "length of i should be 1 when j is not missing or omitted")
    brefExtra[[1, "title"]]
    brefExtra[[1, "title", drop = FALSE]]
    brefExtra[[1, c("title", "author")]]

    ## subset as bibentryExtra
    brefExtra[1, "title"]
    brefExtra[1, "title", drop = FALSE]
    brefExtra[1, c("title", "author")]
    brefExtra_ta <- brefExtra[1:2, c("title", "author")]

    
    b1 <- brefExtra

    expect_equal(b1$key, list("boot-package", "boot-book"))
    expect_equal(b1$bibtype, list("Manual", "Book"))
    b1$title
    b1$author

    b1$key <- list("package", "book")


    b1 <- brefExtra
    format(b1)
    #expect_output(print(b1))

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






