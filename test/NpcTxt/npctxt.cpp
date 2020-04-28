#include <catch.hpp>
#include "file_formats.h"


TEST_CASE("[NpcTxt] Load")
{
    PGESTRING rawData;
    NPCConfigFile npc;

    bool res = FileFormats::ReadNpcTXTFileF("npc-876.txt", npc);

    REQUIRE(res);
    REQUIRE(npc.ReadFileValid);
}
