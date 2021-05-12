
test_that("bibConvert works ok", {
    bibdir <- system.file("bib", package = "rbibutils")

    tmp_xml <- tempfile(fileext = ".xml")
    tmp_bib <- tempfile(fileext = ".bib")
    tmp_bib2 <- tempfile(fileext = ".bib")
    tmp_bib3 <- tempfile(fileext = ".bib")
    tmp_bbl <- tempfile(fileext = ".bbl")
    tmp_bbl2 <- tempfile(fileext = ".bbl")
    tmp_rds <- tempfile(fileext = ".rds")
    tmp_R   <- tempfile(fileext = ".R")

    tmp_ads   <- tempfile(fileext = ".ads")
    tmp_copac   <- tempfile(fileext = ".copac")
    tmp_ebi   <- tempfile(fileext = ".ebi")
    tmp_end   <- tempfile(fileext = ".end")
    tmp_endx   <- tempfile(fileext = ".endx")
    tmp_isi   <- tempfile(fileext = ".isi")
    tmp_med   <- tempfile(fileext = ".med")
    tmp_nbib  <- tempfile(fileext = ".nbib")
    tmp_ris   <- tempfile(fileext = ".ris")
    tmp_wordbib   <- tempfile(fileext = ".wordbib")

    ex0_xml <- file.path(bibdir, "ex0.xml")
    bibConvert(ex0_xml, tmp_bib)

    bibConvert(tmp_bib, tmp_xml)
    bibConvert(tmp_bib, tmp_rds)
    bibConvert(tmp_bib, tmp_R)
    bibConvert(tmp_bib, tmp_bbl, outformat = "biblatex")
    
    bibConvert(tmp_bbl, tmp_bib2, informat = "biblatex")

    bibConvert(tmp_rds, tmp_xml)
    bibConvert(tmp_rds, tmp_bbl2, outformat = "biblatex")

    bibConvert(tmp_bib, tmp_ads)
    
    bibConvert(tmp_bib, tmp_end)
    bibConvert(tmp_end, tmp_bib3, informat = "end")     

    expect_error(bibConvert(tmp_bib, tmp_endx),
                 "export to Endnote XML format not implemented")
    expect_error(bibConvert(tmp_bib, tmp_endx, outformat = "endx"),
                 "export to Endnote XML format not implemented")

    bibConvert(tmp_bib, tmp_isi)
    ## bibConvert(tmp_bib, tmp_nbib) - TODO: seqfaults!
    bibConvert(tmp_bib, tmp_ris)

    bibConvert(tmp_bib, tmp_wordbib, outformat = "word")
    bibConvert(tmp_wordbib, tmp_bib3)

    expect_error(bibConvert(tmp_bib, tmp_copac), "export to copac format not implemented")
    expect_error(bibConvert(tmp_bib, tmp_ebi, outformat = "ebi"),
                 "export to EBI XML format not implemented")
    expect_error(bibConvert(tmp_bib, tmp_med), "export to Medline XML format not implemented")

    

    cyr_utf8 <- file.path(bibdir, "cyr_utf8.bib")
    bibConvert(cyr_utf8, tmp_bib, options = c(o = "cp1251"))
    bibConvert(tmp_bib, tmp_xml, options = c(i = "cp1251"))
    
    bibacc <- system.file("bib/latin1accents_utf8.bib", package = "rbibutils")

    bibConvert(bibacc, tempfile(fileext = "bib"), outformat = "biblatex")

    
    expect_error(bibConvert(tmp_ads, tmp_bib, informat = "ads"))
    
    bibConvert(tmp_end, tmp_bib)
    bibConvert(tmp_endx, tmp_bib)
    bibConvert(tmp_isi, tmp_bib)
    bibConvert(tmp_nbib, tmp_bib)
    bibConvert(tmp_ris, tmp_bib)
    bibConvert(system.file("bib/easyPubMedvig.xml", package = "rbibutils"), tmp_bib,
               informat = "med")
    bibConvert(system.file("bib/easyPubMedvig.xml", package = "rbibutils"), tmp_bib,
               informat = "med", outformat = "biblatex")
})
