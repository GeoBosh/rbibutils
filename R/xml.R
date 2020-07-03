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
#browser()    
    col
 }

## parse a mods collection to  bib-like structure
bibmods <- function(col){ # col - object returned by read_mods
    count_type <- c("no_type" = 0, "not_char" = 0) # for testing only
    
    mods <- xml_find_all(col, ".//mods")
    ids <- xml_attr(mods, "ID")
    ## xml_length(mods) # number of children of each element

    res <- vector(length(ids), mode = "list")
    names(res) <- ids
    for(i in seq_along(ids)){
        res[[i]] <- process_mods(mods[[i]])
        ## if(!is.null(res[[i]]$entry_type)  && res[[i]]$entry_type == "TechReport")
        ##     browser()

        et <- res[[i]]$entry_type
        if(!is.null(et)){
            if(is.character(et) && length(et) == 1)
                count_type[et] <- 1 + if(is.na(count_type[et]))
                                          0
                                      else count_type[et]
            else{
                count_type["not_char"] <- count_type["not_char"] + 1
            }
        }else{
            count_type["no_type"] <- count_type["no_type"] + 1
            #browser()
        }
        
    }
    
    cat("\ncount_type:\n\n")
    print(count_type)
    cat("\n\n")

#browser()    

    structure(res, class = "bibmods")
}

.toBibentry1 <- function(x){
    do.call(bibentry, x)
}

toBibentry <- function(object){
    set_bibtype <- function(k){
        object[[k]]$bibtype <<-
            if(is.null(object[[k]]$entry_type))
                "Misc"
            else object[[k]]$entry_type
    }
    set_key <- function(k){
        object[[k]]$key <<-
            if(is.null(object[[k]]$citekey)){
                message("TODO: generate an automatic key?")
                stop("citekey is absent")
            }else object[[k]]$citekey
    }

    set_journal <- function(k){ # temporary fudge
        if(object[[k]]$bibtype == "Article" && is.null(object[[k]]$journal)){
            object[[k]]$journal <<- object[[k]]$relatedItem
        }
    }
    
    
    lapply(seq_along(object), set_bibtype)
    lapply(seq_along(object), set_key)
    lapply(seq_along(object), set_journal) ## TODO: patch
    
    ## wrk <- lapply(object, .toBibentry1)
    wrk <- vector(length(object), mode = "list")
    names(wrk) <- names(object)
    for(i in seq_along(object)){
        item <- try(.toBibentry1(object[[i]]))
        if(inherits(item, "try-error")){
            wrk[[i]] <- NA
        }else
            wrk[[i]] <- item
    }
    flag.na <- sapply(wrk, function(x) is.logical(x) && is.na(x) )
#browser()    
    if(any(flag.na)){
        message("\n", length(flag.na[flag.na]), " out of ", length(flag.na), " items could not be converted:\n",
                paste(names(object)[flag.na], collapse = "\n"), "\n")
        wrk <- wrk[!flag.na]
    }

    do.call("c", wrk)
}


.name_piece <- function(cur, type, new){
    cur[[type]] <- if(is.null(cur[[type]]))
                       new
                   else{
                       c(cur[[type]], new)
                   }
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
                   if(is.na(type))
                       ## krapka, possibly name of organisation
                       personal <- .name_piece(personal, "family", xml_text(piece))
                   else if(type %in% c("given", "family")){
                       personal <- .name_piece(personal, type, xml_text(piece))
                   }else if(type == "email")
                       personal <- .name_piece(personal, type, xml_text(piece))
                   else
                       message("namePart", type, " not implemented yet for persons")
               },
               role = {
                   role <- tolower(xml_text(piece))

                   ## krapka
                   switch(role,
                          "degree grantor" = {role <- "degree granting institution"},
                          "organizer of meeting" = {role <- "organizer"}
                          )
                      
                   ind <- which(MARC_relator_abbr[ , 1] == role)
                   if(length(ind) > 0){ # == 1
                       Rrole <- MARC_relator_abbr[ind, 2]
                       personal <- .name_piece(personal, "role", Rrole)
                   }else{
                       message("unknown role '", role, "', ignoring it")
                   }
               },
               ## default
               stop("unknown element ", xml_name(piece), " in 'name'")
               )
    }
                    
    personal
}

