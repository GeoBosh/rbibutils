readBib <- function(file, encoding = NULL, ..., direct = FALSE,
                    texChars = c("keep", "convert", "export"), macros = NULL, key){

    if(is.null(encoding))
        encoding <- c("utf8", "utf8")  # would default input 'native' be better?
    else {
        encoding <- ifelse(encoding == "UTF-8", "utf8", encoding)
        if(length(encoding) == 1)
            encoding <- c(encoding, "utf8")
    }

    if(!is.null(macros)){
        fn <- tempfile(fileext = ".bib")
            # for(s in c(macros, file))
            #     if(!file.append(fn, s))
            #         stop("could not copy file ", s)
        files <- c(macros, file)
        
        if(!file.copy(files[1], fn, overwrite = TRUE))
            stop("could not copy file ", files[1], " to destination")
        for(s in files[-1])
            if(!file.append(fn, s))
                stop("could not copy file ", s)
                
        file <- fn
    }
#browser()    
    if(!direct){
        ## to make sure that the old behaviour before adding arguments is kept.
        ##     TODO: relax and coordinate with direct = TRUE later
        stopifnot(length(list(...)) == 0) # no ... arguments allowed

        bib <- tempfile(fileext = ".bib")
        on.exit(unlink(bib))

        be <- bibConvert(file, bib, "bibtex",
                         "bibentry", encoding = encoding, tex = "no_latex")
        res <- be$bib
    }else{ ## direct is TRUE below
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
        
        res <- bibtexImport(file, encoding = encoding, tex = tex)
    }

    if(!missing(key)){
        ind <- which(grepl("dummyid", names(res)))
        if(length(ind) > 0  &&  length(ind) != length(key)){
            stop("length of 'key' is not equal to the number of keyless entries")
        }else if(length(ind) > 0){
            for(i in 1:length(ind))
                res[ind[i]]$key <-  key[i]
            ## TODO: it seems necessary to do also this. Investigate for a simpler approach.
            names(res)[ind] <- key
        }
    }
    
    res
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

charToBib <- function(text, informat, ...) {
    fn <- tempfile()
    writeLines(text, fn)
    on.exit(unlink(fn))
    
    res <- if(missing(informat))
               readBib(fn, ...)
           else
               bibConvert(fn, informat = informat, ...)
    res
}
