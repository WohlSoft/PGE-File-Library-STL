#include <catch_amalgamated.hpp>
#include "file_formats.h"

#ifndef TEST_WORKDIR
#   define TEST_WORKDIR "."
#endif

#ifndef TEST_WRITEDIR
#   define TEST_WRITEDIR "."
#endif


TEST_CASE("[38A 1.4.0 Level] Load")
{
    LevelData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-140.lvl";

    bool res = FileFormats::OpenLevelFile(path, data);

    INFO("File: " + path + "; Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_NONE);
}