mods_titleInfo <- function(field){
    stopifnot(xml_name(field) == "titleInfo")

    ## todo: for now just get the title
    title <- xml_text(xml_find_all(field, ".//title"))
    subtitle <- xml_text(xml_find_all(field, ".//subTitle"))
    if(length(subtitle) == 0)       
        title
    else
        paste0(title, ": ", subtitle)
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
        ##res$date <- dateIssued
        res$year <- dateIssued
    
    publisher <- xml_text(xml_find_first(field, ".//publisher"))
    place <- xml_text(xml_find_first(field, ".//place"))
    edition <- xml_text(xml_find_first(field, ".//edition"))

    res$publisher <- c(publisher = publisher, place = place)
    res$edition <- edition

    ## print(names(res))
    
          
    res
}


mods_typeOfResource <- function(field){
    stopifnot(xml_name(field) == "typeOfResource")

    xml_text(field)
}


## genre terms authority = marcgt, downloaded on 2020-06-24
## https://www.loc.gov/standards/valuelist/marcgt.html
genre_marcgt <- c(
    "abstract"			 , NA_character_,
    "summary"			 , NA_character_, # equiv. to abstract
    "art original"		 , NA_character_,
    "art reproduction"		 , NA_character_,
    "article"                    , "Article",
    "atlas"			 , NA_character_,
    "autobiography"		 , NA_character_,
    "bibliography"		 , NA_character_,
    "biography"			 , NA_character_,
    "book"                       , "Book",
    "calendar"			 , NA_character_,
    "catalog"			 , NA_character_,
    "chart"			 , NA_character_,
    "comedy"			 , NA_character_,
    "comic or graphic novel"	 , NA_character_,
    "conference publication"     , "Proceedings", # "InProceedings"?
    "database"			 , NA_character_,
    "dictionary"		 , NA_character_,
    "diorama"			 , NA_character_,
    "directory"			 , NA_character_,
    "discography"		 , NA_character_,
    "document (computer)"	 , NA_character_,
    "drama"			 , NA_character_,
    "encyclopedia"		 , NA_character_,
    "essay"			 , NA_character_,
    "festschrift"		 , NA_character_,
    "fiction"			 , NA_character_,
    "filmography"		 , NA_character_,
    "filmstrip"			 , NA_character_,
    "finding aid"		 , NA_character_,
    "flash card"		 , NA_character_,
    "folktale"			 , NA_character_,
    "font"			 , NA_character_,
    "game"			 , NA_character_,
    "government publication"	 , NA_character_,
    "graphic"			 , NA_character_,
    "globe"			 , NA_character_,
    "handbook"			 , NA_character_,
    "history"			 , NA_character_,
    "hymnal"			 , NA_character_,
    "humor"			 , NA_character_,
    "satire"			 , NA_character_, # equiv
    "index"			 , NA_character_,
    "instruction"                , "Manual",
    "interview"			 , NA_character_,
    "issue"			 , NA_character_,
    "journal"			 , NA_character_,
    "kit"			 , NA_character_,
    "language instruction"	 , NA_character_,
    "law report or digest"	 , NA_character_,
    "legal article"		 , NA_character_,
    "legal case and case notes"	 , NA_character_,
    "legislation"		 , NA_character_,
    "letter"			 , NA_character_,
    "loose-leaf"		 , NA_character_,
    "map"			 , NA_character_,
    "memoir"			 , NA_character_,
    "microscope slide"		 , NA_character_,
    "model"			 , NA_character_,
    "motion picture"		 , NA_character_,
    "multivolume monograph"	 , NA_character_,
    "newspaper"			 , "Article",
    "nonmusical sound"		 , NA_character_,
    "novel"			 , NA_character_,
    "numeric data"		 , NA_character_,
    "offprint"			 , NA_character_,
    "online system or service"	 , NA_character_,
    "patent"			 , NA_character_,
    "periodical"		 , "Article",  # todo: check if ok
    "picture"			 , NA_character_,
    "poetry"			 , NA_character_,
    "programmed text"		 , NA_character_,
    "realia"			 , NA_character_,
    "rehearsal"			 , NA_character_,
    "remote sensing image"	 , NA_character_,
    "reporting"			 , NA_character_,
    "review"			 , NA_character_,
    "script"			 , NA_character_,
    "series"			 , NA_character_,
    "short story"		 , NA_character_,
    "slide"			 , NA_character_,
    "sound"			 , NA_character_,
    "speech"			 , NA_character_,
    "standard or specification"	 , NA_character_,
    "statistics"		 , NA_character_,
    "survey of literature"	 , NA_character_,
    "technical drawing"		 , NA_character_,
    "technical report"		 , "TechReport",       
    "thesis"			 , "PhdThesis",
    "toy"			 , NA_character_,
    "transparency"		 , NA_character_,
    "treaty"			 , NA_character_,
    "videorecording"		 , NA_character_,
    "web site"			 , NA_character_,
    "yearbook"                   , NA_character_
)

