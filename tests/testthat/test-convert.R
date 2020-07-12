
test_that("bibConvert works ok", {
    bibdir <- system.file("bib", package = "rbibutils")

    tmp_xml <- tempfile(fileext = ".xml")
    tmp_bib <- tempfile(fileext = ".bib")
    tmp_bbl <- tempfile(fileext = ".bbl")
    tmp_rds <- tempfile(fileext = ".rds")
    tmp_R   <- tempfile(fileext = ".R")

    ex0_xml <- file.path(bibdir, "ex0.xml")
    bibConvert(ex0_xml, tmp_bib)

    bibConvert(tmp_bib, tmp_xml)
    bibConvert(tmp_bib, tmp_rds)
    bibConvert(tmp_bib, tmp_R)

    bibConvert(tmp_rds, tmp_xml)
    bibConvert(tmp_rds, tmp_bbl, outformat = "biblatex")


    cyr_utf8 <- file.path(bibdir, "cyr_utf8.bib")
    bibConvert(cyr_utf8, tmp_bib, options = c(o = "cp1251"))
    bibConvert(tmp_bib, tmp_xml, options = c(i = "cp1251"))
    

})





