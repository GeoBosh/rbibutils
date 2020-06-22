## read a mods xml file
read_mods <- function(x, ..., strip_ns = TRUE, modsCollection = TRUE){
    col <- read_xml(x, ...)
    if(xml_name(xml_root(col)) != "modsCollection"){
        msg <- paste0("xml file '", x, "' is not modsCollection")
        if(modsCollection)
            stop(msg)
        else
            message(msg)
    }
    if(strip_ns)
        xml_ns_strip(col)
    col
 }

## parse a mods collection to  bib-like structure
bibmods <- function(col){ # col - object returned by read_mods
    mods <- xml_find_all(col, ".//mods")
    ids <- xml_attr(mods, "ID")
    ## xml_length(mods) # number of children of each element

    res <- vector(length(ids), mode = "list")
    names(res) <- ids
    for(i in seq_along(ids)){
        res[[i]] <- process_mods(mods[[i]])
    }

    structure(res, class = "bibmods")
}


.name_piece <- function(cur, type, new){
    cur[[type]] <- if(is.null(cur[[type]]))
                       new
                   else
                       c(cur[[type]], new)
    cur
}

.add_field <- function(res, field, fieldname){
    if(is.null(res[[fieldname]]))
        xml_text(field)
    else
        c(res[[fieldname]], xml_text(field))
}



## names(tools:::BibTeX_entry_field_db)
## #:  [1] "Article"       "Book"          "Booklet"       "InBook"       
## #:  [5] "InCollection"  "InProceedings" "Manual"        "MastersThesis"
## #:  [9] "Misc"          "PhdThesis"     "Proceedings"   "TechReport"   
## #: [13] "Unpublished"  

mods_name <- function(field){
    stopifnot(xml_name(field) == "name")

    ## person(given = NULL, family = NULL, middle = NULL,
    ##        email = NULL, role = NULL, comment = NULL,
    ##        first = NULL, last = NULL)
    personal <- list()
    pieces <- xml_children(field)
    ## note: xml_length() gives the lengths of the children, not the number of children
    for(j in seq_along(xml_length(pieces))){
        piece <- pieces[[j]]
        switch(xml_name(piece),
               namePart = {
                   type <- xml_attr(piece, "type")
                   if(type %in% c("given", "family")){
                       personal <- .name_piece(personal, type, xml_text(piece))
                   }else if(type == "email")
                       personal <- .name_piece(personal, type, xml_text(piece))
                   else
                       message("namePart", type, " not implemented yet for persons")
               },
               role = {
                   role <- xml_text(piece)
                   role <- substr(role, 1, 3)
                   personal <- .name_piece(personal, "role", role )
               },
               ## default
               stop("unknown element ", xml_name(piece), " in 'name'")
               )
    }
    prs <- do.call("person", personal)
#browser()         
    ##    persons <- c(persons, prs)
                    
    personal
}

mods_titleInfo <- function(field){
    stopifnot(xml_name(field) == "titleInfo")

    ## todo: for now just get the title
    xml_text(xml_find_all(field, ".//title"))
}


mods_originInfo <- function(field){
    stopifnot(xml_name(field) == "originInfo")

    res <- list()
    
    dateIssued <- xml_text(xml_find_first(field, ".//dateIssued"))
    ## TODO: this is lazy and incomplete
    if(grepl("^[0-9][0-9][0-9][0-9]$", dateIssued))
        res$year <- dateIssued
    else if(grepl("^[0-9][0-9][0-9][0-9]-[0-9]$", dateIssued)){
        res$year <- substr(dateIssued, 1, 4)
        res$month <- substr(dateIssued, 6, 6)
    }else if(grepl("^[0-9][0-9][0-9][0-9]-[0-9][0-9]$", dateIssued)){
        res$year <- substr(dateIssued, 1, 4)
        res$month <- substr(dateIssued, 6, 7)
    }else
        res$date <- dateIssued
    
    publisher <- xml_text(xml_find_first(field, ".//publisher"))
    place <- xml_text(xml_find_first(field, ".//place"))
    edition <- xml_text(xml_find_first(field, ".//edition"))

    res$publisher <- c(publisher = publisher, place = place)
    res$edition <- edition

    print(names(res))
    
          
    res
}


mods_typeOfResource <- function(field){
    stopifnot(xml_name(field) == "typeOfResource")

    xml_text(field)
}


mods_genre <- function(field){
    stopifnot(xml_name(field) == "genre")

    cat(xml_name(field), "\n")
    #browser()
}

mods_relatedItem <- function(field){
    stopifnot(xml_name(field) == "relatedItem")
    
    xml_text(xml_find_all(field, ".//titleInfo"))
}

