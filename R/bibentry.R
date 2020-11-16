readBibentry <- function(file){
    exprs <- parse(n = -1, file = file, srcfile = NULL, keep.source = FALSE,
                   encoding = "UTF-8")    # TODO: fixed encoding for now

    if(length(exprs) == 1){
        res <- try(eval(exprs))
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
    for (i in seq_along(exprs)){
        wrk[[i]] <- tryCatch(eval(exprs[[i]], envir = envir),
                             error = function(e){
                                 txt <- if(is.null(exprs[[i]]$key))
                                            paste(as.character(exprs[[i]]), collapse = ", ")
                                        else
                                            paste0("key '", exprs[[i]]$key, "'")
                                     
                                 mess <- paste0(txt, "\n      ",
                                                geterrmessage() )
                                 caution <<- c(caution, mess)
                                 NA
                             }
                             ## ,
                             ##  warning = function(w){
                             ##      caution <<- c(caution, w)
                             ##      NA
                             ##  }
                             )
    }
    if(length(caution) > 0) {
        ind <- sapply(wrk, function(x) identical(x, NA))
        wrk <- wrk[!ind]
        for(i in seq_along(caution))
            warning(caution[[i]])
    }
    
    res <- do.call("c", wrk)
    names(res) <- unlist(res$key) # TODO: what if 'key' is missing in some entries?  (this
                                        # cannot happen for the output of bibConvert() though)
                                        # If you change this, don't forget
                                        # to do it also for the return statement earlier
                                        # in this function!
    res
}

writeBibentry <- function(be, file, style = c("Rstyle", "loose")){
    style <- match.arg(style)

    con <- file(file, "wt")
    on.exit(close(con))

    sink(con)
    ## on.exit(sink(), add = TRUE)

    if(style == "Rstyle"){
        print(be, style = "R")
    }else{ # "loose"
        for(i in seq_along(be)){
            print(be[i], style = "R")
            cat("\n")
        }
    }
   
    sink()
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
