.onLoad <- function(libname, pkgname){
    ## (2023-11-11) TODO: not completely sure tht JSSextra needs to be automatically made
    ##                    default. Probably yes.
    ##
    ## register for now to keep with the old behaviour, i.e the one before fixing JSSextra to
    ## not make modifications in the environment of JSS, making JSS and JSSextra the same
    ## style.
    ##
    ## 2025-10-31
    ## was: register_JSSextra(TRUE)
    ##
    ## TODO: Don't export these functions, make afront end for them with using fixed argument reset = TRUE
    ##       to avoid (to some extent) external callers accidentally messing up these bibstyles.
    bibstyle_JSSextra(make_default = TRUE)

    bibstyle_JSSextraLongNames() ## maybe don't initialise it automatically?

    NULL
}
