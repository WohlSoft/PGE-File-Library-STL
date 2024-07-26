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

/*! \file mdx_gamesave_file_rw.cpp
 *
 *  \brief Implements MDX functions for loading a gamesave object
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "mdx/mdx_gamesave_file.h"
#include "mdx/common/mdx_exception.h"
#include "file_formats.h"
#include "pge_file_lib_private.h"
#include "pge_file_lib_globs.h"

static void s_on_error(void* _FileData, FileFormatsError& err)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    static_cast<FileFormatsError&>(FileData.meta) = std::move(err);
    FileData.meta.ReadFileValid = false;
}

static bool s_load_head(void* _FileData, GamesaveHead& dest)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.lives = dest.lives;
    FileData.hundreds = dest.hundreds;
    FileData.coins = dest.coins;
    FileData.points = dest.points;
    FileData.totalStars = dest.totalStars;
    FileData.worldPosX = dest.worldPosX;
    FileData.worldPosY = dest.worldPosY;
    FileData.last_hub_warp = dest.last_hub_warp;
    FileData.last_hub_level_file = dest.last_hub_level_file;
    FileData.musicID = dest.musicID;
    FileData.musicFile = dest.musicFile;
    FileData.gameCompleted = dest.gameCompleted;

    return true;
}

static bool s_save_head(const void* _FileData, GamesaveHead& dest, pge_size_t index)
{
    if(index != 0)
        return false;

    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    dest = GamesaveHead();

    dest.lives = FileData.lives;
    dest.hundreds = FileData.hundreds;
    dest.coins = FileData.coins;
    dest.points = FileData.points;
    dest.totalStars = FileData.totalStars;
    dest.worldPosX = FileData.worldPosX;
    dest.worldPosY = FileData.worldPosY;
    dest.last_hub_warp = FileData.last_hub_warp;
    dest.last_hub_level_file = FileData.last_hub_level_file;
    dest.musicID = FileData.musicID;
    dest.musicFile = FileData.musicFile;
    dest.gameCompleted = FileData.gameCompleted;

    return true;
}

static bool s_load_charstate(void* _FileData, saveCharState& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.characterStates.push_back(obj);

    return true;
}

static bool s_save_charstate(const void* _FileData, saveCharState& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.characterStates.size())
        return false;

    obj = FileData.characterStates[index];

    return true;
}

static bool s_load_selchar(void* _FileData, savePlayerState& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.currentCharacter.push_back(obj.characterID);

    return true;
}

static bool s_save_selchar(const void* _FileData, savePlayerState& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.currentCharacter.size())
        return false;

    obj.characterID = FileData.currentCharacter[index];

    return true;
}

static bool s_load_vis_level(void* _FileData, visibleItem& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.visibleLevels.push_back(obj);

    return true;
}

static bool s_save_vis_level(const void* _FileData, visibleItem& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.visibleLevels.size())
        return false;

    obj = FileData.visibleLevels[index];

    return true;
}

static bool s_load_vis_path(void* _FileData, visibleItem& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.visiblePaths.push_back(obj);

    return true;
}

static bool s_save_vis_path(const void* _FileData, visibleItem& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.visiblePaths.size())
        return false;

    obj = FileData.visiblePaths[index];

    return true;
}

static bool s_load_vis_scene(void* _FileData, visibleItem& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.visibleScenery.push_back(obj);

    return true;
}

static bool s_save_vis_scene(const void* _FileData, visibleItem& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.visibleScenery.size())
        return false;

    obj = FileData.visibleScenery[index];

    return true;
}

static bool s_load_star(void* _FileData, starOnLevel& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.gottenStars.push_back(obj);

    return true;
}

static bool s_save_star(const void* _FileData, starOnLevel& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.gottenStars.size())
        return false;

    obj = FileData.gottenStars[index];

    return true;
}

static bool s_load_level_info(void* _FileData, saveLevelInfo& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.levelInfo.push_back(obj);

    return true;
}

static bool s_save_level_info(const void* _FileData, saveLevelInfo& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.levelInfo.size())
        return false;

    obj = FileData.levelInfo[index];

    return true;
}

static bool s_load_userdata(void* _FileData, saveUserData::DataSection& obj)
{
    GamesaveData& FileData = *reinterpret_cast<GamesaveData*>(_FileData);

    FileData.userData.store.push_back(obj);

    return true;
}

static bool s_save_userdata(const void* _FileData, saveUserData::DataSection& obj, pge_size_t index)
{
    const GamesaveData& FileData = *reinterpret_cast<const GamesaveData*>(_FileData);

    if(index >= FileData.userData.store.size())
        return false;

    obj = FileData.userData.store[index];

    return true;
}

bool MDX_load_gamesave(PGE_FileFormats_misc::TextInput &file, GamesaveData &FileData)
{
    FileData = GamesaveData();
    FileData.lives = 3;

    //Add path data
    PGESTRING filePath = file.getFilePath();
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo  in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    FileData.meta.untitled = false;
    FileData.meta.modified = false;
    FileData.meta.ReadFileValid = true;

    GamesaveLoadCallbacks callbacks;

    callbacks.on_error = s_on_error;

    callbacks.load_head = s_load_head;
    callbacks.load_charstate = s_load_charstate;
    callbacks.load_selchar = s_load_selchar;
    callbacks.load_vis_level = s_load_vis_level;
    callbacks.load_vis_path = s_load_vis_path;
    callbacks.load_vis_scene = s_load_vis_scene;
    callbacks.load_star = s_load_star;
    callbacks.load_level_info = s_load_level_info;
    callbacks.load_userdata = s_load_userdata;

    callbacks.userdata = reinterpret_cast<void*>(&FileData);

    return MDX_load_gamesave(file, callbacks);
}

bool MDX_save_gamesave(PGE_FileFormats_misc::TextOutput &file, const GamesaveData &FileData)
{
    GamesaveSaveCallbacks callbacks;

    callbacks.save_head = s_save_head;
    callbacks.save_charstate = s_save_charstate;
    callbacks.save_selchar = s_save_selchar;
    callbacks.save_vis_level = s_save_vis_level;
    callbacks.save_vis_path = s_save_vis_path;
    callbacks.save_vis_scene = s_save_vis_scene;
    callbacks.save_star = s_save_star;
    callbacks.save_level_info = s_save_level_info;
    callbacks.save_userdata = s_save_userdata;

    callbacks.userdata = reinterpret_cast<const void*>(&FileData);

    return MDX_save_gamesave(file, callbacks);
}
