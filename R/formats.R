rbibutils_formats <- matrix(c(
    ## "bibentry", "Yes", "Yes", 
    "ads",      "ads",      "No",  "Yes", "ADS reference format",
    "bib",      "bib",      "Yes", "Yes", "BibTeX",
    "bibtex",   "bibtex",   "Yes", "Yes", "BibTeX",
    "biblatex", "biblatex", "Yes", "Yes", "BibLaTeX",
    "copac",    "copac",    "Yes", "No",  "COPAC format references",
    "ebi",      "ebi",      "Yes", "No",  "EBI XML",
    "end",      "end",      "Yes", "Yes", "EndNote (Refer format)", 
    "endx",     "endx",     "Yes", "No",  "EndNote XML",
    "isi",      "isi",      "Yes", "Yes", "ISI web of science",  
    "med",      "med",      "Yes", "No",  "Pubmed XML references",  
    "nbib",     "nbib",     "Yes", "Yes", "Pubmed/National Library of Medicine nbib format",  
    "ris",      "ris",      "Yes", "Yes", "RIS format",
    "R, r, Rstyle", "R",    "Yes", "Yes", "R source file containing bibentry commands",
    "rds",      "rds",      "Yes", "Yes", "bibentry object in a binary file created by saveRDS()",
    "xml",      "xml",      "Yes", "Yes", "MODS XML intermediate",
    "wordbib",  "wordbib",  "Yes", "Yes", "Word 2007 bibliography format"
), ncol = 5, byrow = TRUE)

rbibutils_formats <- data.frame(
    Abbreviation = rbibutils_formats[ , 1],
    FileExt      = rbibutils_formats[ , 2],
    Input        = rbibutils_formats[ , 3] == "Yes",
    Output       = rbibutils_formats[ , 4] == "Yes",
    Description  = rbibutils_formats[ , 5]
)
