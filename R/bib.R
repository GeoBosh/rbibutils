readBib <- function(file, encoding = NULL, ..., direct = FALSE,
                    texChars = c("keep", "convert", "export", "Rdpack"),
                    macros = NULL, extra = FALSE, key, fbibentry = NULL){

    if(is.null(encoding))
        encoding <- c("utf8", "utf8")  # would default input 'native' be better?
    else {
        encoding <- ifelse(encoding == "UTF-8", "utf8", encoding)
        if(length(encoding) == 1)
            encoding <- c(encoding, "utf8")
    }

    if(is.null(macros)){
        if(!file.exists(file))
            stop("file '", file, "' doesn't exist")
    }else{
        fn <- tempfile(fileext = ".bib")
                                        # TODO: changing this to try to fix a strange error on Windows,
                                        #       though it turns out that
                                        #       bib/litprog280macros_only.bib is not there during 'R CMD check'
                                        # for(s in c(macros, file))
                                        #     if(!file.append(fn, s))
                                        #         stop("could not copy file ", s)
        files <- c(macros, file)
        exist_flags <- file.exists(files)
        if(any(!exist_flags)){
            stop("files  ", paste(files[!exist_flags], collapse = ", "), " do not exist")
        }
        
        if(!file.copy(files[1], fn, overwrite = TRUE))
            stop("could not copy file ", files[1], "\nto destination")
        for(s in files[-1])
            if(!file.append(fn, s))
                stop("could not copy file ", s)
        
        file <- fn
    }
    
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
                   ## was: tex <- "no_latex"
                   tex <- c("convert_latex_escapes", "no_latex")
               },
               keep = {
                   ## this will need separate no_latex option for infile and outfile.
                   #stop(" 'texChars = keep' not implemented yet")
                   tex <- c("keep_tex_chars", "no_latex")
               },
               Rdpack = {
                   ## like 'keep' but patches for Rdpack, see issue #7 in rbibutils
                   tex <- c("keep_tex_chars", "no_latex", "Rdpack")
               },
               export = {
                   tex <- c("export_tex_chars")
               },
               ## default
               tex <- NULL
               )

        res <- bibtexImport(file, encoding = encoding, tex = tex, extra = extra,
                            fbibentry = fbibentry)
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



####### bibstyle

bibstyle_JSSextra <- local({
    JSSextra <- NULL
    
    function(reset = FALSE, parent_style = "JSS"){
        if(!is.null(JSSextra) && !reset)
            return(JSSextra)
        
        JSSextra <<- tools::bibstyle(parent_style, .default = FALSE)

        with(JSSextra, {
            sortKeys <- function (bib) {
                result <- character(length(bib))
                for (i in seq_along(bib)) {
                    authors <- authorList(bib[[i]])
                    if (!length(authors)) 
                        authors <- editorList(bib[[i]])
                    if (!length(authors)) 
                        authors <- ""
                    year <- collapse(bib[[i]]$year)
                    authyear <- if(authors != "" )
                                    paste0(authors, ", ", year)
                                else
                                    year
                    result[i] <- authyear
                }
                result
            }
        
            fmtTitleNoPeriod <- function(title){
                if (length(title)) {
                    title <- gsub("%", "\\\\\\%", title)
                    paste0("\\dQuote{", collapse(cleanupLatex(title)), "}")
                }
            }
        
            ## TODO: this is a patch to make this safely reentrable
            if(!exists("formatMiscJSS"))
                formatMiscJSS <- formatMisc

            ## TODO: more work needed here
            formatMisc <- function(paper) {
                if(is.null(paper$year) && !is.null(paper$date)){
                    if(grepl("^[0-9][0-9][0-9][0-9]", paper$date))
                        paper$year <- substr(paper$date, 1, 4)
                }
                
                if(is.null(paper$truebibtype))
                    ## default copy of JSS's formatMisc
                    formatMiscJSS(paper)
                else{
                    switch(paper$truebibtype,
                           Periodical = { # TODO: this is to get things going
                               collapse(c(
                                   fmtPrefix(paper),
                                   paste0(fmtTitleNoPeriod(paper$title),
                                           fmtYear(paper$year)),
                                   paste0(paper$paper$editor),
                                   editorList(paper),
                                   sentence(procOrganization(paper)),
                                   paste0(paper$paper$publisher),
                                   #paste0(paper$address),        
                                   sentence(fmtISSN(paper$issn), extraInfo(paper)),
                                   paste0(paper$organization),
                                   #paste0(paper$note),
                                   #paste0(paper$acknowledgement),
                                   #paste0(paper$key),
                                   #paste0(paper$bibdate),
                                   paste0(paper$bibsource)
                               ))
                           },
                           Article = {
                               if(is.null(paper$journal))
                                   paper$journal <- paper$journaltitle
                               formatArticle(paper)
                           },
                           Book = {
                               formatBook(paper)
                           },
                           # Booklet = {
                           #     formatBook(paper)
                           # },
                           InBook = {
                               formatInBook(paper)
                           },
                           InCollection = {
                               formatInCollection(paper)
                           },
                           InProceedings = {
                               formatInProceedings(paper)
                           },
                           Manual = {
                               formatManual(paper)
                           },
                           MastersThesis = {
                               formatMastersThesis(paper)
                           },
                           Misc = {
                               formatMisc(paper)
                           },
                           PhdThesis = {
                               formatPhdThesis(paper)
                           },
                           Proceedings = {
                               formatProceedings(paper)
                           },
                           TechReport = {
                               formatTechReport(paper)
                           },
                           Unpublished = {
                               formatUnpublished(paper)
                           },
                           ## default - TODO,
                           {
                               collapse(c(fmtPrefix(paper),
                                          sentence(authorList(paper), fmtYear(paper$year), sep = " "),
                                          fmtTitle(paper$title),
                                          sentence(fmtHowpublished(paper$howpublished)),
                                          sentence(extraInfo(paper))))
                           }
                           )
                }
            }
        })
        
        JSSextra
    }
})

register_JSSextra <- function(make_default = FALSE, reset = FALSE, parent_style = "JSS"){
    tools::bibstyle("JSSextra", envir = bibstyle_JSSextra(), .default = make_default)
}
