
test_that("bibConvert works ok", {
    expect_error(bibConvert(tempfile(fileext = ".bib"), tempfile(fileext = ".bib")),
                 "input file .* doesn't exist")
    bibConvert(tempfile(fileext = ".bib"), tempfile(fileext = ".bib"), options = c(h = ""))

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
    tmp_xml2 <- tempfile(fileext = ".xml")
    tmp_bib <- tempfile(fileext = ".bib")
    tmp_bib2 <- tempfile(fileext = ".bib")
    tmp_bib3 <- tempfile(fileext = ".bib")
    tmp_bibtex <- tempfile(fileext = ".bibtex")
    tmp_biblatex <- tempfile(fileext = ".biblatex")
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
    tmp2_nbib  <- tempfile(fileext = ".nbib")
    tmp_ris   <- tempfile(fileext = ".ris")
    tmp_wordbib   <- tempfile(fileext = ".wordbib")
    tmp_word      <- tempfile(fileext = ".word")

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
    ##     tmp2 <- bibConvert(tmp_copac, tmp_xml, options = c(h = ""))
    ##     tmp2 <- bibConvert(tmp_copac, tmp_bib3, informat = "copac")
    ## export to copac is not implemented:
    expect_error(bibConvert(tmp_bib, tmp_copac), "export to copac format not implemented")
     
    expect_error(bibConvert(tmp_bib, tmp_ebi, outformat = "ebi"),
                 "export to EBI XML format not implemented")
    ## can't export to EBI, so using another file
    bibConvert(system.file("bib", "ebi.xml", package = "rbibutils"), tmp_xml2,
               informat = "ebi")
    
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
    ##     2021-10-29 Note: restoring, maybe this was fixed when other valgroind
    ##                      errors were fixed?
    bibConvert(endx_in, tmp_bib3, informat = "endx", options = c(nb = ""))
    expect_known_value(readLines(tmp_bib3), "end2bib.rds", update = FALSE)
    
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
   
    bibConvert(tmp_bib, tmp_nbib)
    ## This 
    ##     bibConvert(tmp_nbib, tmp_bib2)
    ## fails to process the references with messages like:
    ##
    ## Warning.  Tagged line not in properly started reference.
    ## Ignored: 'VI  - 50'
    ##
    ## There is no error, just 0 output references. Apparently tmp_nbib created from
    ## bib-to-nbib doesn't contain crucial info for each reference.
    ##
    ## TODO: maybe exporting a dummy PMID field will resolve this when the input doesn't have
    ##       it? Or give a more informative message?
    ##
    ## expect_known_value(readLines(tmp_bib2), "nbib2bib.rds", update = FALSE)

    single_nbib <- system.file("bib", "single.nbib", package = "rbibutils")
    bibConvert(single_nbib, tmp_bib2)

    ## when the input file contains PMID the nbib output is read back successfully:
    bibConvert(tmp_bib2, tmp2_nbib)
    bibConvert(tmp2_nbib, tmp_bib3 )

    ## use a shorter file in tests
    ##     bibConvert(system.file("bib", "pubmed-balloongui-set.nbib", package = "rbibutils"), tmp_xml)
    bibConvert(system.file("bib", "pubmed-balloongui-set_09_31542275.nbib", package = "rbibutils"), tmp_xml)
    
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

    ## #########################

    tmpdir <- tempdir()
    xampl_fn <- system.file("bib", "xampl_modified.bib", package = "rbibutils")

    bibConvert(xampl_fn, tmp_ads, options = c(nb = ""))
    expect_known_value(readLines(tmp_ads), "xampl_bib2ads.rds", update = FALSE)
    
    bibConvert(xampl_fn, tmp_bbl, outformat = "biblatex", options = c(nb = ""))
    expect_known_value(readLines(tmp_bbl), "xampl2biblatex.rds", update = FALSE)
    
    xampl_fn2 <- file.path(tmpdir, "xampl_bbl2bib.bib")
    bibConvert(tmp_bbl, xampl_fn2, informat = "biblatex", options = c(nb = ""))
    expect_known_value(readLines(xampl_fn2), "xampl_bbl2bib.rds", update = FALSE)
       
    bibConvert(xampl_fn, tmp_end)
    bibConvert(xampl_fn, tmp_end, outformat = "end", options = c(nb = ""))
    expect_known_value(readLines(tmp_end), "xampl_bib2end.rds", update = FALSE)
    
    bibConvert(tmp_end, tmp_bib3)     
    bibConvert(tmp_end, tmp_bib2, informat = "end", options = c(nb = ""))
    
    bibConvert(xampl_fn, tmp_isi, options = c(nb = ""))
    expect_known_value(readLines(tmp_isi), "xampl_bib2isi.rds", update = FALSE)
    
    bibConvert(tmp_isi, tmp_bib2, options = c(nb = ""))
    expect_known_value(readLines(xampl_fn2), "xampl_isi2bib.rds", update = FALSE)

    bibConvert(med_in, tmp_bib2, informat = "med")
    bibConvert(med_in, tmp_bib3, informat = "med", outformat = "biblatex", options = c(nb = ""))
    expect_known_value(readLines(tmp_bib3), "med2bib.rds", update = FALSE)

    
    ## tex.bib
    tex_fn <- system.file("bib", "tex.bib", package = "rbibutils")

    bibConvert(tex_fn, tmp_ads)
    bibConvert(tex_fn, tmp_bibtex)
    bibConvert(tex_fn, tmp_biblatex)
    bibConvert(tex_fn, tmp_biblatex, informat = "biblatex") # it's bibtex but should work
    
    ## bibConvert(tex_fn, tmp_copac) # not supported
    ## bibConvert(tex_fn, tmp_ebi)      # not supported
    bibConvert(tex_fn, tmp_end)
    ## bibConvert(tex_fn, tmp_endx) 
    bibConvert(tex_fn, tmp_isi)
    ## bibConvert(tex_fn, tmp_med)
    bibConvert(tex_fn, tmp_nbib)
    bibConvert(tex_fn, tmp_ris)
    bibConvert(tex_fn, tmp_R, outformat = "Rstyle")
    bibConvert(tex_fn, tmp_rds)
    bibConvert(tex_fn, tmp_xml)

    bibConvert(tex_fn, tmp_wordbib)
    bibConvert(tex_fn, tmp_word, outformat = "word")

    bibConvert(tmp_bibtex  , tmp_xml2)
    bibConvert(tmp_biblatex, tmp_xml2)
    bibConvert(tmp_word    , tmp_xml2, informat = "word")
    ## bibConvert(tmp_ads  , tmp_xml2)  # not supported
    ## bibConvert(tmp_copac, tmp_xml2)  # tmp_copac not available
    ## bibConvert(tmp_ebi  , tmp_xml2)  # tmp_ebi not available
    bibConvert(tmp_end     , tmp_xml2)
    ## bibConvert(tmp_endx , tmp_xml2)  # tmp_endx not available
    bibConvert(tmp_isi     , tmp_xml2)
    ## bibConvert(tmp_med, , tmp_xml2)  # tmp_med not available
    bibConvert(tmp_nbib    , tmp_xml2)  # TODO: amend nbib output to output dummy PMID's
                                            #       when not present in input file?
    bibConvert(tmp_ris     , tmp_xml2)
    bibConvert(tmp_R       , tmp_xml2)  # "bibentry"
    bibConvert(tmp_rds     , tmp_xml2)
    bibConvert(tmp_xml     , tmp_xml2)
    bibConvert(tmp_wordbib , tmp_xml2)

    
    readBib(tex_fn, direct = TRUE)
    readBib(xampl_fn, direct = TRUE)

    biblatex_fn <- system.file("bib", "biblatex-examples_sans_key_aksin.bib", package = "rbibutils")
    
    tmp <- bibConvert(biblatex_fn, tmp_bib2, informat = "biblatex")
    
})

## TODO: 'tried to fix' is not followed by report if it was successful
##
## > tmp <- bibConvert(tex_fn, tmp_rds)
## Error in .bibentry_check_bibentry1(rval) : 
##   A bibentry of bibtype ‘Book’ has to specify the field: year
## 
## Tried to fix above errors/warnings, see the warnings and messages below.
## 
## key 'G-G'
##       A bibentry of bibtype ‘Book’ has to specify the field: year



