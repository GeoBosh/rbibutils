# rbibutils 1.0.3 (CRAN)

- fixed warnings from `clang` compilers on CRAN. (These were about tautology
  `if` clauses in `src/adsout.c` and default argument promotion of the second
  argument in a couple of invocations of `va_start` in `src/modsout.c`).

- fixed typo's in the documentation.

- updated the website.


# rbibutils 1.0.2 (CRAN)

- completed the copyright credits in DESCRIPTION (there were contributors not
  mentioned anywhere except in the C source files they contributed).


# rbibutils 1.0.1 

- clarified the copyright holders in DESCRIPTION.


# rbibutils 1.0.0 

Features of this version:

- includes an R port of `bibutils` libraries (currently `bibutils_6.10`).

- supports all character encodings available in `bibutils` (defaults are UTF-8).

- supports all input/output bibliography formats available in `bibutils`,
  including Bibtex, Biblatex, and XML mods intermediate.

- in addition, supports conversions of the above formats from/to `bibentry` R
  source files or `rds` objects.
