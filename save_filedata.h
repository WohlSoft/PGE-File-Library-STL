/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
 * \file save_filedata.h
 *
 * \brief Contains structure definitions for a game save data
 *
 */

#pragma once
#ifndef SAVE_FILEDATA_H
#define SAVE_FILEDATA_H

#include "pge_file_lib_globs.h"
#include "meta_filedata.h"

//! Game Save specific Visible element entry <array-id, is-vizible>
typedef PGEPAIR<unsigned int, bool > visibleItem;
//! Game Save specific gotten star entry <Level-Filename, Section-ID(SMBX64-Standard, one star per section) or NPC-ArrayID (PGE-X, multiple stars per section)>
typedef PGEPAIR<PGESTRING, int > starOnLevel;

/*!
 * \brief Recent state of each playable character
 */
struct saveCharState
{
    saveCharState() : id(1), state(1), itemID(0), mountType(0), mountID(0), health(1) {}
    //! ID of playable character
    unsigned long id = 0;
    //! Curent ID of playable character's state
    unsigned long state = 0;
    //! Current item ID in the item slot (SMBX64-only)
    unsigned long itemID = 0;
    //! Mounted vehicle type (SMBX64-only)
    unsigned int mountType = 0;
    //! Mounted vehicle ID (SMBX64-only)
    unsigned int mountID = 0;
    //! Recent health level
    unsigned int health = 0;
};

/*!
 * \brief Recent playable character ID per player ID
 */
struct savePlayerState
{
    //! ID of playable character
    int characterID;
};

/*!
 * \brief User data bank, middle format for (de)serialization.
 */
struct saveUserData
{
    enum DataLocation
    {
        DATA_WORLD = 0,
        DATA_LEVEL = 1,
        DATA_GLOBAL =2,
        DATA_LOCATION_MASK = 0x0000FFFF,
        DATA_VOLATILE_FLAG = 0x10000
    };

    struct DataEntry
    {
        PGESTRING key;
        PGESTRING value;
    };

    struct DataSection
    {
        //! Type of data location
        int                 location = DATA_WORLD;
        //! Optionally, for example, level filename
        PGESTRING           location_name;
        //! Name of data section
        PGESTRING           name;
        //! key=value Data entries
        PGELIST<DataEntry>  data;
    };
    //! Data store
    PGELIST<DataSection> store;
};


/*!
 * \brief Game save data structure
 */
struct GamesaveData
{
    //! Helper meta-data
    FileFormatMeta meta;

    //! Number of lives
    int lives = 0;
    //! Number of coins
    unsigned int coins = 0;
    //! Number of points
    unsigned int points = 0;
    //! Total stars
    unsigned int totalStars = 0;

    //! Last world map position X
    long worldPosX = 0;
    //! Last world map position Y
    long worldPosY = 0;

    //! Last entered/exited warp Array-ID on the HUB-based episodes.
    unsigned long last_hub_warp = 0;

    //! Current world music ID
    unsigned int musicID = 0;
    //! Current world music file (custom music)
    PGESTRING musicFile;

    //! Is episode was completed in last time
    bool gameCompleted = false;

    //! Extra data bank, saved via lua
    saveUserData userData;

    PGELIST<saveCharState > characterStates;
    PGELIST<unsigned long > currentCharacter;

    //Visible state of world map items
    PGELIST<visibleItem > visibleLevels;
    PGELIST<visibleItem > visiblePaths;
    PGELIST<visibleItem > visibleScenery;
    PGELIST<starOnLevel > gottenStars;
};

#endif // SAVE_FILEDATA_H
