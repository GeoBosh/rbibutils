
test_that("bibConvert works ok", {
    bibdir <- system.file("bib", package = "rbibutils")

    ## file is from
    ##    https://gist.github.com/low-decarie/3831049#file-endnote-xml
    ## downloaded on 2021-05-14
    ##
    ## This doesn't work under devtools since pkgload::system.file
    ## adds 'inst' again: 
    ##     endx_in <- system.file(bibdir, "endnote.xml", package = "rbibutils")
    ##
    ## file.path here is more logical anyway
    endx_in <- file.path(bibdir, "endnote.xml")
    med_in  <- file.path(bibdir, "easyPubMedvig.xml")
    med_in_no_new_lines <- file.path(bibdir, "easyPubMedvig_no_new_lines.xml")
    bibacc  <- file.path(bibdir, "latin1accents_utf8.bib")
    end_in  <- file.path(bibdir, "Putnam1992.end")
    litprog280_bib <- file.path(bibdir, "litprog280.bib")
    
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
    bibConvert(ex0_xml, tmp_bib, options = c(nb = ""))
    expect_known_value(readLines(tmp_bib), "xml2bib.rds", update = FALSE)

    
    bibConvert(tmp_bib, tmp_ads, options = c(nb = ""))
    expect_known_value(readLines(tmp_ads), "bib2ads.rds", update = FALSE)
    
    expect_error(bibConvert(tmp_ads, tmp_bib2),
                 "import from ADS abstracts format not implemented")
    expect_error(bibConvert(tmp_ads, tmp_bib2, informat = "ads"),
                 "import from ADS abstracts format not implemented")

    
    bibConvert(tmp_bib, tmp_bbl, outformat = "biblatex", options = c(nb = ""))
    expect_known_value(readLines(tmp_bbl), "bib2biblatex.rds", update = FALSE)

    bibConvert(tmp_bbl, tmp_bib2, informat = "biblatex", options = c(nb = ""))
    expect_known_value(readLines(tmp_bib2), "biblatex2bib.rds", update = FALSE)

    
    ## TODO: need copac file to test for input from copac
    expect_error(bibConvert(tmp_bib, tmp_copac),
                 "export to copac format not implemented")
 
     
    expect_error(bibConvert(tmp_bib, tmp_ebi, outformat = "ebi"),
                 "export to EBI XML format not implemented")
    ## TODO: need EBI file to test for input from EBI

    
    bibConvert(tmp_bib, tmp_end)
    bibConvert(tmp_bib, tmp_end, outformat = "end", options = c(nb = ""))
    expect_known_value(readLines(tmp_end), "bib2end.rds", update = FALSE)

    bibConvert(tmp_end, tmp_bib3)     
    bibConvert(tmp_end, tmp_bib2, informat = "end", options = c(nb = ""))
    ## github actions gives error on all platforms:
    ##       incomplete final line found on '.../Rtmp770EIj/fileb7c68793fe6.bib'
    ## expect_known_value(readLines(tmp_bib2), "end2bib1.rds", update = FALSE)
    
    bibConvert(end_in, tmp_bib3, options = c(nb = ""))
    ## see note above 
    ## expect_known_value(readLines(tmp_bib3), "end2bib2.rds", update = FALSE)
    

    expect_error(bibConvert(tmp_bib3, tmp_endx),
                 "export to Endnote XML format not implemented")
    expect_error(bibConvert(tmp_bib3, tmp_endx, outformat = "endx"),
                 "export to Endnote XML format not implemented")
    expect_message(bibConvert(endx_in, tmp_bib3), "no references to output")
## these raise valgrind erors abput uninitialised values:
##     bibConvert(endx_in, tmp_bib3, informat = "endx", options = c(nb = ""))
##     expect_known_value(readLines(tmp_bib3), "end2bib.rds", update = FALSE)

    
    bibConvert(tmp_bib, tmp_isi, options = c(nb = ""))
    expect_known_value(readLines(tmp_isi), "bib2isi.rds", update = FALSE)

    bibConvert(tmp_isi, tmp_bib2, options = c(nb = ""))
    expect_known_value(readLines(tmp_bib2), "isi2bib.rds", update = FALSE)

    
    expect_error(bibConvert(tmp_bib, tmp_med),
                 "export to Medline XML format not implemented")
    bibConvert(med_in, tmp_bib, informat = "med")
    bibConvert(med_in, tmp_bib, informat = "med", outformat = "biblatex", options = c(nb = ""))
    expect_known_value(readLines(tmp_bib), "med2bib.rds", update = FALSE)

    ## issue #4
    tmp_meda <- tempfile()
    tmp_medb <- tempfile()
    bibConvert(infile = med_in,              outfile = tmp_meda, informat = "med", outformat = "bib")
    bibConvert(infile = med_in_no_new_lines, outfile = tmp_medb, informat = "med", outformat = "bib")
    expect_identical(readLines(tmp_meda), readLines(tmp_medb))
    unlink(tmp_meda)
    unlink(tmp_medb)
    
    
    ## this assignment was used when the above lines were commented out during memory leak tests.
    ##   (but it causes check error on Windows due to BOM)
    ## tmp_bib <- file.path(bibdir, "bib_from_medin.bib")
   
    ## bibConvert(tmp_bib, tmp_nbib)   # TODO: segfaults!
    ## bibConvert(tmp_nbib, tmp_bib2)  # TODO: need nbib file for import!
    ## expect_known_value(readLines(tmp_bib2), "nbib2bib.rds", update = FALSE)

    
    bibConvert(tmp_bib, tmp_ris, options = c(nb = ""))
    expect_known_value(readLines(tmp_ris), "bib2ris.rds", update = FALSE)

    bibConvert(tmp_ris, tmp_bib, options = c(nb = ""))
    expect_known_value(readLines(tmp_bib), "ris2bib.rds", update = FALSE)

    
    bibConvert(tmp_bib, tmp_xml, options = c(un = ""))
    expect_known_value(readLines(tmp_xml), "bib2xml2.rds", update = FALSE)

    bibConvert(tmp_bib, tmp_rds, options = c(nb = ""))
    expect_known_value(readRDS(tmp_rds), "bib2rds.rds", update = FALSE)

    bib2R <- bibConvert(tmp_bib, tmp_R)
    expect_known_value(bib2R$bib, "bib2R.rds", update = FALSE)

    
    bibConvert(tmp_rds, tmp_xml, options = c(un = ""))  # rds to MODS XML intermediate
    expect_known_value(readLines(tmp_xml), "rds2xml.rds", update = FALSE)

    bibConvert(tmp_rds, tmp_bbl2, outformat = "biblatex", options = c(nb = ""))
    expect_known_value(readLines(tmp_bbl2), "rds2bbl2.rds", update = FALSE)

    
    bibConvert(tmp_bib, tmp_wordbib)
    bibConvert(tmp_bib, tmp_wordbib, outformat = "wordbib", options = c(nb = ""))
    expect_known_value(readLines(tmp_wordbib), "bib2wordbib.rds", update = FALSE)

    ## TODO: this currently misses the authors! Don't know if the culprit is
    ##       the above conversion to wordbib.
    bibConvert(tmp_wordbib, tmp_bib3)
    bibConvert(tmp_wordbib, tmp_bib3, informat = "wordbib", options = c(nb = ""))
    expect_known_value(readLines(tmp_bib3), "wordbib2bib.rds", update = FALSE)

    ## accept also 'word' since, due to a mixup, the C code was accepting that
    bibConvert(tmp_bib, tmp_wordbib, outformat = "word")
    bibConvert(tmp_wordbib, tmp_bib3, informat = "word")

    ## #########################
    ## -h and -v currently print to standard error and continue

    ## Without the assignment to tmp2 inside capture.output(..., type = "message")
    ## the results are printed by devtools::test() and friends.

    ## -h
    ## a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_bib, options = c(h = "")), type = "message")
    ## a <- capture.output(bibConvert(ex0_xml, tmp_bib, options = c(h = "")), type = "message")
    ## expect_match(a, "usage: xml2bib xml_file > bibtex_file", all = FALSE)
    ##
    ## The above catch the message by REprintf but then leak memory
    ## (discovered by valgrind with --leak-check=full --track-origins=yes --show-leak-kinds=all)
    ##
    ##  The leak seems to come from capture.output() and its interference with testthat. Note
    ##  that I put the above example in one of the Rd files and there was no problem there
    ##  from the check (checks of examples are not run under testthat),
    ## 
    ##  For example in xml2any, help_xml2bibtex() has a bunch of print statements and
    ##  starting from about the one with '-nl, --no-latex' (but not the previous ones)
    ##  REprintf seem not to release some memory.  testthat's caputure_output doesn't have
    ##  argument 'type'. so doesn't capture stderr.
    ##
    ## So, for now just run the statement to make sure that they work (and similarly for the
    ## similar commands below. The downside is that the messages apppear in testthat.Rout and, if under devtools,
    ## on the console, which is unnerving. (:TODO: find a way to fix this)
    tmp2 <- bibConvert(ex0_xml, tmp_bib, options = c(h = ""))

    ##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_bib2, outformat = "biblatex", options = c(h = "")), type = "message")
    ##     expect_match(a, "usage: xml2biblatex xml_file > biblatex_file", all = FALSE)
    bibConvert(ex0_xml, tmp_bib2, outformat = "biblatex", options = c(h = ""))
 
##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_ads, options = c(h = "")), type = "message")
##     expect_match(a, "usage: xml2ads xml_file > adsabs_file", all = FALSE)
    tmp2 <- bibConvert(ex0_xml, tmp_ads, options = c(h = ""))
    
##       # a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_copac, options = c(h = "")), type = "message")
##       # a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_ebi, options = c(h = "")), type = "message")
##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_end, options = c(h = "")), type = "message")
##     expect_match(a, "usage: xml2end xml_file > endnote_file", all = FALSE)
    tmp2 <- bibConvert(ex0_xml, tmp_end, options = c(h = ""))
    
##       # a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_endx, options = c(h = "")), type = "message")
##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_isi, options = c(h = "")), type = "message")
##     expect_match(a, "usage: xml2isi xml_file > isi_file", all = FALSE)
    tmp2 <- bibConvert(ex0_xml, tmp_isi, options = c(h = ""))
    
      # a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_med, options = c(h = "")), type = "message")
    #a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_nbib, options = c(h = "")), type = "message")
    #expect_match(a, "", all = FALSE)
 
##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_ris, options = c(h = "")), type = "message")
##     expect_match(a, "usage: xml2ris xml_file > ris_file", all = FALSE)
    tmp2 <- bibConvert(ex0_xml, tmp_ris, options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_wordbib, options = c(h = "")), type = "message")
##     expect_match(a, "usage: xml2wordbib xml_file > word_file", all = FALSE)
    tmp2 <- bibConvert(ex0_xml, tmp_wordbib, options = c(h = ""))
 
##     a <- capture.output(tmp2 <- bibConvert(tmp_bib,  tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: bib2xml bibtex_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_bib,  tmp_xml, options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_bib2,    tmp_xml, informat = "biblatex", options = c(h = "")), type = "message")
##     expect_match(a, "usage: biblatex2xml bibtex_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_bib2,    tmp_xml, informat = "biblatex", options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_end,     tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: end2xml endnote_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_end,     tmp_xml, options = c(h = ""))
    
##       # a <- capture.output(tmp2 <- bibConvert(tmp_ads,     tmp_xml, options = c(h = "")), type = "message")
##     a <- capture.output(tmp2 <- bibConvert(tmp_copac,   tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: copac2xml copac_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_copac,   tmp_xml, options = c(h = ""))
    
##       # a <- capture.output(tmp2 <- bibConvert(tmp_ebi,     tmp_xml, options = c(h = "")), type = "message")
##     a <- capture.output(tmp2 <- bibConvert(tmp_end,     tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: end2xml endnote_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_end,     tmp_xml, options = c(h = ""))
##     a <- capture.output(tmp2 <- bibConvert(tmp_endx,    tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: endx2xml endnotexml_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_endx,    tmp_xml, options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_isi,     tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: isi2xml isi_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_isi,     tmp_xml, options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_med,     tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: med2xml medline_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_med,     tmp_xml, options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_nbib,    tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: nbib2xml nbib_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_nbib,    tmp_xml, options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_ris,     tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "ris_file can be replaced with file list or omitted to use as a filter", all = FALSE)
    tmp2 <- bibConvert(tmp_ris,     tmp_xml, options = c(h = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_wordbib, tmp_xml, options = c(h = "")), type = "message")
##     expect_match(a, "usage: wordbib2xml word2007bib_file > xml_file", all = FALSE)
    tmp2 <- bibConvert(tmp_wordbib, tmp_xml, options = c(h = ""))
 
       ## -v
##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_bib, options = c(v = "")), type = "message")
##     expect_match(a, ".*xml2bib, bibutils suite version", all = FALSE)
    tmp2 <- bibConvert(ex0_xml, tmp_bib, options = c(v = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_bib,  tmp_xml, options = c(v = "")), type = "message")
##     expect_match(a, "bib2xml, bibutils suite version", all = FALSE)
    tmp2 <- bibConvert(tmp_bib,  tmp_xml, options = c(v = ""))
     
       ## --debug
##     a <- capture.output(tmp2 <- bibConvert(ex0_xml, tmp_bib, options = c(debug = "")), type = "message")
##     expect_match(a, "-------------------params start ", all = FALSE)
    tmp2 <- bibConvert(ex0_xml, tmp_bib, options = c(debug = ""))
    
##     a <- capture.output(tmp2 <- bibConvert(tmp_bib, tmp_xml, options = c(verbose = "")), type = "message")
##     expect_match(a, "# NUM   level = LEVEL   'TAG' = 'VALUE'", all = FALSE)
    tmp2 <- bibConvert(tmp_bib, tmp_xml, options = c(verbose = ""))
 
       ## --verbose
##     a <- capture.output(tmp2 <- bibConvert(litprog280_bib, tmp_xml, options = c(verbose = "")), type = "message")
##     expect_match(a, "bib2xml: Cannot find tag 'ISSN-L'", all = FALSE)  # Note: there are other unknown tags
    tmp2 <- bibConvert(litprog280_bib, tmp_xml, options = c(verbose = ""))
    
    ## #########################

    cyr_utf8 <- file.path(bibdir, "cyr_utf8.bib")
    bibConvert(cyr_utf8, tmp_bib, options = c(o = "cp1251"))
    bibConvert(tmp_bib, tmp_xml, options = c(i = "cp1251"))
    
    bibConvert(bibacc, tempfile(fileext = "bib"), outformat = "biblatex")
    
})
