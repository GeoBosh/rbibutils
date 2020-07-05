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

                           "R"      = ,
                           r        = "r",
                           rds      = "bibentry",

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

                           R        = ,
                           r        = "r",
                           rds      = "bibentry",

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

                  bibtex   = {
                      prg <- paste0("bib", "2xml")
                      .C(C_any2xml_main, argc, argv, xmlfile, prg)
                  }, 
                  biblatex = ,
                  ads      = ,
                  copac    = ,
                  end      = ,
                  endx     = ,
                  isi      = ,
                  med      = ,
                  nbib     = ,
                  ris      = ,
                  wordbib  = {
                      prg <- paste0(informat, "2xml")
                      .C(C_any2xml_main, argc, argv, xmlfile, prg)
                  },

                  r        = ,
                  bibentry = {
                      bibe <- if(informat == "bibentry")
                                  readRDS(infile)
                              else
                                  readBibentry(infile)
                      
                      ## convert to bibtex and save to file
                      bibfn <- tempfile(fileext = ".bib")
                      ## krapka, check for keys

                      ## krapka, bib2xml is not happy with empty keys
                      keys <- bibe$key
                      for(i in seq_along(bibe)){
                          if(is.null(keys[[i]]))
                              bibe$key[i] <- paste0("tmp", i)
                      }

                      writeLines(toBibtex(bibe), bibfn)
                      argv[2] <- bibfn
                      
                      .C(C_any2xml_main, argc, argv, xmlfile, "bib2xml")
                  },
                  ## default
                  stop("converting a file from format ", informat, " not available yet")
                  )

    argv <- c("dummy", xmlfile)
    switch(outformat,
           xml = xmlfile,
           bibtex = ,
           bib = {
                    #  wrk <- .C(C_xml2bib_main, argc, argv, outfile, "xml2bib")
               prg <- paste0("xml2", "bib")
               wrk <- .C(C_xml2any_main, argc, argv, outfile, prg)

           },
               # biblatex = {
               # 
               #         # wrk <- .C(C_xml2biblatex_main, argc, argv, outfile, "xml2biblatex")
               #     prg <- paste0("xml2", "biblatex")
               #     wrk <- .C(C_xml2any_main, argc, argv, outfile, prg)
               # },
           R = ,
           r = ,
           bibentry = {
               modsi.obj <- read_mods(xmlfile)
               bm <- bibmods(modsi.obj)
               bibe <- toBibentry(bm)
               if(outformat == "bibentry"){
                   saveRDS(bibe, outfile)
               }else{ # r
                   writeBibentry(bibe, outfile)
               }
               ## TODO: think of suitable return value
               bibe
           },
           {
               ## default
                   # stop("outformat ", outformat, " not supported by bibConvert yet")
               prg <- paste0("xml2", outformat)
               wrk <- .C(C_xml2any_main, argc, argv, outfile, prg)
           }
           )
}

readBibentry <- function(file){
    expr <- parse(file, encoding = "UTF-8") # NOTE: fixed encoding for now

    fu <- function(){
        .allval <- vector(length(expr), mode = "list")
        for(i in seq_along(expr)){
            .val <- eval(expr[i])
            .allval[[i]] <- if(is.null(.val))
                               NA
                           else
                               .val
        }
        .bibflag <- sapply(.val, function(x) inherits(x, "bibentry"))
        .wrk <- .allval[.bibflag]
        .vars <- mget(ls())
        if(length(.vars) > 0){
            .bibflag <- sapply(.vars, function(x) inherits(x, "bibentry"))
            .vars <- .vars[.bibflag]
            .wrk <- c(.vars, .wrk)
        }
        
        do.call("c", .wrk)
    }
    
    fu()
}

writeBibentry <- function(be, file){
    con <- file(file, "wt")
    on.exit(close(con))


    sink(con)
    # on.exit(sink(), add = TRUE)

    for(i in seq_along(be)){
        print(be[i], style = "R")
        cat("\n")
    }
    sink()
    NULL
}
