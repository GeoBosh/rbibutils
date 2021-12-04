readBibentry <- function(file, extra = FALSE){
    ## TODO: fixed encoding for now, but:
    ##   It is hardly worth the bother to consider other encodings.
    ##   First, bibConvert can produce 'file' in UTF-8.
    ##       Second, the argument 'encoding' of parse only asks it to mark the input with that
    ##       encoding, it does not re-encode. The only other acceptable value is "latin1".
    ##   Third, to allow other encodings 'file' in the call below, needs to be declared as a
    ##       connection with from/to encodings for iconv().
    exprs <- parse(n = -1, file = file, srcfile = NULL, keep.source = FALSE,
                   encoding = "UTF-8")
    
    if(length(exprs) == 1){
        res <- try(eval(exprs), silent = TRUE)
        if(!inherits(res, "try-error")) { # TODO: check that it is bibentry?
            names(res) <- unlist(res$key)
            return(res)

        } else if(identical(exprs[[1]][[1]], as.name("c")))
            exprs <- exprs[[1]][-1]  # drop enclosing c()
    }

    envir <- environment()              # for (i in seq_along(exprs))  eval(exprs[i], envir)
    n <- length(exprs)
    wrk <- vector("list", n)
    caution <- list()
    ind_caution <- numeric(0)
    for (i in seq_along(exprs)){
        ## I collect the messages and at the end of the function print more suitable ones.
        
                                     ## wrk[[i]] <- tryCatch(eval(exprs[[i]]),
        wrk[[i]] <- tryCatch(eval(exprs[[i]], envir = envir),
                             error = function(e){
                                 txt <- if(is.null(exprs[[i]]$key))
                                            paste(as.character(exprs[[i]]), collapse = ", ")
                                        else
                                            paste0("key '", exprs[[i]]$key, "'")
                                 
                                 mess <- paste0(txt, "\n      ", geterrmessage() )
                                 caution <<- c(caution, mess)
                                 ind_caution <<- c(ind_caution, i)
                                 NA
                             }
                             ## ,
                             ##  warning = function(w){
                             ##      caution <<- c(caution, w)
                             ##      NA
                             ##  }
                             )
    }

    extraflag <- FALSE
    if(length(caution) > 0) {
        if(extra){
            for(j in seq_along(caution)){
                i <- ind_caution[j]
                bibtype_flag <- grepl("bibtype", caution[[j]]) &&
                    (grepl("has to be one of ", caution[[j]]) ||
                     ## has to specify the field: or (plural) has to specify the fields:
                     grepl("has to specify the field", caution[[j]]))
                if(bibtype_flag){
                    modbib <- exprs[[i]]
                    oldtype <- modbib$bibtype
                    modbib$bibtype <- "Misc"
                    
                    miscbib <- try(eval(modbib, envir = envir), silent = TRUE)  # simple 'try' for now
                        # miscbib <- tryCatch(eval(modbib, envir = envir), error = print("Hello!"))
                    if(inherits(miscbib, "try-error")){
                        caution[[j]] <- paste0(caution[[j]],
                                               "\n  NOT FIXED.")
                        next
                    }
                    
                    curbib <- unclass(miscbib)
                       # curbib$bibtype <- oldtype # no, bibtype is attribute!
                    attr(curbib[[1]], "bibtype") <- oldtype
                    class(curbib) <- "bibentry"
                    wrk[[i]] <- curbib
                    ## 2021-10-16 was:  caution[[j]] <- ""  # success, no need for the message
                    caution[[j]] <- paste0("\nMessage: ", caution[[j]],
                                "\n  FIXED (the returned object will be 'bibentryExtra').\n")
                    extraflag <- TRUE
                }else{
                        caution[[j]] <- paste0(caution[[j]], "\n  NOT FIXED.")
                }
            }

        }

        ## message("\nTried to fix above errors/warnings, see the warnings and messages below.\n")
        for(i in seq_along(caution))
            if(caution[[i]] != ""){
                if(grepl("NOT FIXED", caution[[i]]))
                    warning(caution[[i]])
                else
                    message(caution[[i]])
            }
    }


    ind <- sapply(wrk, function(x) identical(x, NA))
    wrk <- wrk[!ind]

    if(length(wrk) > 0){       # wrk is list of bibentry objects or list()
        res <- do.call("c", wrk)
        names(res) <- unlist(res$key) # TODO: what if 'key' is missing in some entries?  #
                                      #    (this cannot happen for the output of bibConvert()
                                      #    though). If you change this, don't forget to do it
                                      #    also for the return statement earlier in this
                                      #    function!
    } else
        res <- bibentry()

    if(extraflag)
        class(res) <- c("bibentryExtra", class(res))

    res
}

