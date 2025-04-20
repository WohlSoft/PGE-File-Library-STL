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

/*! \file mdx_world_file_rw.cpp
 *
 *  \brief Implements MDX functions for loading a world object
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "mdx/mdx_world_file.h"
#include "mdx/common/mdx_exception.h"
#include "file_formats.h"
#include "pge_file_lib_private.h"
#include "pge_file_lib_globs.h"

static void s_on_error(void* _FileData, FileFormatsError& err)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    static_cast<FileFormatsError&>(FileData.meta) = std::move(err);
    FileData.meta.ReadFileValid = false;
}

static bool s_load_head(void* _FileData, WorldHead& dest)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    FileData.EpisodeTitle = dest.EpisodeTitle;
    FileData.nocharacter = dest.nocharacter;
    FileData.IntroLevel_file = dest.IntroLevel_file;
    FileData.GameOverLevel_file = dest.GameOverLevel_file;
    FileData.HubStyledWorld = dest.HubStyledWorld;
    FileData.restartlevel = dest.restartlevel;
    FileData.stars = dest.stars;
    FileData.authors = dest.authors;
    FileData.authors_music = dest.authors_music;
    FileData.starsShowPolicy = dest.starsShowPolicy;
    FileData.custom_params = dest.custom_params;
    FileData.meta.configPackId = dest.configPackId;
    FileData.meta.engineFeatureLevel = dest.engineFeatureLevel;
    FileData.meta.RecentFormat = dest.RecentFormat;
    FileData.meta.RecentFormatVersion = dest.RecentFormatVersion;

    // unpack nocharacter array for some loaders
    FileData.charactersToS64();

    if(FileData.meta.RecentFormat == LevelData::SMBX38A)
        FileData.meta.smbx64strict = false;
    else if(FileData.meta.RecentFormat == LevelData::SMBX64)
        FileData.meta.smbx64strict = true;

    return true;
}

static bool s_load_head_only(void* _FileData, WorldHead& dest)
{
    s_load_head(_FileData, dest);
    throw PGE_FileFormats_misc::callback_interrupt();
}

static bool s_save_head(const void* _FileData, WorldHead& dest, pge_size_t index)
{
    if(index != 0)
        return false;

    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    dest = WorldHead();

    dest.EpisodeTitle = FileData.EpisodeTitle;
    dest.nocharacter = FileData.nocharacter;
    dest.IntroLevel_file = FileData.IntroLevel_file;
    dest.GameOverLevel_file = FileData.GameOverLevel_file;
    dest.HubStyledWorld = FileData.HubStyledWorld;
    dest.restartlevel = FileData.restartlevel;
    dest.stars = FileData.stars;
    dest.authors = FileData.authors;
    dest.authors_music = FileData.authors_music;
    dest.starsShowPolicy = FileData.starsShowPolicy;
    dest.custom_params = FileData.custom_params;
    dest.configPackId = FileData.meta.configPackId;
    dest.engineFeatureLevel = FileData.meta.engineFeatureLevel;

    // clear no-character array if unnecessary
    bool any_no_character = false;
    for(bool b : dest.nocharacter)
    {
        if(b)
        {
            any_no_character = true;
            break;
        }
    }

    if(!any_no_character)
        dest.nocharacter.clear();

    return true;
}

static bool s_load_bookmark(void* _FileData, Bookmark& dest)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);
    FileData.metaData.bookmarks.push_back(std::move(dest));

    return true;
}

static bool s_save_bookmark(const void* _FileData, Bookmark& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.metaData.bookmarks.size())
        return false;

    obj = FileData.metaData.bookmarks[index];

    return true;
}

static bool s_load_crash_data(void* _FileData, CrashData& dest)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);
    FileData.metaData.crash = dest;
    FileData.metaData.crash.used = true;

    return true;
}

static bool s_save_crash_data(const void* _FileData, CrashData& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(!FileData.metaData.crash.used || index != 0)
        return false;

    obj = FileData.metaData.crash;

    return true;
}

static bool s_load_tile(void* _FileData, WorldTerrainTile& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.tile_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.tiles.size());
    FileData.tiles.push_back(obj);

    return true;
}

static bool s_save_tile(const void* _FileData, WorldTerrainTile& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.tiles.size())
        return false;

    obj = FileData.tiles[index];

    return true;
}

static bool s_load_scene(void* _FileData, WorldScenery& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.scene_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.scenery.size());
    FileData.scenery.push_back(obj);

    return true;
}

static bool s_save_scene(const void* _FileData, WorldScenery& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.scenery.size())
        return false;

    obj = FileData.scenery[index];

    return true;
}

static bool s_load_path(void* _FileData, WorldPathTile& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.path_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.paths.size());
    FileData.paths.push_back(obj);

    return true;
}

static bool s_save_path(const void* _FileData, WorldPathTile& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.paths.size())
        return false;

    obj = FileData.paths[index];

    return true;
}

static bool s_load_music(void* _FileData, WorldMusicBox& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.musicbox_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.music.size());
    FileData.music.push_back(obj);

    return true;
}

static bool s_save_music(const void* _FileData, WorldMusicBox& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.music.size())
        return false;

    obj = FileData.music[index];

    return true;
}

static bool s_load_arearect(void* _FileData, WorldAreaRect& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.arearect_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.arearects.size());
    FileData.arearects.push_back(obj);

    return true;
}

static bool s_save_arearect(const void* _FileData, WorldAreaRect& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.arearects.size())
        return false;

    obj = FileData.arearects[index];

    return true;
}

static bool s_load_level(void* _FileData, WorldLevelTile& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.level_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.levels.size());
    FileData.levels.push_back(obj);

    return true;
}

static bool s_save_level(const void* _FileData, WorldLevelTile& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.levels.size())
        return false;

    obj = FileData.levels[index];

    return true;
}

static bool s_load_layer38a(void* _FileData, WorldLayer& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.layers_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.layers.size());
    FileData.layers.push_back(obj);

    return true;
}

static bool s_save_layer38a(const void* _FileData, WorldLayer& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.layers.size())
        return false;

    obj = FileData.layers[index];

    return true;
}

static bool s_load_event38a(void* _FileData, WorldEvent38A& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    obj.meta.array_id = FileData.events38A_array_id++;
    obj.meta.index = static_cast<unsigned int>(FileData.events38A.size());
    FileData.events38A.push_back(obj);

    return true;
}

static bool s_save_event38a(const void* _FileData, WorldEvent38A& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.events38A.size())
        return false;

    obj = FileData.events38A[index];

    return true;
}

static bool s_load_config38a(void* _FileData, WorldItemSetup38A& obj)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    FileData.custom38A_configs.push_back(obj);

    return true;
}

static bool s_save_config38a(const void* _FileData, WorldItemSetup38A& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.custom38A_configs.size())
        return false;

    obj = FileData.custom38A_configs[index];

    return true;
}

static bool s_load_junk_line(void* _FileData, PGESTRING& src)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);
    FileData.unsupported_38a_lines.push_back(std::move(src));

    return true;
}

static bool s_save_junk_line(const void* _FileData, PGESTRING& obj, pge_size_t index)
{
    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    if(index >= FileData.unsupported_38a_lines.size())
        return false;

    obj = FileData.unsupported_38a_lines[index];

    return true;
}

static bool s_load_head38a(void* _FileData, WorldHead38A& src)
{
    WorldData& FileData = *reinterpret_cast<WorldData*>(_FileData);

    static_cast<WorldHead38A&>(FileData) = src;

    return true;
}

static bool s_save_head38a(const void* _FileData, WorldHead38A& obj, pge_size_t index)
{
    if(index != 0)
        return false;

    const WorldData& FileData = *reinterpret_cast<const WorldData*>(_FileData);

    obj = static_cast<const WorldHead38A&>(FileData);

    return true;
}

bool MDX_load_world(PGE_FileFormats_misc::TextInput &file, WorldData &FileData)
{
    FileFormats::CreateWorldData(FileData);
    FileData.meta.RecentFormat = WorldData::PGEX;

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

    return MDX_load_world(file, PGEFL_make_load_callbacks(FileData));
}

WorldLoadCallbacks PGEFL_make_load_callbacks(WorldData& target)
{
    WorldLoadCallbacks callbacks;

    callbacks.on_error = s_on_error;

    callbacks.load_head = s_load_head;
    callbacks.load_bookmark = s_load_bookmark;
    callbacks.load_crash_data = s_load_crash_data;
    callbacks.load_tile = s_load_tile;
    callbacks.load_scene = s_load_scene;
    callbacks.load_path = s_load_path;
    callbacks.load_music = s_load_music;
    callbacks.load_arearect = s_load_arearect;
    callbacks.load_level = s_load_level;

    callbacks.load_head38a = s_load_head38a;
    callbacks.load_layer38a = s_load_layer38a;
    callbacks.load_event38a = s_load_event38a;
    callbacks.load_config38a = s_load_config38a;
    callbacks.load_junk_line = s_load_junk_line; // but aren't they all junk lines?

    callbacks.userdata = reinterpret_cast<void*>(&target);

    return callbacks;
}

bool MDX_load_world_header(PGE_FileFormats_misc::TextInput &file, WorldData &FileData)
{
    WorldHead h;
    s_load_head(&FileData, h);
    FileData.meta.RecentFormat = LevelData::PGEX;

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

    return MDX_load_world(file, PGEFL_make_header_load_callbacks(FileData));
}

WorldLoadCallbacks PGEFL_make_header_load_callbacks(WorldData& target)
{
    WorldLoadCallbacks callbacks;

    callbacks.on_error = s_on_error;

    callbacks.load_head = s_load_head_only;

    callbacks.userdata = reinterpret_cast<void*>(&target);

    return callbacks;
}

bool MDX_save_world(PGE_FileFormats_misc::TextOutput &file, const WorldData &FileData)
{
    return MDX_save_world(file, PGEFL_make_save_callbacks(FileData));
}

WorldSaveCallbacks PGEFL_make_save_callbacks(const WorldData& target)
{
    WorldSaveCallbacks callbacks;

    callbacks.save_head = s_save_head;
    callbacks.save_bookmark = s_save_bookmark;
    callbacks.save_crash_data = s_save_crash_data;
    callbacks.save_tile = s_save_tile;
    callbacks.save_scene = s_save_scene;
    callbacks.save_path = s_save_path;
    callbacks.save_music = s_save_music;
    callbacks.save_arearect = s_save_arearect;
    callbacks.save_level = s_save_level;

    callbacks.save_head38a = s_save_head38a;
    callbacks.save_layer38a = s_save_layer38a;
    callbacks.save_event38a = s_save_event38a;
    callbacks.save_config38a = s_save_config38a;
    callbacks.save_junk_line = s_save_junk_line;

    callbacks.userdata = reinterpret_cast<const void*>(&target);

    return callbacks;
}
