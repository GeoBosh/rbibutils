.onLoad <- function(libname, pkgname){
    ## (2023-11-11) TODO: not completely sure tht JSSextra needs to be automatically made
    ##                    default. Probably yes.
    ##
    ## register for now to keep with the old behaviour, i.e the one before fixing JSSextra to
    ## not make modifications in the environment of JSS, making JSS and JSSextra the same
    ## style.
    register_JSSextra(TRUE)

    NULL
}