writeBibentry <- function(be, file, style = c("Rstyle", "loose")){
    style <- match.arg(style)

    con <- file(file, "wt")
    sink(con)
    on.exit({sink(); close(con)})

    if(style == "Rstyle"){
        print(be, style = "R")
    }else{ # "loose"
        for(i in seq_along(be)){
            print(be[i], style = "R")
            cat("\n")
        }
    }
    
    invisible()
}

## readBibentry <- function(file){
##     expr <- parse(file, encoding = "UTF-8") # NOTE: fixed encoding for now
## 
##     fu <- function(){
##         .allval <- vector(length(expr), mode = "list")
##         for(.i in seq_along(expr)){
##             .val <- eval(expr[.i])
##             .allval[[.i]] <- if(is.null(.val))
##                                NA
##                            else
##                                .val
##         }
##         .bibflag <- sapply(.allval, function(x) inherits(x, "bibentry"))
##         .wrk <- .allval[.bibflag]
##         .vars <- mget(ls())
##         if(length(.vars) > 0){
##             .bibflag <- sapply(.vars, function(x) inherits(x, "bibentry"))
##             .vars <- .vars[.bibflag]
##             if(length(.vars) > 0)
##                 .wrk <- c(.vars, .wrk)
##         }
##         
##         do.call("c", .wrk)
##     }
##     
##     fu()
## }

.mangle_nonstandard_types <- function(x, all_types = FALSE){
    if(all_types){
        tbt <- x$truebibtype
        flags <- (unlist(x$bibtype) != "Misc") &
                 if(length(tbt) > 1) sapply(x$truebibtype, is.null) else is.null(x$truebibtype)
    }else{
            # TODO: non-rigid  way to do this?
            #    cat("c(", paste(paste0('"', names(tools:::BibTeX_entry_field_db), '"'),
            #                    collapse = ", "), ")")
        stdbibtypes <- c( "Article", "Book", "Booklet", "InBook", "InCollection",
                         "InProceedings", "Manual", "MastersThesis", "Misc", "PhdThesis",
                         "Proceedings", "TechReport", "Unpublished" )

        flags <- !(unlist(x$bibtype) %in% stdbibtypes)
    }
        
    y <- unclass(x[flags])
    y <- lapply(y, function(s){
                       s$truebibtype <- attr(s, "bibtype")
                       attr(s, "bibtype") <- "Misc"
                       s
                   })
    class(y) <- "bibentry"
    class(x) <- "bibentry"
    x[flags] <- y
    x
}

## sapply(tmp2$"issn-l", function(x) !is.null(x))
.unmangle_nonstandard_types <- function(x){
    ## TODO: non-rigid  way to do this?
    ## cat("c(", paste(paste0('"', names(tools:::BibTeX_entry_field_db), '"'), collapse = ", "), ")")
    stdbibtypes <- c( "Article", "Book", "Booklet", "InBook", "InCollection", "InProceedings", "Manual",
                     "MastersThesis", "Misc", "PhdThesis", "Proceedings", "TechReport", "Unpublished" )

#browser()
    flags <- unlist(x$bibtype) == "Misc"  &  sapply(x$truebibtype, function(x) !is.null(x))
    if(!any(flags))
        return(x)
    
    y <- unclass(x[flags])
    y <- lapply(y, function(s){ attr(s, "bibtype") <- s$truebibtype; s[["truebibtype"]] <- NULL; s }, USE.NAMES = FALSE)
    #class(y) <- "bibentry"
    y
}

print.bibentryExtra <- function(x, style = "text", .bibstyle = "JSSextra", ...){
    wrk <- format(x, style = style, .bibstyle = .bibstyle, ...)
    cat(wrk, sep = "\n\n")
    invisible(x)
}

#.be_R <- function(text){
#    expr <- parse(text = text)
#    stopifnot(mode(expr[[1]][[1]]) == "name"  &&  as.character(expr[[1]][[1]]) == "c")
#
#    wrk <- expr[[1]]
#    for(i in seq_along(wrk)[-1]){ # start from i=2
#        if(!is.null(truetype <- wrk[[i]]$truebibtype)){
#            wrk[[i]]$bibtype <- truetype
#            wrk[[i]]$truebibtype <- NULL
#        }
#    }
#    expr[[1]] <- wrk
#    
#browser()
#    
#
#    
#    res
#}


## These can piggyback on the bibentry methods
## 
## `[[.bibentryExtra`
## `[.bibentryExtra`
##
## `$.bibentryExtra`