mods_identifier <- function(field){
    stopifnot(xml_name(field) == "identifier")

    field.type <- xml_attr(field, "type")
    switch(field.type,
           citekey = ,
           doi = ,
           issn = ,
           isbn = {
               structure(xml_text(field), names = field.type)
           },
           {
               ## default
               message("unknown type '", field.type, "' of 'identifier'")
               character(0)
           }
           )
}

mods_part_detail <- function(piece){
    stopifnot(xml_name(piece) == "detail")

    piece.type <- xml_attr(piece, "type")
    switch(piece.type,
           volume = ,
           issue   = ,
           chapter = , # some of these may not be in use
           section = {
               structure(xml_integer(piece), names = piece.type)
           },
           {
               ## default
               message("unknown type '", piece.type, "' of 'detail'")
               integer(0)
           }
           )
}

mods_part_extent <- function(piece){
    stopifnot(xml_name(piece) == "extent")

    ## there is 'unit' attribute for this, assume pages for now
    st <- xml_text(xml_find_first(piece, ".//start"))
    en <- xml_text(xml_find_first(piece, ".//end"))
    paste0(st, "--", en)
}

mods_part_namePart <- function(piece){
    stopifnot(xml_name(piece) == "namePart")

    type <- xml_attr(piece, "type")
    if(type %in% c("given", "family")){
        personal <- .name_piece(personal, type, xml_text(piece))
    }else if(type == "email")
        personal <- .name_piece(personal, type, xml_text(piece))
    else{
        message("namePart", type, " not implemented yet for persons")
        NULL
    }
}




## process a single mods entry
process_mods <- function(modsbib){
    
    fields <- xml_find_all(modsbib, "./*")
    fields.names <- xml_name(fields)
    
    paths <- xml_path(xml_find_all(modsbib, "./*"))

    ## process fields
    persons <- person()
    res <- list()
    res[["author"]] <- persons
    for(i in seq_along(fields)){
        field <- fields[[i]]
        fieldname <- fields.names[i]
        switch(
            fieldname,
            "name" = {
                personal <- mods_name(field)
                prs <- do.call("person", personal)
                persons <- c(persons, prs)
            },
            "titleInfo" = {
                res[["title"]] <- mods_titleInfo(field)
            },
            "originInfo" = {
                wrk <- mods_originInfo(field)
                
                flags.pub <- is.na(wrk$publisher)
                if(!all(flags.pub))
                    res[["publisher"]] <- paste(wrk$publisher[!flags.pub], collapse = ", ")

                for(s in c("year", "month", "edition"))
                    if(length(wrk[[s]]) > 0  && !is.na(wrk[[s]]))
                        res[[s]] <- wrk[[s]]
            },
            "typeOfResource" = {
                res[["typeOfResource"]] <- mods_typeOfResource(field)
            },
            "genre" = {
                mods_genre(field)
            },
            "relatedItem" = {
                res[[fieldname]] <- mods_relatedItem(field)
            },
            "identifier" = {
                wrk <- mods_identifier(field)
                for(s in names(wrk))
                    res[[s]] <- wrk[[s]]
            },
            "part" = {
                ## for now assuming each attribute appears at most once
                pieces <- xml_children(field)
                ## note: xml_length() gives the lengths of the children
                for(j in seq_along(xml_length(pieces))){
                    piece <- pieces[[j]]
                    nam <- xml_name(piece)
                    switch(nam,
                           detail = {
                               wrk <- mods_part_detail(piece)
                               for(s in names(wrk))
                                   res[[s]] <- wrk[[s]]
                               
                           },
                           extent = {
                               res[["pages"]] <- mods_part_extent(piece)
                           },
                           namePart = {
                                   # type <- xml_attr(piece, "type")
                                   # if(type %in% c("given", "family")){
                                   #     personal <- .name_piece(personal, type, xml_text(piece))
                                   # }else if(type == "email")
                                   #     personal <- .name_piece(personal, type, xml_text(piece))
                                   # else
                                   #     message("namePart", type, " not implemented yet for persons")
                               mpnp <- mods_part_namePart(piece)
                               browser()
                               if(!is.null(mpnp))
                                   ## TODO: this is almost certainly wrong!
                                   personal <- mpnp
                           },
                           ## default
                           {
                               message("ignoring ", nam, " for now")
                           }
                           )
                }
                prs <- do.call("person", personal)
#browser()         
                persons <- c(persons, prs)
                    
            },
            ## default
            ##     cat("skipping ", fieldname, "\n")
                       # res[[fieldname]] <- xml_text(field)
            res[[fieldname]] <- .add_field(res, field, fieldname)
#browser()

        )
        
    }

    res[["author"]] <- persons
    
    res
}
