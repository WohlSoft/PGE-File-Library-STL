#include <catch_amalgamated.hpp>
#include "file_formats.h"

#ifndef TEST_WORKDIR
#   define TEST_WORKDIR "."
#endif

TEST_CASE("[38A Warp: None] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile(TEST_WORKDIR "/warp0.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_NONE);
}

TEST_CASE("[38A Warp: Scroll] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile(TEST_WORKDIR "/warp1.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_SCROLL);
}


TEST_CASE("[38A Warp: Fade] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile(TEST_WORKDIR "/warp2.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_FADE);
}

TEST_CASE("[38A Warp: Flip Horizontal] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile(TEST_WORKDIR "/warp3.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_FLIP_H);
}

TEST_CASE("[38A Warp: Flip Vertical] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile(TEST_WORKDIR "/warp4.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_FLIP_V);
}
