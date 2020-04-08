#include <catch.hpp>
#include "file_formats.h"


TEST_CASE("[LevelFile] Load")
{
    PGESTRING rawData;
    LevelData lvl;

    bool res = FileFormats::OpenLevelFile("/home/wohlstand/_git_repos/PGE-Project/_common/PGE_File_Formats/test/LevelLoad/sample.lvl", lvl);

    REQUIRE(res);
    REQUIRE(lvl.meta.ReadFileValid);
}
