
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





    
    xeCJK_utf8    <- system.file("bib", "xeCJK_utf8.bib", package = "rbibutils")
    xeCJK_gb18030 <- system.file("bib", "xeCJK_gb18030.bib", package = "rbibutils")
    fn_gb18030 <- tempfile(fileext = ".bib")
    on.exit(unlink(fn_gb18030))
    
    ## TODO: the encoding for authors is not handled correctly ('title' is fine)
    bibConvert(xeCJK_utf8, fn_gb18030, encoding = c("utf8", "gb18030"))
    
    obj1 <- readBib(xeCJK_gb18030, encoding = "gb18030")
    obj2 <- readBib(xeCJK_utf8)
    ## expect_equal(obj1, obj2) # see above TODO for the reason to comment out this line


    ## issue 5
    issue_5 <- system.file("bib", "issue_5.bib", package = "rbibutils")

    test_5 <- rbibutils::readBib(issue_5, direct = TRUE)
    expect_known_value(test_5, "issue_5a.rds", FALSE)

    if(packageVersion("rbibutils") > '2.2.4'){
        test_5rdpack <- rbibutils::readBib(issue_5, direct = TRUE, texChars = "Rdpack")
        expect_known_value(test_5rdpack, "issue_5rdpack.rds", FALSE)
    }
    
    test5_extra <- as.bibentryExtra(test_5)
    expect_identical(test5_extra[["x", "author"]]$family, test5_extra[["y", "author"]]$family)
    
    expect_known_value(rbibutils::readBib(issue_5, direct=TRUE, encoding = "UTF-8", texChars = "convert"),
                       "issue_5_utf8.rds", FALSE)

    bib_extra <- system.file("bib", "extra.bib", package = "rbibutils")

    ## 2023-11-04 - after today's changes no longer warning
    ## expect_message(readBib(bib_extra, direct = TRUE, extra = TRUE))

    ## test the fix for texChars = "export"; the file contains both escaped TeX chars and unicode chars
    bib_texChars <- system.file("bib", "texChars.bib", package = "rbibutils")
    expect_known_value(readBib(bib_texChars, direct = TRUE),
                       "texChars_kept.rds", FALSE)

    expect_known_value(readBib(bib_texChars, direct = TRUE, texChars = "convert"),
                       "texChars_converted.rds", FALSE)
                       
    expect_known_value(readBib(bib_texChars, direct = TRUE, texChars = "export"),
                       "texChars_exported.rds", FALSE)

    xample_fn <- system.file("bib", "xampl_modified.bib", package = "rbibutils")

    ## 2023-11-05 TODO: xampl[[3]] has a crossreference to a journal but it contains
    ##     redundant duplicated tags. It looks like bibtexin_crossref_oneref (and/or the
    ##     calling functions) needs fixing. (although this bug may be harmless)
    ##
    xampl <- readBib(xample_fn, direct = TRUE)
    expect_known_value(xampl, "xampl_1.rds", FALSE)

    ## This gives error (a number of such things currently are fixed only for direct = TRUE): 
    ##    expect_known_value(readBib(xample_fn), "xampl_2.rds", FALSE)
    ##
    ## > tmp <- readBib(xample_fn)
    ## Error in parse(n = -1, file = file, srcfile = NULL, keep.source = FALSE,  : 
    ##   360:39: unexpected symbol
    ## 359:       key = "unpublished-minimal",
    ## 360:       author = c(person(family = "{\\"U
    ##
    ## Trace:
    ##
    ## Enter a frame number, or 0 to exit   
    ##
    ## 1: readBib(xample_fn)
    ## 2: bib.R#47: bibConvert(file, bib, "bibtex", "bibentry", encoding = encoding, 
    ## 3: convert.R#279: readBibentry(outfile)
    ## 4: bibentry.R#9: parse(n = -1, file = file, srcfile = NULL, keep.source = FALS

    acc_fn <- system.file("bib", "accents_tabbing.bib", package = "rbibutils")
    accfn <- readBib(acc_fn, direct = TRUE)
    
    ## fix for a change in R-devel, see Rdpack/tests/testthattest-bib.R for more info
    if(is.numeric(svnrev <- R.Version()$'svn rev')  &&  svnrev >= 84986)
        expect_known_value(accfn, "acc_fn.rds", FALSE)
})
