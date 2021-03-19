
test_that("bibRead works ok", {
    bibacc <- system.file("bib/latin1accents_utf8.bib", package = "rbibutils")

    expect_known_value(
        readBib(bibacc, direct = TRUE, texChars = "keep"),
        "bibacc_1.rds", update = FALSE )
    
    expect_known_value(
        readBib(bibacc, direct = TRUE, texChars = "export"),
        "bibacc_2.rds", update = FALSE )

    expect_known_value(
        readBib(bibacc, direct = TRUE, texChars = "convert"),
        "bibacc_3.rds", update = FALSE )

    
    

})