## utils:::bibentry_attribute_names is not exported
.bibentry_attribute_names <- 
    c("bibtype", "textVersion", "header", "footer", "key")

## utils:::bibentry_list_attribute_names
.bibentry_list_attribute_names <- 
    c("mheader", "mfooter")


`[[.bibentryExtra` <- function(x, i, j, drop = TRUE){

    ## TODO: make this method similar to `[[<-.bibentryExtra` (using a list, instead of 'j') ?

    ## if(!length(x)) return(x)
    
    Narg <- nargs() - !missing(drop)
    j.omitted   <- Narg >= 3  && missing(j)
    j.has.value <- !missing(j)

    if(missing(j) && !j.omitted){
        if(is.list(i)){
            ## this is for symmetry with `[[<-` which cant' have argument j
            if(length(i) == 2){
                j.has.value <- TRUE
                j <- i[[2]]
                i <- i[[1]]
            }else
                stop("if 'i' is a list it should have length 2")
        }else{
            x <- NextMethod()
            return(x)
        }
    }

    if(length(i) != 1)
        stop("length of i should be 1 when j is not missing or omitted")

    res <- unclass(x)
    res <- res[[i]]

    if(j.has.value){
        if(!is.character(j))
            stop("j mist be character or omitted")
        chind <- intersect(j, names(res))   # !all(j %in% names(res))
        a <- attributes(res)
        res <- res[chind] # empty list if chind is character(0)
        if(!drop){
            ## attr(res, "bibtype") <- a$bibtype
            ## attr(res, "key") <- a$key
            attributes(res)[.bibentry_attribute_names] <- a[.bibentry_attribute_names]
        }
    }

    if(drop && length(res) == 1)
        structure(res[[1]], names = names(res)) # trying to keep the name
    else
        res
}

`[.bibentryExtra` <-
function(x, i, j, drop = TRUE)
{
    mdrop <- missing(drop)
    Narg <- nargs() - !mdrop
    j.omitted <- Narg >= 3  && missing(j)

    ## argument drop is (currently?) used only by the inherited bibentry method
    if(missing(j) && !j.omitted){
        x <- NextMethod()
        return(x)
    }

    cl <- class(x)
    res <- unclass(x)
    if(missing(i))
        i <- seq_along(res)
    else if(is.character(i) && is.null(names(x))){
        names(res) <- sapply(x$key, function(y) if(is.null(y)) "" else y)
    }
    res <- res[i]

    if(!missing(j)){
        ## (:TODO:) TO CONSIDER:
        ##
        ## The result (bibentryExtra object) may be missing complsory fields.
        ## Note that interactively printing the result will show informative messages.
        ##
        ## Should this be allowed? -- it enables incrementally building reference(s) and keeps the key and bibtype.
        ##  
        if(!is.character(j))
            stop("j mist be character or omitted")
        for(ind in seq_along(res)){
            wrk <- res[[ind]]
            chind <- intersect(j, names(wrk))
            ## TODO: more care with attributes
            a <- attributes(wrk)

            wrk <- wrk[chind] # empty list if chind is character(0)
            attributes(wrk)[.bibentry_attribute_names] <- a[.bibentry_attribute_names]
            
            res[[ind]] <- wrk
            
        }
    }
    class(res) <- cl
    res
}


                              # TODO: change the default .bibstyle when the new style is ready
