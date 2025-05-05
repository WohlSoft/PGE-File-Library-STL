/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*! \file mdx_savx_file.cpp
 *
 *  \brief Implements defines MDX structures for the save objects and file
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "save_filedata.h"
#include "pge_file_lib_private.h"

#include "mdx/common/mdx_file.h"
#include "mdx/common/mdx_macros.h"

#include "mdx/mdx_gamesave_file.h"

MDX_SETUP_OBJECT(GamesaveHead,
    MDX_FIELD("LV", lives);
    MDX_FIELD("HN", hundreds);
    MDX_FIELD("CN", coins);
    MDX_FIELD("PT", points);
    MDX_FIELD("TS", totalStars);
    MDX_FIELD("WX", worldPosX);
    MDX_FIELD("WY", worldPosY);
    MDX_FIELD("HW", last_hub_warp);
    MDX_FIELD("HL", last_hub_level_file);
    MDX_FIELD("MI", musicID);
    MDX_FIELD("MF", musicFile);
    MDX_FIELD("GC", gameCompleted);
    MDX_FIELD("SLV", savedLayersVisible);
    MDX_FIELD("SLH", savedLayersHidden);
);

MDX_SETUP_OBJECT(saveCharState,
    MDX_FIELD_NO_SKIP("ID", id);
    MDX_FIELD("ST", state);
    MDX_FIELD("IT", itemID);
    MDX_FIELD("MT", mountType);
    MDX_FIELD("MI", mountID);
    MDX_FIELD("HL", health);
);

MDX_SETUP_OBJECT(savePlayerState,
    MDX_FIELD_NONNEG_NO_SKIP("ID", characterID);
);

MDX_SETUP_OBJECT(visibleItem,
    MDX_FIELD_NO_SKIP("ID", first);
    MDX_FIELD("V", second);
);

MDX_SETUP_OBJECT(starOnLevel,
    MDX_FIELD_NO_SKIP("L", first);
    MDX_FIELD("S", second);
);

MDX_SETUP_OBJECT(saveLevelInfo,
    MDX_FIELD_NO_SKIP("L", level_filename);
    MDX_FIELD("S", max_stars);
    MDX_FIELD("M", max_medals);
    MDX_FIELD("MG", medals_got);
    MDX_FIELD("MB", medals_best);
    MDX_FIELD("E", exits_got);
);

static const char* MDX_DataSection_load_location(saveUserData::DataSection& s, const char* field_data)
{
    return MDX_load_value(s.location, field_data);
}

static bool MDX_DataSection_save_location(std::string& out, const saveUserData::DataSection& s)
{
    if((s.location & saveUserData::DATA_VOLATILE_FLAG) != 0)
        return false;

    int location_clean = (s.location & saveUserData::DATA_LOCATION_MASK);

    return MDX_save_value(out, location_clean);
}

#include "pge_x.h"

template<>
const char* MDX_Value<saveUserData::DataEntry>::load(saveUserData::DataEntry& e, const char* field_data)
{
    PGESTRING got;
    const char* ret = MDX_load_value(got, field_data);

    PGESTRINGList dp;
    PGE_SPLITSTRING(dp, got, "=");
    if(dp.size() < 2)
        throw MDX_missing_delimiter('=');
    e.key = PGE_ReplSTRING(PGEFile::X2STRING(dp[0]), "\\q", "=");
    e.value = PGE_ReplSTRING(PGEFile::X2STRING(dp[1]), "\\q", "=");

    return ret;
}

template<>
bool MDX_Value<saveUserData::DataEntry>::save(std::string& out, const saveUserData::DataEntry& src)
{
    std::string sub;

    PGESTRING temp = src.key;
    PGE_ReplSTRING_inline(temp, "=", "\\q");

    MDX_save_value(sub, temp);

    sub += '=';

    temp = src.value;
    PGE_ReplSTRING_inline(temp, "=", "\\q");

    MDX_save_value(sub, temp);

    return MDX_save_value(out, sub);
}

using DataSection = saveUserData::DataSection;

MDX_SETUP_OBJECT(DataSection,
    MDX_UNIQUE_FIELD("L", MDX_DataSection_load_location, MDX_DataSection_save_location);
    MDX_FIELD_NOT_ONLY("SN", name);
    MDX_FIELD_NOT_ONLY("LN", location_name);
    MDX_FIELD_NOT_ONLY("D", data);
);

struct MDX_GamesaveFile : MDX_File<GamesaveLoadCallbacks, GamesaveSaveCallbacks>
{
    MDX_SECTION_SINGLE("SAVE_HEADER", GamesaveHead, head);

    MDX_SECTION("CHARACTERS", saveCharState, charstate);

    MDX_SECTION("CHARACTERS_PER_PLAYERS", savePlayerState, selchar);

    MDX_SECTION("VIZ_LEVELS", visibleItem, vis_level);

    MDX_SECTION("VIZ_PATHS", visibleItem, vis_path);

    MDX_SECTION("VIZ_SCENERY", visibleItem, vis_scene);

    MDX_SECTION("STARS", starOnLevel, star);

    MDX_SECTION("LEVEL_INFO", saveLevelInfo, level_info);

    MDX_SECTION("USERDATA", DataSection, userdata);

};

bool MDX_load_gamesave(PGE_FileFormats_misc::TextInput& input, const GamesaveLoadCallbacks& callbacks)
{
    std::unique_ptr<MDX_GamesaveFile> f(new MDX_GamesaveFile());
    return f->load_file(input, callbacks);
}

bool MDX_save_gamesave(PGE_FileFormats_misc::TextOutput& output, const GamesaveSaveCallbacks& callbacks)
{
    std::unique_ptr<MDX_GamesaveFile> f(new MDX_GamesaveFile());
    return f->save_file(output, callbacks);
}
