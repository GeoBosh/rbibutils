readBib <- function(file, encoding){
    bib <- tempfile(fileext = ".bib")
    on.exit(unlink(bib))

    if(encoding == "UTF-8")
        encoding = "utf8"
    
    be <- bibConvert(file, bib, "bibtex",
                     "bibentry", encoding = c(encoding, "utf8"), tex = "no_latex")
#browser()
    be$bib
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
