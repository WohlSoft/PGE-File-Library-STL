#include <catch_amalgamated.hpp>
#include "file_formats.h"

#ifndef TEST_WORKDIR
#   define TEST_WORKDIR "."
#endif

TEST_CASE("[NpcTxt] Load")
{
    PGESTRING rawData;
    NPCConfigFile npc;

    bool res = FileFormats::ReadNpcTXTFileF(TEST_WORKDIR "/npc-876.txt", npc);

    REQUIRE(res);
    REQUIRE(npc.ReadFileValid);
}
