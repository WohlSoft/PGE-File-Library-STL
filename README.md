# PGE File Library v 0.3.1.14
----
This library is a part of Moondust (PGE Project).
----
Supported file formats:

```
PGE-X Family:
*.lvlx  PGE-X Level File               -  Read/Write
*.wldx  PGE-X World File               -  Read/Write
*.savx  PGE-X Game save File           -  Read/Write
*.meta  PGE-X non-SMBX64 Meta File     -  Read/Write
SMBX-64 Family:
*.lvl   SMBX 1...64 Level File         -  Read/Write
*.wld   SMBX 1...64 World File         -  Read/Write
*.sav   SMBX 1...64 Game save File     -  Read only
*.dat   SMBX 1...64 Game config File   -  Read/Write
*.txt   SMBX64 NPC Custom text config  -  Read/Write
SMBX-38A Family:
*.lvl   SMBX-38A Level File         -  Read/Write
*.wld   SMBX-38A World File         -  Read/(planned)Write
*.wls   SMBX-38A World settings     -  (planned)Read/Write
*.sav   SMBX-38A Game save File     -  (planned)Read/Write
```

----
Use library with this header:

```cpp
#include "file_formats.h"
```

----

The Library parses and generates files or RAW text strings.
You can read data from the memory as well as from the file.
You can use the openLevelFile() or openWorldFile() functions to open
necessary files more convenient.

# Notes for files of SMBX-64 format:
1) If you saving a file from raw data yourself, you should save a text file with
using of CRLF for SMBX-* formats, or file will be not readable by SMBX Engine.
You can write a file like binary, but when you detecting '\n' byte, write a
CRLF ("\r\n") bytes instead!

2) When you saving a level file (World file does not require that) into SMBX64 format,
you must prepare your data structures until you will save it:
```C++
//To initialize order priorities fields and mark all star NPCs
FileFormats::smbx64LevelPrepare(YourLevelData);
```

3) If you trying to use PGE File data in the LunaLUA, before filling internal arrays, need to apply the next set of the functions:

```C++
//To initialize order priorities fields and mark all star NPCs
FileFormats::smbx64LevelPrepare(YourLevelData);
//Order blocks
FileFormats::smbx64LevelSortBlocks(YourLevelData);
//Order BGO's
FileFormats::smbx64LevelSortBGOs(YourLevelData);
```

