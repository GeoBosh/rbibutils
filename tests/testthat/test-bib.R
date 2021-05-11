
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

    litprog280 <- readBib(system.file("bib/litprog280.bib", package = "rbibutils"),
                          direct = TRUE)

    ## non-syntactic field
    expect_equal(litprog280$"issn-l", "0883-7252") 
    
    ## expanded abbreviation (in the bib file journal = j-J-APPL-ECONOMETRICS)
    expect_equal(litprog280$journal, "Journal of Applied Econometrics")

    ## string concatenation
    expect_equal(litprog280$month, "jan--feb")

    ## litprog <- readBib("/home/georgi/repos/private/rbibutils/data-raw/litprog_no_atpreamble.bib", direct = TRUE)
})


