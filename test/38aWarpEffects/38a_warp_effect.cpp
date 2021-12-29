#include <catch.hpp>
#include "file_formats.h"


TEST_CASE("[38A Warp: None] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile("warp0.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_NONE);
}

TEST_CASE("[38A Warp: Scroll] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile("warp1.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_SCROLL);
}


TEST_CASE("[38A Warp: Fade] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile("warp2.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_FADE);
}

TEST_CASE("[38A Warp: Flip Horizontal] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile("warp3.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_FLIP_H);
}

TEST_CASE("[38A Warp: Flip Vertical] Load")
{
    LevelData data;

    bool res = FileFormats::OpenLevelFile("warp4.lvl", data);

    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.doors[0].transition_effect == LevelDoor::TRANSIT_FLIP_V);
}
