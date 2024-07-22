PGE File Formats Changelog
==========================

* Made a set of changes to improve library-wide validation.
  * Out-of-range integer fields now cause file parse to fail. (Previously, such fields were silently clipped.)
  * The behavior of the string split function was made consistent between the stdc++ and Qt builds.
    * The bounds of numeric values are now checked more strictly in the Qt build, consistent with the stdc++ build.

* Made a set of changes to standardize the PGE-X file format:
  * Sections:
    * Ignore section names that are all spaces. Fail to parse on section names including nul.
    * Remove old, unused subsection syntax.

      This means that the file

          A
          B
          A_END

      which was previously invalid is now legal, with a text section with name `A` and contents `B`.

      It also means that the file

          A
          B
          B_END
          FIELD:VALUE;
          A_END

      now treats "A" as a text section with contents `B\nB_END\nFIELD_VALUE;`, instead of as a data section with a subsection `B` and a single item.
  * Data lines:
    * Data lines must end with a semicolon.

      This invalidates lines `F1:V1;F2:V2?` (previously parsed as `F1:V1;F2:V2;`) and `F1:V1;F2` (previously parsed as `F1:V1;`).
    * If a data line embeds a nul byte, the file is malformed and behavior is undefined. Currently, the remainder of the line is silently ignored, but there must still be a semicolon immediately before the line ending. This behavior may arbitrarily change in the future.
    * Markers (tags) in data lines may not include the characters `\`, `:`, or `;`. (As before, values in data lines must escape the characters `:` and `;` with a backslash.)

      This invalidates lines `F1\::V1;`, `F1\\:V1;` and `F1\;:V1;`, but does not invalidate lines `F1:V1\:;`, `F1:V1\\;` or `F1:V1\;;`.
  * Markers (tags) in data lines may be repeated. If they are repeated, only the data from the last marker will be kept. However, all values, even the ones that will be overwritten, must be validly formatted.
    * Previously, invalid substructure arrays were silently ignored.
    * This invalidates (for example), `ET:"Event Name";SSS:["F1;V1;"];SSS:["F1:V1;"];` (previously parsed as `ET:"Event Name";SSS:["F1:V1;"];`).
  * Strings:
    * A string may not comprise of a single `"` character. This invalidates line `STR:";` (previously parsed as `STR:"";`).
    * If a string includes a `\` followed by a non-escape character, the `\` is now silently ignored. (Previously, the `\` was included in the unescaped string.)

      This means that `STR:"\a";` (previously parsed as `STR:"\\a";`) is now parsed as `STR:"a";`.
  * Arrays:
    * An array may include no entries. This makes `ARR:[];` valid.

      (Such arrays match the default array, so they will not be saved by any future tooling.)
    * An array may not reopened after it has been closed. This invalidates `ARR:["A"]["B"];` (previously parsed as `ARR:["A","B"];`).
    * An array must be closed. This invalidates `ARR:["A";` and `ARR:["A",;` (previously parsed as `ARR:["A"];`).
    * Array items do not need to escape the characters `[`, `]`, or `,`.

      This makes `ARR["[,]"];` valid, and it parses identically to `ARR["\[\,\]"];`.

      (For compatibility, the save routines will continue to escape these characters in strings indefinitely.)
    * Escape characters are passed to the array item's parser. This fixes the parse of `LS:["a\nd"];` (previously parsed as `LS:["and"];`).
  * Numeric types:
    * An unsigned int must not be empty. This invalidates `UINT:;` (previously parsed as `UINT:0;`).
    * Floating point values must not use `,` as the decimal separator. This invalidates `DBL:1,2;` (previously parsed as `DBL:1.2;`).
    * Floating point values must not use `E` as the exponent separator. This invalidates `DBL:1E2;` (previously parsed as `DBL:1e2;`).
    * Floating point values must not use the sign character `+`. This invalidates `DBL:+1;` (previously parsed as `DBL:1;`).
    * The floating point exponent may use the sign character `+`. `DBL:1e+1` and `DBL:1e1` have the same meaning.
    * Floating point values may end with `.`, but must have at least 1 digit. The exponent must have at least 1 and at most 4 digits.

      This invalidates `DBL:1e12345;`, and makes `DBL:1.;` valid.
* LVLX:
  * `SECTION`:
    * `SC`: section index must be >= 0 and <= 1000. Any other value will cause a parse failure. This invalidates `SC:-1;` (previously crashed parser) and `SC:1000000;` (previously cause hang and allocation failure).
  * `EVENTS_CLASSIC`:
    * `SSS` (section settings):
      * `ID`: section index must be >= 0 and <= 1000. Any other value will cause a parse failure. This invalidates `ID:-1;` (previously crashed parser) and `ID:1000000;` (previously cause hang and allocation failure).
      * `AST`: autoscroll type must not exceed the maximum 32-bit signed integer, `2 147 483 647`. This invalidates `AST:3000000000;` (previously silently parsed in an undefined way).
      * `SHX`: load section height expression from field with marker `SHX` (was previously incorrectly saved to `SHX` but loaded from `SWH`).
    * `ML` (move layers):
      * `SXX`: load x speed expression from field with marker `SXX` (was previously incorrectly saved to `SXX` but loaded from `AXX`).
      * `SYX`: load y speed expression from field with marker `SYX` (was previously incorrectly saved to `SYX` but loaded from `AYX`).
  * `CUSTOM_ITEMS_38A`:
    * Field `T` (type) is required and must be 0 (block), 1 (bgo), or 2 (effect). This invalidates `T:3;ID:0;D:["1=2"];` and `ID:0;D:["1=2"];` (previously interpreted in an unspecified way).
    * `D`: data keys must not exceed the maximum 32-bit signed integer, `2 147 483 647`. This invalidates `T:0;ID:0;D:["3000000000=0"];` (previously silently parsed in an undefined way).
    * Each item in `D` must be exactly of the form `INT=LONG`. This invalidates `D:["1=2=..."];` (previously interpreted as `D:["1=2"];`).

      (Note also that the stdc++ build previously interpreted `==1==2==` as `1=2`, ignoring any empty segments of the string when split by `=`. This error did not affect the Qt build used in the Moondust Editor.)
* WLDX:
  * `META_BOOKMARKS`: fixed the types of fields `X` and `Y` (`float`, was `int`).

    This makes `BM:"Name";X:128.0;Y:128.0;` valid.
  * `AREARECTS`: fixed the type of field `TP` (`unsigned int`, was `int`).

    This invalidates `TP:-1;` (which previously set the touch policy to an indeterminate value).