format.bibentryExtra <- function (x, style = "text", .bibstyle = NULL, ...){
    ## ... contains further arguments for format.bibentry

    ## TODO: register JSSextra in onLoad ?
    if(!is.null(.bibstyle) && .bibstyle == "JSSextra"  &&
       !("JSSextra" %in% getBibstyle(TRUE)))
        register_JSSextra()
    
    x <- .mangle_nonstandard_types(x, TRUE)
   
    wrk <- NextMethod() # not very sure if this is the way to call it here

    if(style == "R"){
        ## ## patch
        ## wrk <- capture.output(cat(wrk, sep="\n"))
        
        ## 2021-12-04: dropping the final $
        ##   begpat <- "^[[:space:]]*c?\\(?bibentry\\(bibtype[[:space:]]*=[[:space:]]*\"([^\"]+)\",[[:space:]]*$"
        begpat <- "^[[:space:]]*c?\\(?bibentry\\(bibtype[[:space:]]*=[[:space:]]*\"([^\"]+)\",[[:space:]]*"
        starts <- which(grepl(begpat, wrk))
        b <- regexec(begpat, wrk)

        ## this assumes that truebibtype is not the last printed element, so changing it
        ind_truetype <- 2
        #endpat <- "^[[:space:]]*truebibtype[[:space:]]*=[[:space:]]*\"([^\"]+)\",[[:space:]]*$"
        #ind_truetype <- 3
        ## 2021-12-04: dropping the ^ at the beginning
        ##     endpat <- "^[[:space:]]*truebibtype[[:space:]]*=[[:space:]]*\"([^\"]+)\"[[:space:]]*([,|\\)])"
        endpat <- "[[:space:]]*truebibtype[[:space:]]*=[[:space:]]*\"([^\"]+)\"[[:space:]]*([,|\\)])"

        ends <- which(grepl(endpat, wrk))
        e <- regexec(endpat, wrk)

        if(length(starts) < length(ends))
            stop("mismatch between starts and ends")
        else if(length(starts) > length(ends)){
            bmatched <- numeric(length(ends))
            for(k in ends){
                prev <- starts[starts < ends[k]]
                bmatched[k] <- prev[length(prev)]
            }
            starts <- bmatched
        }

        todrop <- integer(0)
        for(i in seq_along(starts)){
            ecur <- e[[ends[i]]]
            pos <- ecur[2] + c(0, attr(ecur, "match.length")[2] - 1)
            truetype <- substr(wrk[ends[[i]]], pos[1], pos[2])
            chafter <- substr(wrk[ends[[1]]], ecur[3], ecur[3] )  # ecur[3] + attr(ecur, "match.length")[3] - 1)
            
            ## restore the true type of the bib entry
            bcur <- b[[starts[i]]]
            bpos <- bcur[2] + c(0, attr(bcur, "match.length")[2] - 1)
            faketype <- substr(wrk[starts[[i]]], bpos[1], bpos[2])

            begline <- paste0(substr(wrk[starts[[i]]], 1, bpos[1] - 1),
                              truetype,
                              substring(wrk[starts[[i]]], bpos[2] + 1)  )
            wrk[starts[[i]]] <- begline

            ## remove field 'truetype' (it is for internal use).
            ##
            ## This will work if the commented out
            ##      wrk <- capture.output(cat(wrk, sep="\n"))
            ## (see further above) is reinstated.
            ## It doesn't remove the comma from the line before though.
            ##
            ## TODO: However, all this is very convoluted and should be rethought completely.
            ##      Also, there may be a case not to restore the true type.
            ##
            ## rest <- substring(wrk[ends[[i]]], ecur[3] + 1)
            ## if(chafter == ","){
            ##     if(nchar(rest) == 0  || grepl("^[[:space:]]+$", rest))
            ##         todrop <- c(todrop, ends[[i]]) # mark for deletion
            ##     else{ # ")"
            ##         ## keep the indentation
            ##         ## leadws <- nchar(strsplit(wrk[ends[[1]]], "[^ ]+")[[1]][1])
            ##         wrk[ends[[i]]] <- paste0(strsplit(wrk[ends[[i]]], "[^ ]+")[[1]][1], rest)
            ##     }
            ## }else{ # ")"
            ##     ## keep the indentation
            ##     ## leadws <- nchar(strsplit(wrk[ends[[1]]], "[^ ]+")[[1]][1])
            ##     wrk[ends[[i]]] <- paste0(strsplit(wrk[ends[[i]]], "[^ ]+")[[1]][1], chafter, rest)
            ## }
        }
#browser()
        if(length(todrop) > 0)
            wrk <- wrk[-todrop]
    }else if(style == "bibtex"){

               # "^[[:space:]]*@([^\"]+)[[:space:]]*\{.*([[:space:]]*truetype = "
        pat_bibtype <- "^[[:space:]]*@([^{ ]+)"
        pat_truebibtype <- "[[:space:]]+truebibtype = \\{([^}\"]*)\\},?"
        
        wrk <- sapply(wrk,
                      function(be){
                          if(grepl(pat_truebibtype, be)){
                              match_bibtype <- regexec(pat_bibtype, be)
                              ecur <- match_bibtype[[1]]
                              ind_bt <- ecur[2] + c(0, attr(ecur, "match.length")[2] - 1)
                              bt <- substr(be, ind_bt[1], ind_bt[2])
                              cat("bt = ", bt, "\n")
                              
                              match_truebibtype <- regexec(pat_truebibtype, be)
                              ecur <- match_truebibtype[[1]]
                              ind_tbt <- ecur[2] + c(0, attr(ecur, "match.length")[2] - 1)
                              tbt <- substr(be, ind_tbt[1], ind_tbt[2])
                              cat("tbt = ", tbt, "\n")

                              ## drop field truebibtype
                              be <- paste0(substr(be, 1, match_truebibtype[[1]][1] - 1),
                                           substring(be, match_truebibtype[[1]][1] +
                                                         attr(match_truebibtype[[1]],
                                                              "match.length")[1]))
                              ## replace the internal bib type with the true one.
                              be <- paste0("@", tbt, substring(be, ind_bt[2] + 1))

                              # browser()
                              
                              be
                          }else
                              be
                      }
                      )
        wrk <- as.vector(wrk) # drop the attributes (TODO: maybe should leave them, they come
                              # from nextMethod(), so maybe they are there for a reason?
    }

    wrk
}