## bu_auth.c, const char *bu_genre[] =
## bibutils genre that are not in marcgt
genre_bibutils <- c(
    "academic journal"     , "Article",
    "airtel"               , NA_character_,
    "collection"           , "Collection",  #"collection" = "InCollection", 
    "communication"        , NA_character_,
    "Diploma thesis"       , "DiplomaThesis",
    "Doctoral thesis"      , NA_character_,
    "electronic"           , "Electronic",
    "e-mail communication" , NA_character_,
    "Habilitation thesis"  , NA_character_,
    "handwritten note"     , NA_character_,
    "hearing"              , NA_character_,
    "journal article"      , "Article",
    "Licentiate thesis"    , "PhdThesis",
    "magazine"             , "Article", # todo: check if ok
    "magazine article"     , "Article",
    "manuscript"           , NA_character_,
    "Masters thesis"       , "MastersThesis",
    "memo"                 , NA_character_,
    "miscellaneous"        , "Misc",
    "newspaper article"    , NA_character_,
    "pamphlet"             , NA_character_,
    "Ph.D. thesis"         , "PhdThesis",
    "press release"        , NA_character_,
    "teletype"             , NA_character_,
    "television broadcast" , NA_character_,
    "unpublished"          , "Unpublished"
)

## defined in bibutils as output type (for bibtex?) but not in the above
genre_other <- c(
    "book chapter" ,            "InBook",
    "report" ,                  "TechReport"
)

genre_all_bibtex <- matrix(c(genre_marcgt, genre_bibutils, genre_other),
                           ncol = 2, byrow = TRUE)
rownames(genre_all_bibtex) <- tolower(genre_all_bibtex[ , 1])
colnames(genre_all_bibtex) <- c("Mods", "Bibtex")


mods_genre <- function(field){
    stopifnot(xml_name(field) == "genre")
 
    type <- xml_text(field)
    ## switch(type,
    ##        "journal article" =        "Article",      
    ## 
    ##        ## bibtexout.c/genre_matches
    ##        ##   simplified here
    ##        "periodical" =              "Article",      
    ##        "academic journal" =        "Article",      
    ##        "magazine" =                "Article",      
    ##        "newspaper" =               "Article",      
    ##        "article" =                 "Article",      
    ##        "instruction" =             "Manual",       
    ##        "book" =                    "Book",         
    ##        #"book" =                    "Inbook",       
    ##        "book chapter" =            "Inbook",       
    ##        "unpublished" =             "Unpublished",  
    ##        "manuscript" =              "Unpublished",  
    ##        "conference publication" =  "Proceedings",  
    ##        #"conference publication" =  "InProceedings",
    ##        "collection" =              "Collection",   
    ##        #"collection" =              "InCollection", 
    ##        "report" =                  "Report",       
    ##        "technical report" =        "Report",       
    ##        "Masters thesis" =          "MastersThesis",
    ##        "Diploma thesis" =          "DiplomaThesis",
    ##        "Ph.D. thesis" =            "PhdThesis",    
    ##        "Licentiate thesis" =       "PhdThesis",    
    ##        "thesis" =                  "PhdThesis",    
    ##        "electronic" =              "Electronic",   
    ##        "miscellaneous" =           "Misc",
    ##        ## default
    ##        {
    ##            ## TODO: change this to return "Article" in this case?
    ##            message("unknown type '", type, "' of 'genre', returning it as is'")
    ##            type
    ##        }
    ##        )

    ## TODO: this logic is for toBibtex or toBibentry, just return type here.
    if(grepl("research report" , tolower(xml_text(field))))
        ## krapka, TODO: add to genre_all_bibtex?
        return("Report")
    else{
##         if(tolower(bt_genre) %in% c("report", "Rapport de recherche")){
##             ## TODO: krapka
##             "TechReport"
##         }else if(tolower(bt_genre) == "m.sc. thesis"){
##             ## TODO: krapka
##             "MastersThesis"
##         }else if(!(tolower(type) %in% rownames(genre_all_bibtex))){
        
        #if(tolower(type) %in% c("report", "rapport de recherche")){
        #    ## TODO: krapka
        #    "TechReport"
        #}else
        if(tolower(type) == "m.sc. thesis"){
            ## TODO: krapka
            type <- "masters thesis"
        #}else if(tolower(type) == "book chapter"){
        #    ## TODO: krapka
        #    type <- "InBook"
        }else if(!(tolower(type) %in% rownames(genre_all_bibtex))){
            message("genre ", type,
                    " is currently unknown to bibConvert; using Misc instead")
            return("Misc")
        }
       
        bt_genre <- genre_all_bibtex[tolower(type), "Bibtex"]
        if(is.na(bt_genre)){
            warning("Can't convert genre '", type, "' to Bibtex; using 'Misc' instead")
            "Misc"
        #}else if(tolower(bt_genre) %in% c("report", "rapport de recherche")){
        #    ## TODO: krapka
        #    "TechReport"
        }else{
            bt_genre
        }
    }
}

