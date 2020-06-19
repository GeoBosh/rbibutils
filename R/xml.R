
## x is modscollection
modsbib2be <- function(x){
    
    stopifnot(length(xml_name(x)) == 1, xml_name(x) == "modsCollection")

    ## xml_name(xml_find_all(x, "/*")) ==  "modsCollection" # TRUE

    ## TODO: for some reason can't get named XPth's to work properly
    
    bib_entries <- xml_name(xml_find_all(x, "/*/*")) # "mods" "mods" "mods" ...
    xml_name(xml_find_all(x, "/*/*")) # "mods" "mods" "mods" ...

    paths <- xml_path(xml_find_all(x, "/*/*"))

    res <- vector( length(paths), mode = "list")  # for now list
    for(i in seq_along(paths)){
        path <- paths[i]
        bib <- xml_find_all(x, path) ## should be of length 1, a single "mods" nodeset
        be <- process_mods(bib)
        res[[i]] <- be
    }
    
    res
}

.name_piece <- function(cur, type, new){
    cur[[type]] <- if(is.null(cur[[type]]))
                       new
                   else
                       c(cur[[type]], new)
    cur
}


read_mods <- function(...){
    col <- read_xml(...)
    if(xml_name(xml_root(col)) != "modsCollection")
        stop("the xml file is not modsCollection")
    xml_ns_strip(col)

    mods <- xml_find_all(col, ".//mods")
    ids <- xml_attr(mods, "ID")
    xml_length(mods) # number of children of each element

    res <- vector(length(ids), mode = "list")
    names(res) <- ids
    for(i in seq_along(ids)){
        wrk <- process_mods(mods[[i]])
        res[[i]] <- wrk
    }
    
# browser()
    
    
    res
}

.add_field <- function(res, field, fieldname){
    if(is.null(res[[fieldname]]))
        xml_text(field)
    else
        c(res[[fieldname]], xml_text(field))
}

process_mods <- function(modsbib){
    
    fields <- xml_find_all(modsbib, "./*")
    fields.names <- xml_name(fields)
    
    paths <- xml_path(xml_find_all(modsbib, "./*"))

    persons <- person()
    res <- list()
    res[["author"]] <- persons
    ## process fields
    res <- list()
    for(i in seq_along(fields)){
        field <- fields[[i]]
        fieldname <- fields.names[i]
        switch(
            fieldname,
            "name" =
                {
                    ## person(given = NULL, family = NULL, middle = NULL,
                    ##        email = NULL, role = NULL, comment = NULL,
                    ##        first = NULL, last = NULL)
                    personal <- list()
                    pieces <- xml_children(field)
                    ## note: xml_length() gives the lengths of the children
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
                    persons <- c(persons, prs)
                    
                },
               "titleInfo" = {
                   res[["title"]] <- xml_text(xml_find_all(field, ".//title"))
                   #browser()
               },
            "originInfo" = {
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

                if(!is.na(publisher)){
                    res[["publisher"]] <- publisher
                    if(!is.na(place))
                        res[["publisher"]] <- paste0(res[["publisher"]], ", ", place)
                }else if(!is.na(place))
                    res[["publisher"]] <- place
                
                if(!is.na(edition))
                   res[["edition"]] <- edition
                
                #browser()
               },
               "typeOfResource" = {
                   res[["typeOfResource"]] <- xml_text(field)
               },
               #"genre" = {
               #    cat(fieldname, "\n")
               #    browser()
               #},
               "relatedItem" = {
                   res[[fieldname]] <- xml_text(xml_find_all(field, ".//titleInfo"))

               },
               "identifier" = {
                   field.type <- xml_attr(field, "type")
                   switch(field.type,
                          citekey = ,
                          doi = ,
                          issn = ,
                          isbn = {
                              res[[field.type]] <- xml_text(field)
                          },
                          ## default
                          message("unknown type '", field.type, "' of 'identifier'")
                          )
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
                                   piece.type <- xml_attr(piece, "type")
                                   switch(piece.type,
                                          volume = ,
                                          issue   = ,
                                          chapter = , # some of these may not be in use
                                          section = {
                                              res[[piece.type]] <- xml_integer(piece)
                                          },
                                          ## default
                                          message("unknown type '", piece.type, "' of 'detail'")
                                          )
                               },
                               extent = {# there is 'unit' attribute for this, assume pages for now
                                   st <- xml_text(xml_find_first(piece, ".//start"))
                                   en <- xml_text(xml_find_first(piece, ".//end"))
                                   res[["pages"]] <- paste0(st, "--", en)
                               },
                               namePart = {
                                   type <- xml_attr(piece, "type")
                                   if(type %in% c("given", "family")){
                                       personal <- .name_piece(personal, type, xml_text(piece))
                                   }else if(type == "email")
                                       personal <- .name_piece(personal, type, xml_text(piece))
                                   else
                                       message("namePart", type, " not implemented yet for persons")
                               },
                               date = {
                                   message("ignoring 'date' for now")
                               },
                               text = {
                                   message("ignoring 'text' for now")
                               },
                               ## default
                               ## stop("unknown element ", xml_name(piece), " in 'name'")
                               message("ignoring ", nam, " for now")
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
