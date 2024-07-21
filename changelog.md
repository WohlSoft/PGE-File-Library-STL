PGE File Formats Changelog
==========================

* Made a set of changes to standardize the PGE-X file format:
  * Out-of-range integer fields now cause file parse to fail. (Previously, such fields were silently clipped.)
  * The behavior of the string split function was made consistent between the stdc++ and Qt builds.
