#include <catch.hpp>
#include "file_formats.h"


TEST_CASE("[LevelFile] Load")
{
    PGESTRING rawData;
    LevelData lvl;

    bool res = FileFormats::OpenLevelFile("sample.lvl", lvl);

    REQUIRE(res);
    REQUIRE(lvl.meta.ReadFileValid);
}
