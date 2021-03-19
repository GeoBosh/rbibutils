readBib <- function(file, encoding = NULL, ..., direct = FALSE,
                    texChars = c("keep", "convert", "export")){

    if(is.null(encoding))
        encoding <- c("utf8", "utf8")  # would default input 'native' be better?
    else {
        encoding <- ifelse(encoding == "UTF-8", "utf8", encoding)
        if(length(encoding) == 1)
            encoding <- c(encoding, "utf8")
    }

    
    if(!direct){
        ## to make sure that the old behaviour before adding arguments is kept.
        ##     TODO: relax and coordinate with direct = TRUE later
        stopifnot(length(list(...)) == 0) # no ... arguments allowed

        bib <- tempfile(fileext = ".bib")
        on.exit(unlink(bib))

        be <- bibConvert(file, bib, "bibtex",
                         "bibentry", encoding = encoding, tex = "no_latex")
        return(be$bib)
    }

    ## direct is TRUE below
    texChars <- match.arg(texChars)
    switch(texChars,
           convert = {
               tex <- "no_latex"
           },
           keep = {
               ## this will need separate no_latex option for infile and outfile.
               #stop(" 'texChars = keep' not implemented yet")
               tex <- c("keep_tex_chars", "no_latex")
           },
           ## export
           ## default
           tex <- NULL
           )
    
    bibtexImport(file, encoding = encoding, tex = tex)
}

writeBib <- function(object, con = stdout(), append = FALSE){
    if(!inherits(object, "bibentry"))
        stop("'object' must inherit from class 'bibentry'.")
    
    mode <- if(append) "a" else "w+"

    if (is.character(con)) {
        con <- file(con, open = mode)
        on.exit(close(con))
    }

    lines <- toBibtex(object)
    writeLines(lines, con)
             
    invisible(object)
}


## readBib_legacy <- function(file, encoding){
##     rds <- tempfile(fileext = ".rds")
##     on.exit(unlink(rds))
## 
##     if(encoding == "UTF-8")
##         encoding = "utf8"
##     
##     be <- bibConvert(file, rds, "bibtex",
##             "bibentry", encoding = c(encoding, "utf8"), tex = "no_latex")
##     res <- readRDS(rds)
## 
##     res
## }
