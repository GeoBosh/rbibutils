
test_that("bibConvert works ok", {
    expect_true(TRUE)
    
    bibdir <- system.file("bib", package = "rbibutils")

    tmp_xml <- tempfile(fileext = ".xml")
    tmp_bib <- tempfile(fileext = ".bib")
    tmp_bib2 <- tempfile(fileext = ".bib")
    tmp_bbl <- tempfile(fileext = ".bbl")
    tmp_bbl2 <- tempfile(fileext = ".bbl")
    tmp_rds <- tempfile(fileext = ".rds")
    tmp_R   <- tempfile(fileext = ".R")

    ex0_xml <- file.path(bibdir, "ex0.xml")
    bibConvert(ex0_xml, tmp_bib)

    bibConvert(tmp_bib, tmp_xml)
    bibConvert(tmp_bib, tmp_rds)
    bibConvert(tmp_bib, tmp_R)
    bibConvert(tmp_bib, tmp_bbl, outformat = "biblatex")
    
    bibConvert(tmp_bbl, tmp_bib2, informat = "biblatex")

    bibConvert(tmp_rds, tmp_xml)
    bibConvert(tmp_rds, tmp_bbl2, outformat = "biblatex")


    cyr_utf8 <- file.path(bibdir, "cyr_utf8.bib")
    bibConvert(cyr_utf8, tmp_bib, options = c(o = "cp1251"))
    bibConvert(tmp_bib, tmp_xml, options = c(i = "cp1251"))
    
    bibacc <- system.file("bib/latin1accents_utf8.bib", package = "rbibutils")

    bibConvert(bibacc, "tmp.bib", outformat = "biblatex")
})





