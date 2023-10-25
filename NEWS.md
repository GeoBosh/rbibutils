# rbibutils 2.2.16 (CRAN)

- fixed processing of the Polish suppressed-l `\l`. Previously the character
  following it was enclosed in braces, which was harmless in most cases but the
  superfluous braces could cause trouble in special circumstances. Did the same
  for the uppercase version, `\L`, which was missing from the code.


# rbibutils 2.2.15 (CRAN)

- adapted some tests to a change in R-devel circa r84986.


# rbibutils 2.2.14 (CRAN)

- fixed a couple of tests in 'test-convert.R' which started failing (ca. R svn
  rev 84760) due to an R-devel change in `person()`.


# rbibutils 2.2.13 (CRAN)

- Bugfix: the declaration of the return value of a C function was accidentally
  changed from `const char *` to `char *` in v2.2.12, causing CRAN warnings on
  some platforms.


# rbibutils 2.2.12 (CRAN)

- Bugfix: `readBib` with `texChars = "Rdpack"` which converts `\'i` to `\'\i`
   was wrapping `\'\i` in braces in 'author' and related fields only. Failure to
   do so in other fields was making the output invalid when `\'i` was followed
   by alphabetical character(s). Fixes issue#7 reopened by @EricMarcon.

   Note that option `"Rdpack"` for argument `texChars` is mainly for internal
   use which compensates for R not recognising `\'i` as an accented character,
   see the news items for v2.2.5 and v2.2.4 and the bug report about
   `tools:::latexToUtf8` at [R bugs](https://bugs.r-project.org/show_bug.cgi?id=18208).

- partial internal refactoring of C code to avoid duplication.


# rbibutils 2.2.11 (CRAN)

- in `C` code, changed `sprintf` calls to use `snprintf` to fix CRAN warnings.


# rbibutils 2.2.10 (CRAN)

- fixed warnings about deprecated function declarations without prototypes in
  `C` code.


# rbibutils 2.2.9 (CRAN)

- fixed wrong markup of an item in this file (for v2.2.4) causing bad rendering
  and a NOTE from CRAN.
  

# rbibutils 2.2.8 (CRAN)

- new argument `fbibentry` for `readBib` and `readBibentry` for specification of
  a function for generation of bib objects. It should have the same arguments as
  the default `utils::bibentry()` but doesn't need to be vectorised, since
  `readBibentry()` generates one `bibentry` call for each reference.

- `readBib` now parses correctly LaTeX accents like `\a'o`. These are officially
  defined only for LaTeX's `tabbing` environment as replacements for `\'o`,
  etc., but seem accepted by LaTeX outside these environments. Also, `Bibtex`
  converts them to the standard ones when writing `.bbl` files and R's bib
  processing functions know about them.

- a bug in `ads` export was causing a test to fail on platforms with `char`
  equivalent to `unsigned char`. Reported by @nileshpatra, who also identified
  the cause. Fixes issue#8 on github.


# rbibutils 2.2.7 (CRAN)

- fixed bug in isi output occuring in v2.2.5 and 2.2.6 and revealed by gss/ASAN
  check.


# rbibutils 2.2.6

- fixed memory leaks in nbib output in biblatex input occuring in v2.2.5.


# rbibutils 2.2.5 (CRAN)

- `readBibentry` with `extra = TRUE` now parses (almost) any syntactically
  correct bib entries. In particular, it accepts arbitrary bib types and doesn't
  throw errors for bibentries missing fields required by bibtex for the standard
  bibtex types. For example, biblatex entries typically have `date`, not `year`.

- `readBib` with `direct = TRUE` and `texChars` set to `"convert"` or `"export"`
  was not processing mathematical expressions properly. Now fixed.
  
- argument `texChars` of `readBib` gets new possible value, "Rdpack". This is
  like the default, "keep", but in addition it converts `\'i` to `\'\i`. This
  is related to issue #7, see below the fix in v2.2.4 for details. This is
  mainly for internal use.

- improved the messages from error handling when creating bibentry
  objects. The fix is in `readBibentry` but users typically see them when
  calling `readBib`.

- fixed an error which caused `bibConvert` to segfault when importing `nbib`
  files.

- `readBibentry` (and hence `readBib`) were printing some error messages that
  were actually handled.
  

# rbibutils 2.2.4 (CRAN)

- fixed the handling of accents over `i` in authors' names when the LaTeX
  escapes are not converted to real characters (there were no problems when
  converted to UTF-8). Reported by Manuel López-Ibáñez with examples (issue #5,
  #6, #7).

  - `\i` in authors' names was messing up the following character (issue#5).

  - stopped converting `\'\i'` to `\'i`. They are equivalent but there is no
    reason to do this. Also, at the time of writing R's `tools::latexToUtf8`
    converts the former but not the latter, while Biber seems to convert the
    latter but not the former, see
    https://bugs.r-project.org/show_bug.cgi?id=18208 . So, users of the bib file
    may have specific reasons to use one or the other.

  - names consisting of just one part, family, were missing processing of escaped
    characters (issue #5).
    

# rbibutils 2.2.3 (CRAN)

- fixed memory issues from valgrind in v2.2.2 thanks to patch supplied by Bill
  Dunlap.


# rbibutils 2.2.2 (CRAN)

- import of `Pubmed XML` was sometimes giving a handful of references for files
  with tens or hundreds of them. Now fixed, see issue #4, reported by Rafael
  Santamaría.  (The function reading the file was implicitly assuming that the
  end of each reference is on a line by itself and was silently ignoring text on
  the same line after the end of reference tag. Reference files from online
  databases often have no new lines at all, except for the XML header.)


# rbibutils 2.2.1 (CRAN)

- fixed problems revealed by valgrind.


# rbibutils 2.2 (CRAN)

- new convenience function `charToBib` takes input from a character vector
  rather than a file. By default it assumes that the input is in `bibtex` format
  and dispatches to `readBib`. If an input format is specified it calls
  `bibConvert`.

- new S3 class `bibentryExtra`, inheriting from `bibentry`, provides support for
  non-standard types (potentially arbitrary) of bibtex entries. Suitable methods
  are defined for printing, converting and manipulating (e.g., subsetting and
  assignment) `bibentryExtra` objects. The initial implementation is incomplete
  and under development.

- `readBib()` gets a new argument `extra`. If it is set to `TRUE` and
  non-standard types of entries are encountered in the input bibtex file, then
  the class of the result is set to `bibentryExtra`. See also the note about
  `bibentryExtra`.

- `readBib` gets new argument, `macros`, for specification of file(s) containing
  Bibtex macros (such as abbreviations for names of journals). These files are
  read in before file(s) specified by `file`.

- `readBib` with `direct = TRUE` now accepts non-syntactic field names,
  e.g. containing `-`. (This is irrelevant when `direct = FALSE` since in that
  case nonstandard fields are ignored and standard fields do not contain unusual
  characters.)

- internal structures for bibtex macros, such as `@string` were not cleared at
  the end of calls and were accumulating from multiple invocations of
  conversions from bibtex and biblatex. Now fixed.

- now references with missing cite keys in bibtex input are accepted (previously
  such references were dropped). Dummy cite keys are inserted.

- now `@preamble` entry in bibtex is ignored silently. Previously it was also
  dropped but with a message about unrecognised type, which was confusing.

- export arXiv:XXX and similar as `https` (some were still exported as `http`).
  Fixes GeoBosh/Rdpack#21, reported by Kisung You.

- unsuported conversions for some formats were accepted with unpredictable
  results. Informative messages are printed now.

- improved handling of byte order marks (BOM) for utf8 output. This was causing
  problems to tests of this package on Windows since on Linux BOMs are not
  emitted (I haven't figured what causes the difference - the code is not OS
  dependent).

- improved handling of corner cases in bibtex input, especially for `readBib`
  with `direct = TRUE`.


# rbibutils 2.1.1 (CRAN)

- now a warning (rather than error) is issued if package 'testthat' is not
  available for tests.

- `readBib` now has a default for the encoding, so it would usually be called
   just with one argument (the filename).


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