mods_relatedItem <- function(field){
    stopifnot(xml_name(field) == "relatedItem")
    
    type <- xml_attr(field, "type")
    res <- list()
    if(!is.null(type) && type == "host"){
        subfields <- xml_children(field)
        nams <- xml_name(subfields)

        ## ##  TODO: more informed check here?
        ## if("name" %in% nams){
        ##     res[["booktitle"]] <- process_mods(field)
        ##     return(res) # NOTE: early return !!!
        ## }
        
        title <- xml_text(xml_find_all(field, ".//titleInfo"))

        xml_genre <- xml_find_all(field, "./genre")
        genre <- xml_text(xml_genre)
        names(genre) <- xml_attr(xml_genre, "authority")
        res$genre <- genre["bibutilsgt"]
        if(is.na(res))
            res$genre <- genre["marcgt"]
        switch(res$genre,
               "academic journal" = ,
               "periodical" = {
                   res$journal = title
               },
               "collection" = {
                   res$booktitle = title
                   #browser()
                   res$entry_type <- "InCollection"
                   if("name" %in% nams){
                       ## no, this doesn't work out of the box:
                       ## res[["booktitle"]] <- process_mods(field)
                       wrk <- process_mods(field)
                       for(nam in names(wrk)){
                           switch(nam,
                               title = {
                                   res[["booktitle"]] = wrk[["title"]]
                               },
                               persons = {
                                   res[["editor"]] = wrk[["persons"]]
                               },
                               entry_type = {
                                   "" # do nothing
                               },
                               ##default
                               res[[nam]] <- wrk[[nam]]
                           )

                       }
                   }
                   publ <- xml_find_first(field, ".//publisher")
                   if(!is.na(publ))
                       res$publisher <- xml_text(publ)
                   
               },
               "conference publication" = {
                   res$booktitle = title
                   res$entry_type = "InProceedings"
                   if("name" %in% nams){
                       ## res[["booktitle"]] <- process_mods(field)
                       wrk <- process_mods(field)
                       for(nam in names(wrk)){
                           switch(nam,
                               title = {
                                   res[["booktitle"]] = wrk[["title"]]
                               },
                               persons = {
                                   res[["editor"]] = wrk[["persons"]]
                               },
                               entry_type = {
                                   "" # do nothing
                               },
                               ##default
                               res[[nam]] <- wrk[[nam]]
                               )

                       }
                   }
                   publ <- xml_find_first(field, ".//publisher")
                   #if(!is.na(publ))
                   #    res$publisher <- xml_text(publ)
                   
               },
               {
                   ## default, krapka
                   res$booktitle <- title
               }
               )
        #print(field)
        #browser()
        res
    }else{
         ## krapka, true processing returns a list (here it is 'character')
         res <- xml_text(xml_find_all(field, ".//titleInfo"))
    }
    res
}

mods_location <- function(field){
    kids <- xml_children(field)
    nams <- xml_name(kids)
    txt <- xml_text(kids)
    res <- sapply(seq_along(nams),
                  function(i) switch(nams[i],
                                     "url" = paste0("\\url{", txt[i], "}"),
                                     ## default
                                     txt[i] )
                  )
    paste(res, collapse = " ")
}

