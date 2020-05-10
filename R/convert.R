bibConvert <- function(infile, outfile, informat, outformat){
    argc <- 2L
    argv <- c("dummy", infile)

    ## sink(outfile)
    ## on.exit(sink())

    xmlfile <- tempfile(fileext = ".xml")
    
    wrk <- .C(C_biblatex2xml_main, argc, argv, xmlfile)

    if(outformat == "bib"){
        argv <- c("dummy", xmlfile)
        ##     xml2bib_main( int *argc, char *argv[], char *outfile[] )
        wrk <- .C(C_xml2bib_main, argc, argv, outfile)
    }
    
    wrk
}
