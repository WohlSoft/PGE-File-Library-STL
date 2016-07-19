/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 *  \file wld_filedata.h
 *  \brief Contains data structure definitions for a world map file data
 */

#ifndef WLD_FILEDATA_H
#define WLD_FILEDATA_H

#include "pge_file_lib_globs.h"
#include "meta_filedata.h"

//////////////////////World file Data//////////////////////
/**
 * @brief Terrain tile
 */
struct WorldTerrainTile
{
    //! Position X
    long x;
    //! Position Y
    long y;
    //! ID of terrain tile
    unsigned long id;
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Scenery object
 */
struct WorldScenery
{
    //! Position X
    long x;
    //! Position Y
    long y;
    //! ID of scenery object
    unsigned long id;
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Path tile
 */
struct WorldPathTile
{
    //! Position X
    long x;
    //! Position Y
    long y;
    //! ID of path tile
    unsigned long id;
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Level entrance tile
 */
struct WorldLevelTile
{
    //! Position X
    long x;
    //! PositionY
    long y;
    //! ID of level entrance object
    unsigned long id;
    //! Target level file to enter
    PGESTRING lvlfile;
    //! Visible title of the level
    PGESTRING title;
    //! Open top path on exit code
    int top_exit;
    //! Open left path on exit code
    int left_exit;
    //! Open bottom path on exit code
    int bottom_exit;
    //! Open right path on exit code
    int right_exit;
    //! Target Warp-ID to enter level
    unsigned long entertowarp;
    //! Level is always shown on map even not opened
    bool alwaysVisible;
    //! Show path background image under main sprite
    bool pathbg;
    //! Is this level entrance point is initial position of player on game start
    bool gamestart;
    //! Teleport to X coordinate of the world map (-1 don't teleport)
    long gotox;
    //! Teleport to Y coordinate of the world map (-1 don't teleport)
    long gotoy;
    //! Show big path background image under main sprite
    bool bigpathbg;
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Music box object
 */
struct WorldMusicBox
{
    //! Position X
    long x;
    //! Position Y
    long y;
    //! ID of starnard music box
    unsigned long id;
    //! Custom music file to play on touch
    PGESTRING music_file;
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief World map data structure
 */
struct WorldData
{
    //! Helper meta-data
    FileFormatMeta meta;
    /*!
     * \brief File format
     */
    enum FileFormat
    {
        //! PGE-X WLDX file format
        PGEX=0,
        //! SMBX1...64 WLD file format
        SMBX64,
        //! SMBX-38A WLD file format
        SMBX38A
    };

    PGESTRING EpisodeTitle;
    bool nocharacter1;
    bool nocharacter2;
    bool nocharacter3;
    bool nocharacter4;
    bool nocharacter5;

    PGELIST<bool > nocharacter;

    PGESTRING IntroLevel_file;
    bool HubStyledWorld;
    bool restartlevel;

    unsigned int stars;

    PGESTRING authors;
    PGESTRING author1;
    PGESTRING author2;
    PGESTRING author3;
    PGESTRING author4;
    PGESTRING author5;

    PGELIST<WorldTerrainTile > tiles;
    unsigned int tile_array_id;
    PGELIST<WorldScenery > scenery;
    unsigned int scene_array_id;
    PGELIST<WorldPathTile > paths;
    unsigned int path_array_id;
    PGELIST<WorldLevelTile > levels;
    unsigned int level_array_id;
    PGELIST<WorldMusicBox > music;
    unsigned int musicbox_array_id;

    //meta:
    MetaData metaData;

    //editing:
    int     CurSection;
    bool    playmusic;
    int     currentMusic;
};

#endif // WLD_FILEDATA_H
