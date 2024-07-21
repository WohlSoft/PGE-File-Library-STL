PGE File Formats Changelog
==========================

* Made a set of changes to standardize the PGE-X file format:
  * Out-of-range integer fields now cause file parse to fail. (Previously, such fields were silently clipped.)
  * The behavior of the string split function was made consistent between the stdc++ and Qt builds.
  * Ignore section names that are all spaces. Fail to parse on section names including nul.
  * Remove old, unused subsection syntax.

    This means that the file

        A
        B
        A_END

    which was previously invalid is now legal, with a text section with name "A" and contents "B".

    It also means that the file

        A
        B
        B_END
        FIELD:VALUE;
        A_END

    now treats "A" as a text section with contents `B\nB_END\nFIELD_VALUE;`, instead of as a data section with a subsection "B" and a single item.
