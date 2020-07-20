# rbibutils 1.0.3

- in `adsout.c`, fixed several (what looked like) typo's in `else if`
  clauses. One was always evaluating to `false`, the rest always to `true`. This
  was raising warnings on CRAN's tests with `clang` on Linux.

- fix typo's in the documentation.

- update website.


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



     adsout.c:433:24: warning: overlapping comparisons always evaluate to false [-Wtautological-overlap-compare]
     adsout.c:426:24: warning: overlapping comparisons always evaluate to true [-Wtautological-overlap-compare]
     adsout.c:413:24: warning: overlapping comparisons always evaluate to true [-Wtautological-overlap-compare]
     adsout.c:400:24: warning: overlapping comparisons always evaluate to true [-Wtautological-overlap-compare]