`$<-.bibentryExtra` <- function(x, name, value){
    cl <- class(x)
    x <- .mangle_nonstandard_types(x)
    x <- NextMethod()
    x <- .unmangle_nonstandard_types(x)
    class(x) <- cl
    x
}

`[[<-.bibentryExtra` <- function(x, i, value){

    cl <- class(x)
    res <- unclass(x)

    if(inherits(value, "bibentry")){   # bibentryExtra ?
        if(length(value) != 1)
            stop("value should contain exactly one bib reference")
        if(length(i) != 1)
            stop("i should have length 1")
        
        wrk <- unclass(value)[[1]]  # drop the enclosing list
            # if(is.null(attr(wrk, "key"))){
            #     message("replacement value does not specify a cite key\n\tinserting a dummy one")
            #     attr(wrk, "key") <- "dummy"
            # }
            # if(is.null(attr(wrk, "bibtype"))){
            #     message("replacement value does not specify bibtype\n\tinserting Misc")
            #     attr(wrk, "bibtype") <- "Misc"
            # }
        
#browser()        
        res[[i]] <- wrk
        
    }else if(is.list(i)){   # value should be a list of named fields in this case or a
                            # character vector of the same length as i[[2]]
        stopifnot(length(i) == 2)
        target_fields <- i[[2]]
        i <- i[[1]]
        
        if(is.character(target_fields)) {
            fields <- names(value)
            if(length(fields) == 0){
                if(length(target_fields) == length(value) && target_fields != "*"){
                    names(value) <- target_fields
                    fields <- target_fields
                }else
                    stop("unsuitable 'i' and/or 'value'")
            }
            if(length(target_fields) == 1  && target_fields == "*") {
                ## add all fiedls from 'value'
                for(field in fields){
                    res[[i]][[field]] <- value[[field]] 
                }
            }else{
                ## add only fields in target_fields
                for(field in intersect(target_fields, fields)){
                    res[[i]][[field]] <- value[[field]] 
                }
            }
        }else{
            stop("argument 'target_fields should be a character vector")
        }
    }else{
        stop("incompatible arguments: 'value' and 'i'")
    }
    
    class(res) <- cl
    res
}



## c.bibentryExtra
## toBibtex.bibentry

## sort.bibentry

## rep.bibentry
## unique.bibentry


## todo: temporary, for 
toRd.bibentryExtra <- function(obj, style="JSSextra", ...) {

    ## R/src/library/tools/R/bibstyle.R
    ## 
    ## obj <- sort(obj, .bibstyle=style)
    ## style <- bibstyle(style, .default = FALSE)
    ## env <- new.env(hash = FALSE, parent = style)
    ## bib <- unclass(obj)
    ## result <- character(length(bib))
    ## for (i in seq_along(bib)) {
    ## 	env$paper <- bib[[i]]
    ## 	result[i] <- with(env,
    ## 	    switch(attr(paper, "bibtype"),
    ## 	    Article = formatArticle(paper),
    ## 	    Book = formatBook(paper),
    ## 	    InBook = formatInbook(paper),
    ## 	    InCollection = formatIncollection(paper),
    ## 	    InProceedings = formatInProceedings(paper),
    ## 	    Manual = formatManual(paper),
    ## 	    MastersThesis = formatMastersthesis(paper),
    ## 	    Misc = formatMisc(paper),
    ## 	    PhdThesis = formatPhdthesis(paper),
    ## 	    Proceedings = formatProceedings(paper),
    ## 	    TechReport = formatTechreport(paper),
    ## 	    Unpublished = formatUnpublished(paper),
    ## 	    paste("bibtype", attr(paper, "bibtype"),"not implemented") ))
    ## }
    ## gsub("(^|[^\\])((\\\\\\\\)*)%", "\\1\\2\\\\%", result)

    obj <- .mangle_nonstandard_types(obj)
    res <- NextMethod()
    res
}

bibentryExtra <- function(x, ...){
    if(!inherits(x, "bibentry"))
        stop("currently the argument is required to be a bibentry object ")

    class(x) <- c("bibentryExtra", class(x))
    x
}

