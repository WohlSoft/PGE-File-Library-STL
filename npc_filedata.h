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

/*!
 * \file npc_filedata.h
 * \brief Contains defition of the SMBX64-NPC.txt config data structure
 */

#pragma once
#ifndef NPC_FILEDATA_H
#define NPC_FILEDATA_H

#include "pge_file_lib_globs.h"

/*!
 * \brief SMBX64-NPC.txt File Data structure
 */
struct NPCConfigFile
{
    bool        ReadFileValid = false;
    PGESTRING   errorString;
    PGESTRING   unknownLines;

    typedef PGEMAP<PGESTRING, PGESTRING> EntriesMap;

    //! Universal store of all NPCTxt values
    EntriesMap entries;

/*
 * SMBX64 Standard
 */
    //! Is GFX-Offset X field using?
    bool en_gfxoffsetx = false;
    //! GFX-Offset X field
    int32_t gfxoffsetx = 0;

    //! Is GFX-Offset Y field using?
    bool en_gfxoffsety = false;
    //! GFX-Offset Y field
    int32_t gfxoffsety = 0;

    //! Is Width field using?
    bool en_width = false;
    //! Width field
    uint32_t width = 0;

    //! Is height field using?
    bool en_height = false;
    //! Height field
    uint32_t height = 0;

    //! Is GFX-Width field using?
    bool en_gfxwidth = false;
    //! GFX-Width field
    uint32_t gfxwidth = 0;

    //! Is GFX-Height field using?
    bool en_gfxheight = false;
    //! GFX-Height field
    uint32_t gfxheight = 0;

    //! Is Score field using?
    bool en_score = false;
    //! Score rewards field (from 0 to 13)
    uint32_t score = 0;

    //! Is "Block player at side" flag using?
    bool en_playerblock = false;
    //! "Block player at side" flag
    bool playerblock = false;

    //! Is "Player can stay on top" flag using?
    bool en_playerblocktop = false;
    //! "Player can stay on top" flag
    bool playerblocktop = false;

    //! Is "Block other NPC at side" flag using?
    bool en_npcblock = false;
    //! "Block other NPC at side" flag
    bool npcblock = false;

    //! Is "NPC Can stay on top" flag using?
    bool en_npcblocktop = false;
    //! "NPC Can stay on top" flag
    bool npcblocktop = false;

    //! Is "Allow to grab at side" flag using?
    bool en_grabside = false;
    //! "Allow to grab at side" flag
    bool grabside = false;

    //! Is "Allow to grab at top" flag using?
    bool en_grabtop = false;
    //! "Allow to grab at top" flag
    bool grabtop = false;

    //! Is "Hurt player on stomp attempt" flag using?
    bool en_jumphurt = false;
    //! "Hurt player on stomp attempt" flag
    bool jumphurt = false;

    //! Is "Safe for playable character" flag using?
    bool en_nohurt = false;
    //! "Safe for playable character" flag
    bool nohurt = false;

    //! Is "Don't collide with blocks" flag using?
    bool en_noblockcollision = false;
    //! "Don't collide with blocks" flag
    bool noblockcollision = false;

    //! Is "Turn on cliff" flag using?
    bool en_cliffturn = false;
    //! "Turn on cliff" flag
    bool cliffturn = false;

    //! Is "Not eatable" flag using?
    bool en_noyoshi = false;
    //! "Not eatable" flag
    bool noyoshi = false;

    //! Is "Foreground render" flag using?
    bool en_foreground = false;
    //! "Foreground render" flag
    bool foreground = false;

    //! Is Speed modifier field us using?
    bool en_speed = false;
    //! Speed modifier field
    double speed = 1.0;

    //! Is "Immune to fire" flag using?
    bool en_nofireball = false;
    //! "Immune to fire" flag
    bool nofireball = false;

    //! Is "Zero gravity" flag using?
    bool en_nogravity = false;
    //! Zero gravity flag
    bool nogravity = false;

    //! Is Frames number field using?
    bool en_frames = false;
    //! Frames number field
    uint32_t frames = 0;

    //! Is Frame speed modifier field using?
    bool en_framespeed = false;
    //! Frame speed modifier field
    uint32_t framespeed = 0;

    //! Is frame style field using?
    bool en_framestyle = false;
    //! Frame style field
    uint32_t framestyle = 0;

    //! Is "Immune to ice" flag using?
    bool en_noiceball = false;
    //! "Immune to ice" flag
    bool noiceball = false;


/*
 * Extended
 */
    //! Is "Immune to hammers/axes/knifes" flag using?
    bool en_nohammer = false;
    //! "Immune to hammers/axes/knifes" flag
    bool nohammer = false;

    //! Is "Immune to thrown NPC's" flag using?
    bool en_noshell = false;
    //! "Immune to thrown NPC's" flag
    bool noshell = false;

    //! Is NPC's Custom name field using?
    bool en_name = false;
    //! NPC's Custom name
    PGESTRING name;

    //! Is NPC's Custom description field using?
    bool en_description = false;
    //! NPC's Custom description
    PGESTRING description;

    //! Is custom health level field using?
    bool en_health = false;
    //! Custom health level field
    uint32_t health = 0;

    //! Is custom image file name field using?
    bool en_image = false;
    //! Is custom image file name field using?
    PGESTRING image;

    //! Is custom in-editor icon image file name field using?
    bool en_icon = false;
    //! Is custom in-editor icon image file name field using?
    PGESTRING icon;

    //! Is custom AI script filename field using?
    bool en_script = false;
    //! Custom AI Script filename field
    PGESTRING script;

    //! Is custom group name of NPC using?
    bool en_group = false;
    //! Custom group name of NPC
    PGESTRING group;

    //! Is custom category name of NPC using?
    bool en_category = false;
    //! Custom category name of NPC
    PGESTRING category;

    //! Is custom aligning grid size field using?
    bool en_grid = false;
    //! Custom aligning grid size field
    uint32_t grid = 1;

    //! Is custom grid offset X field using?
    bool en_gridoffsetx = false;
    //! Custom grid offset X field
    int32_t gridoffsetx = 0;

    //! Is custom grid offset Y field using?
    bool en_gridoffsety = false;
    //! Custom grid offset Y field
    int32_t gridoffsety = 0;

    //! Is custom grid align style field using?
    bool en_gridalign = false;
    //! Custom grid align style field
    uint32_t gridalign = 0;

    //! Is the "usedefaultcam" flag set?
    bool en_usedefaultcam = false;
    //! At player-set resolutions, should the NPC still activate based on the content's default resolution?
    bool usedefaultcam = false;

};

#endif // NPC_FILEDATA_H
