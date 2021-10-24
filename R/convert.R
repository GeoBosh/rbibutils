bibConvert <- function(infile, outfile, informat, outformat, ..., tex, encoding, options){
    stopifnot(length(list(...)) == 0) # no ... arguments allowed
    
    if(!is.character(infile))
        stop("argument 'infile' must be a character string")
    else if(!file.exists(infile) && (missing(options) || is.null(options["h"]))){
        stop("input file \"", infile, "\" doesn't exist")
    }
        
    if(missing(informat)){
        ext <- tools::file_ext(infile)
        informat <- switch(ext,
                           xml      = {
                               message("\n assuming input format is MOD XML intermediate.\n")
                               "xml"
                           },

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
                           wordbib  = "wordbib",

                           "R"      = ,
                           r        = "r",
                           rds      = "bibentry",

                           ads      = "ads",
                           
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
                           endx     = "endx",  # is there to "endx"?
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

    if(informat == "xml")
        xmlfile <- infile
    else if(outformat == "xml")
        xmlfile <- outfile
    else{
        xmlfile <- tempfile(fileext = ".xml")
        on.exit(unlink(xmlfile))
    }

    #argv <- c("dummy", xmlfile)
    #argv <- c("dummy", "-i", "cp1251",  xmlfile)
    #argv <- c("dummy", "-i", "cp1251", "-o", "utf8", xmlfile)

    argv_2xml <- c("dummy")
    argv_xml2 <- c("dummy")

    if(!missing(encoding)){
        if(length(encoding) == 1)
            encoding <- rep(encoding, 2)

        ## todo: UTF-8 => utf8 ?
        argv_2xml <- c(argv_2xml, "-i", encoding[1])
        argv_xml2 <- c(argv_xml2, "-o", encoding[2])
    }

    if(!missing(tex)){
        for(tex_op in tex){
            switch(tex_op,
                   no_latex = { # accents to letters
                       argv_2xml <- c(argv_2xml, "-nl")
                       ## TODO: this is relevant for xml2xxx only when xxx is a latex related format
                       ##       for now inserting a line in the C code to ignore it without warning
                       argv_xml2 <- c(argv_xml2, "-nl") 
                   },
                   uppercase = {
                       argv_xml2 <- c(argv_xml2, "-U")
                   },
                   brackets = {
                       argv_xml2 <- c(argv_xml2, "-b")
                   },
                   dash = {
                       argv_xml2 <- c(argv_xml2, "-sd")
                   },
                   comma = {
                       argv_xml2 <- c(argv_xml2, "-fc")
                   },
                   ## default
                   stop("unsupported 'tex' option")
                   )
        }
    }
    
    if(!missing(options)){
        nams <- names(options)
        ## options <- as.vector(options)
        for(j in seq_along(options)){
            switch(nams[j],
                   i = { argv_2xml <- c(argv_2xml, "-i", options[j])},
                   o = {
                       argv_xml2 <- c(argv_xml2, "-o", options[j])
                       ## print(argv_xml2)
                   },
                   oxml = {argv_2xml <- c(argv_2xml, "-o", options[j])},
                   h = {
                       argv_2xml <- c(argv_2xml, "-h")
                       argv_xml2 <- c(argv_xml2, "-h")
                   },
                   v = {
                       argv_2xml <- c(argv_2xml, "-v")
                       argv_xml2 <- c(argv_xml2, "-v")
                   },
                   a = {argv_2xml <- c(argv_2xml, "-a")},
                   s = {argv_2xml <- c(argv_2xml, "-s")},
                   u = {argv_2xml <- c(argv_2xml, "-u")},
                   U = {argv_xml2 <- c(argv_xml2, "-U")},
                   un = {argv_2xml <- c(argv_2xml, "-un")},
                   x = {argv_2xml <- c(argv_2xml, "-x")},
                   nl = {
                       argv_2xml <- c(argv_2xml, "-nl")
                       argv_xml2 <- c(argv_xml2, "-nl")
                   },
                   d = { argv_2xml <- c(argv_2xml, "-d") },
                   c = {argv_2xml <- c(argv_2xml, "-c", options[j])},
                   ## as = {argv_2xml <- c(argv_2xml, "-as", options[j])},
                   nt = {argv_2xml <- c(argv_2xml, "-nt")},
                   verbose = {
                       argv_2xml <- c(argv_2xml, "--verbose")
                       argv_xml2 <- c(argv_xml2, "--verbose")
                   },
                   nb = {
                       ## TODO: However, switch or no switch, on linux the BOM is not added.
                       ##       But BOM is inserted on windows without the switch.
                       ## Currently I have no idea why linux is special in this respect.
                       argv_xml2 <- c(argv_xml2, "-nb")
                       ## for 2xml the switch in bibutils is -un
                       argv_2xml <- c(argv_2xml, "-un")
                   },
                   debug = {
                       argv_2xml <- c(argv_2xml, "--debug")
                       argv_xml2 <- c(argv_xml2, "--debug")
                   },
                   
                   ##default
                   stop("unsupported option '", nams[j])
                   )
        }
    }

    ## print(argv_xml2)    

    argv_2xml <- c(argv_2xml, infile)
    argv_xml2 <- c(argv_xml2, xmlfile)

    argc_2xml <- as.integer(length(argv_2xml))
    argc_xml2 <- as.integer(length(argv_xml2))

    n_2xml <- as.double(0) # for the number of references (double
    n_xml2 <- as.double(0) 

    ## earlier versions accepted "word" (in the "C" code)
    if(informat == "word")
        informat <- "wordbib"
    
    wrk <- switch(informat,
                  xml      = {
                      wrk_in <- list(xmlfile)
                  },

                  bibtex   = {
                      prg <- paste0("bib", "2xml")
                      argv_2xml[1] <- prg
                      wrk_in <- .C(C_any2xml_main, argc_2xml, argv_2xml, xmlfile, nref_in = n_2xml)
                  }, 
                  biblatex = ,
                  ads      = ,
                  copac    = ,
                  ebi      = ,
                  end      = ,
                  endx     = ,
                  isi      = ,
                  med      = ,
                  nbib     = ,
                  ris      = ,
                  wordbib  = {
#print("Hello!")                      
                      prg <- paste0(informat, "2xml")
                      argv_2xml[1] <- prg
                      wrk_in <- .C(C_any2xml_main, argc_2xml, argv_2xml, xmlfile, nref_in = n_2xml)
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
                      argv_2xml[length(argv_2xml)] <- bibfn
                      argv_2xml[1] <- "bib2xml"
                      wrk_in <- .C(C_any2xml_main, argc_2xml, argv_2xml, xmlfile, nref_in = n_2xml)
                  },
                  ## default
                  stop("converting a file from format ", informat, " not available yet")
                  )

    ## output
    argv_xml2 <- as.character(argv_xml2)    
    switch(outformat,
           xml = {
               wrk_out = list(xmlfile)
           },
           bibtex = ,
           bib = {
                    #  wrk_out <- .C(C_xml2bib_main, argc_xml2, argv_xml2, outfile, "xml2bib")
               prg <- paste0("xml2", "bib")
               argv_xml2[1] <- prg
               wrk_out <- .C(C_xml2any_main, as.integer(argc_xml2), argv_xml2, outfile, nref_out = n_xml2)
           },
               # biblatex = {
               # 
               #         # wrk_out <- .C(C_xml2biblatex_main, argc_xml2, argv_xml2, outfile, "xml2biblatex")
               #     prg <- paste0("xml2", "biblatex")
               #     wrk_out <- .C(C_xml2any_main, argc_xml2, argv_xml2, outfile, prg)
               # },
           R = ,
           r = ,
           Rstyle = , # 2020-11-08 new:
                      # as print(be, style = "R"), currently this is returned by the C code
           bibentry = {
               ## TODO: !!! the variants for bibentry should probably be specified by
               ##       options, not different main level types.
               
                    #  wrk_out <- .C(C_xml2bib_main, argc_xml2, argv_xml2, outfile, "xml2bib")
               prg <- paste0("xml2", "bibentry") # "bibentryC"
               argv_xml2[1] <- prg
               wrk_out <- .C(C_xml2any_main, as.integer(argc_xml2), argv_xml2, outfile, nref_out = n_xml2)
   #browser()

               if(outformat != "Rstyle"){
                   # bibe <- source(outfile)$value # TODO: is the return value of source()
                   #                               #       'official'? (no, it isn't)
                   # names(bibe) <- unlist(bibe$key)

                   bibe <- readBibentry(outfile)
                   
                   if(outformat == "bibentry"){
                       saveRDS(bibe, outfile)
                   }else{ # R   TODO: (2020-11-07) now it could just return the outfile!!!
                       writeBibentry(bibe, outfile, style = "loose")
                   }
                   wrk_out <- list(bib = bibe, nref_out = length(bibe))
               }# else - do nothing if outformat == "Rstyle")
           },
           {
               ## default
                   # stop("outformat ", outformat, " not supported by bibConvert yet")

               ## earlier versions accepted "word" (in the "C" code)
               if(outformat == "word")
                   outformat <- "wordbib"
               
               prg <- paste0("xml2", outformat)
               argv_xml2[1] <- prg
               wrk_out <- .C(C_xml2any_main, argc_xml2, argv_xml2, outfile, nref_out = n_xml2)
           }
           )

    if(is.numeric(wrk_out$nref_out) && wrk_out$nref_out == 0)
        message("\nno references to output.\n",
                "if this is wrong, consider using argument 'informat'.\n")

    wrk <- list("infile" = infile, "outfile" = outfile,
                nref_in = wrk_in$nref_in,
                nref_out = wrk_out$nref_out
                )
    if(!is.null(wrk_out$bib))
       wrk <- c(wrk, list(bib = wrk_out$bib))
    
    wrk
}
