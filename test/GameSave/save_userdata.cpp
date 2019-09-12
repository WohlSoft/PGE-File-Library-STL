#include <catch.hpp>
#include "file_formats.h"


TEST_CASE("[UserData] Save and Load data")
{
    PGESTRING rawData;
    GamesaveData origin = FileFormats::CreateGameSaveData();
    GamesaveData target = FileFormats::CreateGameSaveData();

    {
        saveUserData::DataSection ds;
        ds.location = saveUserData::DATA_WORLD;
        ds.name = "default";
        ds.data.push_back({"kek", "12345"});
        ds.data.push_back({"i am", "goblin!"});
        ds.data.push_back({"дерево", "с яблоками!"});
        origin.userData.store.push_back(ds);
    }
    {
        saveUserData::DataSection ds;
        ds.location = saveUserData::DATA_LEVEL;
        ds.location_name = "Random universe.lvlx";
        ds.name = "default";
        ds.data.push_back({"мышь", "42"});
        ds.data.push_back({"кошь", "рыба"});
        origin.userData.store.push_back(ds);
    }
    {
        saveUserData::DataSection ds;
        ds.location = saveUserData::DATA_LEVEL;
        ds.location_name = "Random universe.lvlx";
        ds.name = "Math";
        ds.data.push_back({"x=", "12+3/y == kek"});
        ds.data.push_back({"4hv24H^24V^H24vh246vh2$^VH$^=90=74=-23065=40b =2-457=-305", "G$2g52g52*#(46&*@$%&@#%&^!@#%&&^()*(+(_+()-=0=-0+_)+_0=0-"});
        origin.userData.store.push_back(ds);
    }

    REQUIRE(FileFormats::WriteExtendedSaveFileRaw(origin, rawData));
    REQUIRE(FileFormats::WriteExtendedSaveFileF("sampleSave.savx", origin));

    REQUIRE(FileFormats::ReadExtendedSaveFileRaw(rawData, "fakePath.savx", target));

    REQUIRE(origin.userData.store.size() == target.userData.store.size());

#define COMPARE_FIELD(field) origin.field  == target.field
    REQUIRE(COMPARE_FIELD(userData.store[0].name));
    REQUIRE(COMPARE_FIELD(userData.store[0].location));
    REQUIRE(COMPARE_FIELD(userData.store[0].location_name));
    REQUIRE(COMPARE_FIELD(userData.store[0].data.size()));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[0].key));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[0].value));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[1].key));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[1].value));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[2].key));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[2].value));
    REQUIRE(COMPARE_FIELD(userData.store[1].name));
    REQUIRE(COMPARE_FIELD(userData.store[1].location));
    REQUIRE(COMPARE_FIELD(userData.store[1].location_name));
    REQUIRE(COMPARE_FIELD(userData.store[1].data.size()));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[0].key));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[0].value));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[1].key));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[1].value));
    REQUIRE(COMPARE_FIELD(userData.store[2].name));
    REQUIRE(COMPARE_FIELD(userData.store[2].location));
    REQUIRE(COMPARE_FIELD(userData.store[2].location_name));
    REQUIRE(COMPARE_FIELD(userData.store[2].data.size()));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[0].key));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[0].value));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[1].key));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[1].value));
#undef COMPARE_FIELD
}



TEST_CASE("[UserData] Save and Load data with volatile entry")
{
    PGESTRING rawData;
    GamesaveData origin = FileFormats::CreateGameSaveData();
    GamesaveData target = FileFormats::CreateGameSaveData();

    {
        saveUserData::DataSection ds;
        ds.location = saveUserData::DATA_WORLD;
        ds.name = "default";
        ds.data.push_back({"kek", "12345"});
        ds.data.push_back({"i am", "goblin!"});
        ds.data.push_back({"дерево", "с яблоками!"});
        origin.userData.store.push_back(ds);
    }
    {
        saveUserData::DataSection ds;
        ds.location = saveUserData::DATA_LEVEL;
        ds.location_name = "Random universe.lvlx";
        ds.name = "default";
        ds.data.push_back({"мышь", "42"});
        ds.data.push_back({"кошь", "рыба"});
        origin.userData.store.push_back(ds);
    }
    {
        saveUserData::DataSection ds;
        ds.location = saveUserData::DATA_LEVEL;
        ds.location_name = "Random universe.lvlx";
        ds.name = "Math";
        ds.data.push_back({"x=", "12+3/y == kek"});
        ds.data.push_back({"4hv24H^24V^H24vh246vh2$^VH$^=90=74=-23065=40b =2-457=-305", "G$2g52g52*#(46&*@$%&@#%&^!@#%&&^()*(+(_+()-=0=-0+_)+_0=0-"});
        origin.userData.store.push_back(ds);
    }
    {
        saveUserData::DataSection ds;
        ds.location = saveUserData::DATA_LEVEL | saveUserData::DATA_VOLATILE_FLAG;
        ds.location_name = "Random universe.lvlx";
        ds.name = "Just a garbage";
        ds.data.push_back({"4h6642h26h26h26h", "epoitjhmpietmhpoet"});
        origin.userData.store.push_back(ds);
    }

    REQUIRE(FileFormats::WriteExtendedSaveFileRaw(origin, rawData));
    REQUIRE(FileFormats::WriteExtendedSaveFileF("sampleSave.savx", origin));

    REQUIRE(FileFormats::ReadExtendedSaveFileRaw(rawData, "fakePath.savx", target));

    REQUIRE(origin.userData.store.size() == target.userData.store.size() + 1);

#define COMPARE_FIELD(field) origin.field  == target.field
    REQUIRE(COMPARE_FIELD(userData.store[0].name));
    REQUIRE(COMPARE_FIELD(userData.store[0].location));
    REQUIRE(COMPARE_FIELD(userData.store[0].location_name));
    REQUIRE(COMPARE_FIELD(userData.store[0].data.size()));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[0].key));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[0].value));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[1].key));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[1].value));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[2].key));
    REQUIRE(COMPARE_FIELD(userData.store[0].data[2].value));
    REQUIRE(COMPARE_FIELD(userData.store[1].name));
    REQUIRE(COMPARE_FIELD(userData.store[1].location));
    REQUIRE(COMPARE_FIELD(userData.store[1].location_name));
    REQUIRE(COMPARE_FIELD(userData.store[1].data.size()));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[0].key));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[0].value));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[1].key));
    REQUIRE(COMPARE_FIELD(userData.store[1].data[1].value));
    REQUIRE(COMPARE_FIELD(userData.store[2].name));
    REQUIRE(COMPARE_FIELD(userData.store[2].location));
    REQUIRE(COMPARE_FIELD(userData.store[2].location_name));
    REQUIRE(COMPARE_FIELD(userData.store[2].data.size()));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[0].key));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[0].value));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[1].key));
    REQUIRE(COMPARE_FIELD(userData.store[2].data[1].value));
#undef COMPARE_FIELD
}
