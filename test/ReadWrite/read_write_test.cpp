#include <catch_amalgamated.hpp>
#include "file_formats.h"

#ifndef TEST_WORKDIR
#   define TEST_WORKDIR "."
#endif

#ifndef TEST_WRITEDIR
#   define TEST_WRITEDIR "."
#endif

template<typename T>
PGESTRING fromNum(T num)
{
    std::ostringstream n;
    n << num;
    return n.str();
}


TEST_CASE("[38A 1.4.0 Level] Load")
{
    LevelData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-140.lvl";

    bool res = FileFormats::OpenLevelFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.blocks.size() == 1);
    REQUIRE(data.bgo.size() == 1);
    REQUIRE(data.npc.size() == 1);
    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.physez.size() == 1);

    REQUIRE(data.layers.size() == 4);
    REQUIRE(data.events.size() == 4);
}

TEST_CASE("[38A 1.4.0 World] Load")
{
    WorldData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-140.wld";

    bool res = FileFormats::OpenWorldFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.tiles.size() == 1);
    REQUIRE(data.scenery.size() == 1);
    REQUIRE(data.paths.size() == 1);
    REQUIRE(data.levels.size() == 1);
    REQUIRE(data.music.size() == 1);

    REQUIRE(data.layers.empty());
    REQUIRE(data.events38A.empty());
}

TEST_CASE("[38A 1.4.1 Level] Load")
{
    LevelData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-141.lvl";

    bool res = FileFormats::OpenLevelFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.blocks.size() == 1);
    REQUIRE(data.bgo.size() == 1);
    REQUIRE(data.npc.size() == 1);
    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.physez.size() == 1);

    REQUIRE(data.layers.size() == 4);
    REQUIRE(data.events.size() == 4);
}

TEST_CASE("[38A 1.4.1 World] Load")
{
    WorldData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-141.wld";

    bool res = FileFormats::OpenWorldFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.tiles.size() == 1);
    REQUIRE(data.scenery.size() == 1);
    REQUIRE(data.paths.size() == 1);
    REQUIRE(data.levels.size() == 1);
    REQUIRE(data.music.size() == 1);

    REQUIRE(data.layers.empty());
    REQUIRE(data.events38A.empty());
}


TEST_CASE("[38A 1.4.2 Level] Load")
{
    LevelData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-142.lvl";

    bool res = FileFormats::OpenLevelFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.blocks.size() == 1);
    REQUIRE(data.bgo.size() == 1);
    REQUIRE(data.npc.size() == 1);
    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.physez.size() == 1);

    REQUIRE(data.layers.size() == 4);
    REQUIRE(data.events.size() == 4);
}

TEST_CASE("[38A 1.4.2 World] Load")
{
    WorldData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-142.wld";

    bool res = FileFormats::OpenWorldFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.tiles.size() == 1);
    REQUIRE(data.scenery.size() == 1);
    REQUIRE(data.paths.size() == 1);
    REQUIRE(data.levels.size() == 1);
    REQUIRE(data.music.size() == 1);

    REQUIRE(data.layers.size() == 1);
    REQUIRE(data.events38A.size() == 0);
}


TEST_CASE("[38A 1.4.3 Level] Load")
{
    LevelData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-143.lvl";

    bool res = FileFormats::OpenLevelFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.blocks.size() == 1);
    REQUIRE(data.bgo.size() == 1);
    REQUIRE(data.npc.size() == 1);
    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.physez.size() == 1);

    REQUIRE(data.layers.size() == 4);
    REQUIRE(data.events.size() == 4);
}

TEST_CASE("[38A 1.4.3 World] Load")
{
    WorldData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-143.wld";

    bool res = FileFormats::OpenWorldFile(path, data);

    INFO("\nFile: " + path + "\n"
         "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.tiles.size() == 1);
    REQUIRE(data.scenery.size() == 1);
    REQUIRE(data.paths.size() == 1);
    REQUIRE(data.levels.size() == 1);
    REQUIRE(data.music.size() == 1);

    REQUIRE(data.layers.size() == 1);
    REQUIRE(data.events38A.size() == 0);
}


TEST_CASE("[38A 1.4.4 Level] Load")
{
    LevelData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-144.lvl";

    bool res = FileFormats::OpenLevelFile(path, data);

    INFO("File: " + path + "; Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.blocks.size() == 1);
    REQUIRE(data.bgo.size() == 1);
    REQUIRE(data.npc.size() == 1);
    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.physez.size() == 1);

    REQUIRE(data.layers.size() == 4);
    REQUIRE(data.events.size() == 4);
}

TEST_CASE("[38A 1.4.4 World] Load")
{
    WorldData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-144.wld";

    bool res = FileFormats::OpenWorldFile(path, data);

    INFO("\nFile: " + path + "\n"
                             "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.tiles.size() == 1);
    REQUIRE(data.scenery.size() == 1);
    REQUIRE(data.paths.size() == 1);
    REQUIRE(data.levels.size() == 1);
    REQUIRE(data.music.size() == 1);

    REQUIRE(data.layers.size() == 1);
    REQUIRE(data.events38A.size() == 0);
}


TEST_CASE("[38A 1.4.5 Level] Load")
{
    LevelData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-145.lvl";

    bool res = FileFormats::OpenLevelFile(path, data);

    INFO("File: " + path + "; Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.blocks.size() == 1);
    REQUIRE(data.bgo.size() == 1);
    REQUIRE(data.npc.size() == 1);
    REQUIRE(data.doors.size() == 1);
    REQUIRE(data.physez.size() == 1);

    REQUIRE(data.layers.size() == 4);
    REQUIRE(data.events.size() == 4);
}

TEST_CASE("[38A 1.4.5 World] Load")
{
    WorldData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/test-145.wld";

    bool res = FileFormats::OpenWorldFile(path, data);

    INFO("\nFile: " + path + "\n"
                             "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.tiles.size() == 1);
    REQUIRE(data.scenery.size() == 1);
    REQUIRE(data.paths.size() == 1);
    REQUIRE(data.levels.size() == 1);
    REQUIRE(data.music.size() == 1);

    REQUIRE(data.layers.size() == 1);
    REQUIRE(data.events38A.size() == 0);
}

TEST_CASE("[38A 1.4.5 World] Load - Events Test")
{
    WorldData data;
    PGESTRING path = TEST_WORKDIR "/test-files/SMBX-38A/wld-events-test.wld";

    bool res = FileFormats::OpenWorldFile(path, data);

    INFO("\nFile: " + path + "\n"
                             "Line=" + fromNum(data.meta.ERROR_linenum) + "\n" +
         "Error: " + data.meta.ERROR_info);
    REQUIRE(res);
    REQUIRE(data.meta.ReadFileValid);

    REQUIRE(data.tiles.size() == 0);
    REQUIRE(data.scenery.size() == 0);
    REQUIRE(data.paths.size() == 0);
    REQUIRE(data.levels.size() == 2);
    REQUIRE(data.music.size() == 0);

    REQUIRE(data.layers.size() == 4);
    REQUIRE(data.events38A.size() == 1);
}
