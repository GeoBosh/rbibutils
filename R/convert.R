bibConvert <- function(infile, outfile, informat, outformat){
    argc <- 2L
    argv <- c("dummy", infile)
                                        # sink(outfile)
                                        # on.exit(sink())
    if(missing(informat)){
        ext <- tools::file_ext(infile)
        informat <- switch(ext,
                           xml      = "xml",
                           
                           bib      = "bibtex",
                           bibtex   = "bibtex",
                           biblatex = "biblatex",

                           copac    = "copac",
                           end      = "end",  
                           endx     = "endx",  
                           isi      = "isi",  
                           med      = "med",  
                           nbib     = "nbib",  
                           ris      = "ris",

                           rds = "bibentry",

                           ## default
                           stop("Can't infer input format, please use arg. informat")
                           )
    }

    if(missing(outformat)){
        ext <- tools::file_ext(outfile)
        outformat <- switch(ext,
                           xml      = "xml",
                           
                           bib      = "bibtex", 
                           bibtex   = "bibtex",
                           biblatex = "biblatex",

                           ads      = "ads",
                           copac    = "copac",
                           end      = "end",  
                           endx     = "end",  # is there to "endx"?
                           isi      = "isi",  
                           med      = "med",  
                           nbib     = "nbib",  
                           ris      = "ris",
                           wordbib  = "wordbib",
                           
                           rds = "bibentry",

                           ## default
                           stop("Can't infer output format, please use arg. outformat")
                           )
    }

    if(outformat == "xml")
        xmlfile <- outfile
    else{
        xmlfile <- tempfile(fileext = ".xml")
        on.exit(unlink(xmlfile))
    }

    
    wrk <- switch(informat,
                  xml      = xmlfile,
                  bibtex   = .C(C_bib2xml_main, argc, argv, xmlfile),
                  biblatex = .C(C_biblatex2xml_main, argc, argv, xmlfile),
                  ## default
                  stop("converting a file from format ", informat, " not available yet")
                  )
                  
    

    switch(outformat,
           xml = xmlfile,
           bibtex = ,
           bib = {
               argv <- c("dummy", xmlfile)
               ##     xml2bib_main( int *argc, char *argv[], char *outfile[] )
               wrk <- .C(C_xml2bib_main, argc, argv, outfile)
           },
           bibentry = {
               modsi.obj <- read_mods(xmlfile)
               bm <- bibmods(modsi.obj)
               bibe <- toBibentry(bm)
               saveRDS(bibe, outfile)
               ## TODO: think of suitable return value
               bibe
           },
           ## default
           stop("outformat ", outformat, " not supported by bibConvert yet")
           )
}


