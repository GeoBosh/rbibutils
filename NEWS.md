# rbibutils 2.1.1

- now a warning is issued if package 'testthat' is not available for tests.


# rbibutils 2.1 (CRAN)

- `readBib` can now convert bibtex files directly (i.e., without first
  converting to XML intermediate) to bibentry R objects. This can be requested
  by setting the new argument `direct` to `TRUE`.

- `readBib` gets argument `texChars` to control whether or not to convert TeX
  sequences representing characters (such as accented Latin characters) to
  normal characters in the output encoding. There is an option also to convert
  charaters to the corresponding TeX sequences.

- `readBib` now accepts encodings as for `bibConvert`.

- `readBib` now processes field `key` in bibtex files. (This field is optional,
  used by some bibtex styles for sorting.)

- bug fixes and improvements.

- removed field `LazyData` from DESCRIPTION as there is no data directory
  (and R-devel now flags it with a NOTE).
  

# rbibutils 2.0 (CRAN)

- there is no longer (unintended) dependence on R >= 3.4. This was because of
  the use of `R_unif_index`. Report and fix due to Henrik Sloot (#1).

- completely reimplemented the conversion to `bibentry` - now this is done
  entirely in `C` and it now has the same speed as the conversions to other
  bibliograthy formats.

- removed `xml2` from the imports - it is no longer needed now that the
  conversion to bibentry is done in `C`.

- new functions `readBibentry` and `writeBibentry` for reading from and writing
  bibentries to R source files.

- now errors when reading bibentry files are turned into warnings with suitable
  messages.


# rbibutils 1.4 (CRAN)

- new function `writeBib` for writing bibtex files.

- fixed erroneous processing of PhD thesis bib entries with some values of field
  `type`.  (reported by Kisung You for GeoBosh/Rdpack#17)

- fixed a compiler warning about a pointer differing in
  signedness from the expected type (reported by Patrice Kiener).

- somehow `README.md` went missing in v1.3, now reinstated.


# rbibutils 1.3 (CRAN)

- reverted a change in v1.2 which caused trouble with some latex characters.

- trimmed white space in cite keys and some others to avoid getting cite keys
  containing the newline character (possible if the comma after the key is on a
  new line).

- new function `readBib` for importing bibtex files.


# rbibutils 1.2.1

- improved processing of URL field when converting to bibentry.


# rbibutils 1.2

- fixed inBook processing.

- fixed encoding bug introduced in v1.1.

- fixed misterous loss of `$`s and curly braces in certain circumstances.


# rbibutils 1.1.0 (CRAN)

- fixed processing of multiple person names in bibtex import.

- mathematical formulas were wrongly exported without dollars in some cases.

- stopped printing some messages causing problems to Rdpack.


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
