/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

/*! \file mdx_world_file.cpp
 *
 *  \brief Implements defines MDX structures for the world objects and file
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "wld_filedata.h"
#include "pge_file_lib_private.h"

#include "mdx/common/mdx_file.h"
#include "mdx/common/mdx_macros.h"
#include "mdx/mdx_meta_objects.hpp"

#include "mdx/mdx_world_file.h"

MDX_SETUP_OBJECT(WorldHead,
    MDX_FIELD("TL", EpisodeTitle);        //Episode Title
    MDX_FIELD("DC", nocharacter);     //Disabled characters
    MDX_FIELD("IT", IntroLevel_file);     //Intro level
    MDX_FIELD("GO", GameOverLevel_file);  //Game Over level
    MDX_FIELD("HB", HubStyledWorld);     //Hub Styled
    MDX_FIELD("RL", restartlevel);       //Restart level on fail
    MDX_FIELD("SZ", stars);              //Starz number
    MDX_FIELD("CD", authors);     //Credits list
    MDX_FIELD("CM", authors_music);     //Credits scene background music
    MDX_FIELD("SSS", starsShowPolicy); //Per-level stars count showing policy
    MDX_FIELD("XTRA", custom_params);     //World-wide Extra settings
    MDX_FIELD("CPID", configPackId);//Config pack ID string
);

MDX_SETUP_OBJECT(WorldTerrainTile,
    MDX_FIELD_NO_SKIP("ID", id); //Tile ID
    MDX_FIELD("X", x); //X Position
    MDX_FIELD("Y", y); //Y Position
    MDX_FIELD_XTRA();//Custom JSON data tree
);

MDX_SETUP_OBJECT(WorldScenery,
    MDX_FIELD_NO_SKIP("ID", id);  //Scenery ID
    MDX_FIELD("X", x); //X Position
    MDX_FIELD("Y", y); //Y Position
    MDX_FIELD_XTRA();//Custom JSON data tree
);

MDX_SETUP_OBJECT(WorldPathTile,
    MDX_FIELD_NO_SKIP("ID", id);  //Path ID
    MDX_FIELD("X", x); //X Position
    MDX_FIELD("Y", y); //Y Position
    MDX_FIELD_XTRA();//Custom JSON data tree
);

MDX_SETUP_OBJECT(WorldMusicBox,
    MDX_FIELD_NO_SKIP("ID", id); //MISICBOX ID
    MDX_FIELD("X", x); //X Position
    MDX_FIELD("Y", y); //X Position
    MDX_FIELD("MF", music_file);  //Custom music file
    MDX_FIELD_XTRA();//Custom JSON data tree
);

MDX_SETUP_OBJECT(WorldAreaRect,
    MDX_FIELD_NO_SKIP("F", flags);  //Flags
    MDX_FIELD("X", x); //X Position
    MDX_FIELD("Y", y); //X Position
    MDX_FIELD_NONNEG("W", w); //Width
    MDX_FIELD_NONNEG("H", h); //Height

    // unused stuff
    MDX_FIELD("MI", music_id); //MUSICBOX ID
    MDX_FIELD("MF", music_file);  //Custom music file
    MDX_FIELD("LR", layer);
    MDX_FIELD("EB", eventBreak);
    MDX_FIELD("EW", eventWarp);
    MDX_FIELD("EA", eventAnchor);
    MDX_FIELD("ET", eventTouch);
    MDX_FIELD("TP", eventTouchPolicy);
    MDX_FIELD_XTRA();//Custom JSON data tree
);

MDX_SETUP_OBJECT(WorldLevelTile,
    MDX_FIELD_NO_SKIP("ID", id); //LEVEL IMAGE ID
    MDX_FIELD("X", x); //X Position
    MDX_FIELD("Y", y); //X Position
    MDX_FIELD("LF", lvlfile);  //Target level file
    MDX_FIELD("LT", title);   //Level title
    MDX_FIELD("EI", entertowarp); //Entrance Warp ID (if 0 - start level from default points)
    MDX_FIELD("ET", top_exit); //Open top path on exit type
    MDX_FIELD("EL", left_exit); //Open left path on exit type
    MDX_FIELD("ER", right_exit); //Open right path on exit type
    MDX_FIELD("EB", bottom_exit); //Open bottom path on exit type
    MDX_FIELD("WX", gotox); //Goto world map X
    MDX_FIELD("WY", gotoy); //Goto world map Y
    MDX_FIELD("AV", alwaysVisible); //Always visible
    MDX_FIELD("SP", gamestart); //Is Game start point
    MDX_FIELD("BP", pathbg); //Path background
    MDX_FIELD("BG", bigpathbg); //Big path background
    MDX_FIELD("SSS", starsShowPolicy); // Stars count showing policy
    MDX_FIELD_XTRA();//Custom JSON data tree
);

struct MDX_WorldFile : MDX_File<WorldLoadCallbacks, WorldSaveCallbacks>
{
    MDX_SECTION_SINGLE("HEAD", WorldHead, head);

    MDX_SECTION_SINGLE("META_SYS_CRASH", CrashData, crash_data);

    MDX_SECTION("META_BOOKMARKS", Bookmark, bookmark);

    MDX_SECTION("TILES", WorldTerrainTile, tile);

    MDX_SECTION("SCENERY", WorldScenery, scene);

    MDX_SECTION("PATHS", WorldPathTile, path);

    MDX_SECTION("MUSICBOXES", WorldMusicBox, music);

    MDX_SECTION("AREARECTS", WorldAreaRect, arearect);

    MDX_SECTION("LEVELS", WorldLevelTile, level);
};

bool MDX_load_world(PGE_FileFormats_misc::TextInput& input, const WorldLoadCallbacks& callbacks)
{
    MDX_WorldFile f;
    return f.load_file(input, callbacks);
}

bool MDX_save_world(PGE_FileFormats_misc::TextOutput& output, const WorldSaveCallbacks& callbacks)
{
    MDX_WorldFile f;
    return f.save_file(output, callbacks);
}
