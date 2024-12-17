#include <catch_amalgamated.hpp>
#include "file_formats.h"

#ifndef TEST_WORKDIR
#   define TEST_WORKDIR "."
#endif


TEST_CASE("[LevelFile] Load")
{
    PGESTRING rawData;
    LevelData lvl;

    bool res = FileFormats::OpenLevelFile(TEST_WORKDIR "/sample.lvl", lvl);

    REQUIRE(res);
    REQUIRE(lvl.meta.ReadFileValid);
}