mods_identifier <- function(field){
    stopifnot(xml_name(field) == "identifier")

    ## see modstypes.c
    field.type <- xml_attr(field, "type")
    switch(field.type,
           eprint = ,
           arXiv = ,
           
           citekey = ,      
           issn = ,         
           isbn = ,         
           doi = ,          
           url = ,          
           uri = ,          
           pubmed = ,       
           medline = ,      
           pmc = ,          
           pii = ,          
           isi = ,          
           lccn = ,         
           accessnum = 
               {
                   structure(xml_text(field), names = field.type)
               },
           "serial number" = 
               {
                   ## as above but remove the space from the name
                   structure(xml_text(field), names = "serialnumber")
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
           number = ,
           "report number" = ,
           issue   = ,
           chapter = , # some of these may not be in use
           section = {
               ## making this text since the things are not always integer:
               ##     structure(xml_integer(piece), names = piece.type)
               #browser()
               structure(xml_text(piece), names = piece.type)
           },
           page = {
               ## it is not neessarilly integer, may be more like id
               ## with leading zeroes and maybe letters
               structure(xml_text(piece), names = piece.type)
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


## fn_biblatex <- system.file("inst", "bib", "ex0.biblatex",  package = "rbibutils")
## 
## bib <- tempfile(fileext = ".bib")
## bibConvert(infile = fn_biblatex, outfile = bib, informat = "biblatex", outformat = "bib")
## 
## modl <- tempfile(fileext = ".xml")
## bibConvert(infile = fn_biblatex, outfile = modl, informat = "biblatex", outformat = "xml")
## 
## modl.obj <- read_mods(modl)
## modl.obj # {xml_document} <modsCollection> ...
## 
## y <- bibmods(modl.obj)
## toBibentry(y)
##
## process a single mods entry
process_mods <- function(modsbib){
    fields <- xml_find_all(modsbib, "./*")
    fields.names <- xml_name(fields)
    
    paths <- xml_path(xml_find_all(modsbib, "./*"))

    ## process fields
    persons <- person()
    part_persons <- person()
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

                ## krapka
                ro <- unclass(prs[[1]])[[1]]$role
                if(!is.null(ro) && any(c("dgg", "spn") %in% ro)){
                    res[["institution"]] <- prs
                    res[["school"]] <- prs
                    ## print(modsbib)
                }else{
                    persons <- c(persons, prs)
                }
            },
            "titleInfo" = {
                res[["title"]] <- mods_titleInfo(field)
            },
            "originInfo" = {
                wrk <- mods_originInfo(field)
                
                flags.pub <- is.na(wrk$publisher)
                if(!all(flags.pub)){
                    res[["publisher"]] <- paste(wrk$publisher[!flags.pub], collapse = ", ")
                    ## krapka for TechReport; TODO: sort out
                    res[["institution"]] <- res[["publisher"]]
                }

                for(s in c("year", "month", "edition"))
                    if(length(wrk[[s]]) > 0  && !is.na(wrk[[s]]))
                        res[[s]] <- wrk[[s]]
            },
            "typeOfResource" = {
                res[["typeOfResource"]] <- mods_typeOfResource(field)
            },
            "genre" = {
                ## note: entry types for bibtex and bibentry are the same except
                ##       for "booklet" which is only in bibtex
                res[["entry_type"]] <- mods_genre(field)            
                # print(res[["entry_type"]])
            },
            "relatedItem" = {
                wrk <- mods_relatedItem(field)
                if(is.character(wrk))
                    res[[fieldname]] <- wrk
                else ## must be 'list'
                    res[names(wrk)] <- wrk
                    
            },
            "identifier" = {
                wrk <- mods_identifier(field)
                for(s in names(wrk))
                    res[[s]] <- wrk[[s]]
            },
            "location" = {
                res[["howpublished"]] <- mods_location(field)
            },
            "part" = {
                part_personal <- person()
                ## for now assuming each attribute appears at most once
                pieces <- xml_children(field)
                ## note: xml_length() gives the lengths of the children
                for(j in seq_along(xml_length(pieces))){
                    piece <- pieces[[j]]
                    nam <- xml_name(piece)
                    switch(nam,
                           date = {
                               ## TODO: this needs more care
                               res[["year"]] <- xml_text(piece)
                           },

                           detail = {
                               wrk <- mods_part_detail(piece)
                               for(s in names(wrk))
                                   res[[s]] <- wrk[[s]]
                               
                           },
                           extent = {
                               res[["pages"]] <- mods_part_extent(piece)
                           },
                           namePart = {
                               mpnp <- mods_part_namePart(piece)
                               if(!is.null(mpnp))
                                   
                                   part_personal <- mpnp
                           },
                           ## default
                           {
                               message("ignoring ", nam, " for now")
#browser()                               
                           }
                           )
                }
                if(length(part_personal) > 0){
                    prs <- do.call("person", part_personal)
#browser()         
                    part_persons <- c(part_persons, prs)
                }
            },
            ## default
            ##     cat("skipping ", fieldname, "\n")
                       # res[[fieldname]] <- xml_text(field)
            res[[fieldname]] <- .add_field(res, field, fieldname)
#browser()

        )
        
    }

    #cat("count_type:\n")
    #print(count_type)

    res[["author"]] <- persons
    ## TODO: part_persons?
    
    res
}


