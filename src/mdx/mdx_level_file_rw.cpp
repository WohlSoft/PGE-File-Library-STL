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

/*! \file mdx_level_file_rw.cpp
 *
 *  \brief Implements defines MDX functions for loading a level object
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "mdx/mdx_level_file.h"
#include "mdx/common/mdx_exception.h"
#include "file_formats.h"
#include "pge_file_lib_private.h"
#include "pge_file_lib_globs.h"

static void s_on_error(void* _FileData, FileFormatsError& err)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    static_cast<FileFormatsError&>(FileData.meta) = std::move(err);
    FileData.meta.ReadFileValid = false;
}

static bool s_load_head(void* _FileData, LevelHead& dest)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    FileData.LevelName = dest.LevelName;
    FileData.stars = dest.stars;
    FileData.open_level_on_fail = dest.open_level_on_fail;
    FileData.open_level_on_fail_warpID = dest.open_level_on_fail_warpID;
    FileData.player_names_overrides = dest.player_names_overrides;
    FileData.custom_params = dest.custom_params;
    FileData.meta.configPackId = dest.configPackId;
    FileData.music_files = dest.music_files;

    FileData.music_overrides = dest.music_overrides;

    FileData.meta.RecentFormat = dest.RecentFormat;
    FileData.meta.RecentFormatVersion = dest.RecentFormatVersion;

    if(FileData.meta.RecentFormat == LevelData::SMBX38A)
        FileData.meta.smbx64strict = false;
    else if(FileData.meta.RecentFormat == LevelData::SMBX64)
        FileData.meta.smbx64strict = true;

    return true;
}

static bool s_load_head_only(void* _FileData, LevelHead& dest)
{
    s_load_head(_FileData, dest);
    throw PGE_FileFormats_misc::callback_interrupt();
}

static bool s_save_head(const void* _FileData, LevelHead& dest, pge_size_t index)
{
    if(index != 0)
        return false;

    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    dest = LevelHead();

    dest.LevelName = FileData.LevelName;
    dest.stars = FileData.stars;
    dest.open_level_on_fail = FileData.open_level_on_fail;
    dest.open_level_on_fail_warpID = FileData.open_level_on_fail_warpID;
    dest.player_names_overrides = FileData.player_names_overrides;
    dest.custom_params = FileData.custom_params;
    dest.configPackId = FileData.meta.configPackId;
    dest.music_files = FileData.music_files;

    return true;
}

static bool s_load_bookmark(void* _FileData, Bookmark& dest)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.metaData.bookmarks.push_back(std::move(dest));

    return true;
}

static bool s_save_bookmark(const void* _FileData, Bookmark& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.metaData.bookmarks.size())
        return false;

    obj = FileData.metaData.bookmarks[index];

    return true;
}

static bool s_load_crash_data(void* _FileData, CrashData& dest)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.metaData.crash = dest;
    FileData.metaData.crash.used = true;

    return true;
}

static bool s_save_crash_data(const void* _FileData, CrashData& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(!FileData.metaData.crash.used || index != 0)
        return false;

    obj = FileData.metaData.crash;

    return true;
}

static bool s_load_section(void* _FileData, LevelSection& dest)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    dest.PositionX = dest.size_left - 10;
    dest.PositionY = dest.size_top - 10;

    //add captured value into array
    pge_size_t sections_count = FileData.sections.size();

    if(dest.id < 0 || dest.id > 1000)
        throw PGE_FileFormats_misc::callback_error("Invalid section ID");

    if(dest.id >= static_cast<int>(sections_count))
    {
        pge_size_t needToAdd = static_cast<pge_size_t>(dest.id) - (FileData.sections.size() - 1);
        while(needToAdd > 0)
        {
            LevelSection dummySct = FileFormats::CreateLvlSection();
            dummySct.id = (int)FileData.sections.size();
            FileData.sections.push_back(std::move(dummySct));
            needToAdd--;
        }
    }

    FileData.sections[static_cast<pge_size_t>(dest.id)] = std::move(dest);

    return true;
}

static bool s_save_section(const void* _FileData, LevelSection& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.sections.size())
        return false;

    obj = FileData.sections[index];

    return true;
}

static bool s_load_startpoint(void* _FileData, PlayerPoint& player)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    //add captured value into array
    bool found = false;
    pge_size_t q = 0;
    pge_size_t playersCount = FileData.players.size();
    for(q = 0; q < playersCount; q++)
    {
        if(FileData.players[q].id == player.id)
        {
            found = true;
            break;
        }
    }

    PlayerPoint sz = FileFormats::CreateLvlPlayerPoint(player.id);
    if(player.w == 0)
        player.w = sz.w;
    if(player.h == 0)
        player.h = sz.h;

    if(found)
        FileData.players[q] = std::move(player);
    else
        FileData.players.push_back(std::move(player));

    return true;
}

static bool s_save_startpoint(const void* _FileData, PlayerPoint& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.players.size())
        return false;

    obj = FileData.players[index];

    return true;
}

static bool s_load_block(void* _FileData, LevelBlock& block)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    block.meta.array_id = FileData.blocks_array_id++;
    block.meta.index = static_cast<unsigned int>(FileData.blocks.size());
    FileData.blocks.push_back(std::move(block));

    return true;
}

static bool s_save_block(const void* _FileData, LevelBlock& block, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.blocks.size())
        return false;

    block = FileData.blocks[index];

    return true;
}

static bool s_load_bgo(void* _FileData, LevelBGO& bgodata)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    bgodata.meta.array_id = FileData.bgo_array_id++;
    bgodata.meta.index = static_cast<unsigned int>(FileData.bgo.size());
    FileData.bgo.push_back(std::move(bgodata));

    return true;
}

static bool s_save_bgo(const void* _FileData, LevelBGO& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.bgo.size())
        return false;

    obj = FileData.bgo[index];

    return true;
}

static bool s_load_npc(void* _FileData, LevelNPC& npcdata)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    npcdata.meta.array_id = FileData.npc_array_id++;
    npcdata.meta.index = static_cast<unsigned int>(FileData.npc.size());
    FileData.npc.push_back(std::move(npcdata));

    return true;
}

static bool s_save_npc(const void* _FileData, LevelNPC& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.npc.size())
        return false;

    obj = FileData.npc[index];

    return true;
}

static bool s_load_phys(void* _FileData, LevelPhysEnv& physiczone)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    physiczone.meta.array_id = FileData.physenv_array_id++;
    physiczone.meta.index = static_cast<unsigned int>(FileData.physez.size());
    FileData.physez.push_back(std::move(physiczone));

    return true;
}

static bool s_save_phys(const void* _FileData, LevelPhysEnv& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.physez.size())
        return false;

    obj = FileData.physez[index];

    return true;
}

static bool s_load_warp(void* _FileData, LevelDoor& door)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    door.isSetIn = (!door.lvl_i);
    door.isSetOut = (!door.lvl_o || (door.lvl_i));

    if(!door.isSetIn && door.isSetOut)
    {
        door.ix = door.ox;
        door.iy = door.oy;
    }

    if(!door.isSetOut && door.isSetIn)
    {
        door.ox = door.ix;
        door.oy = door.iy;
    }

    door.meta.array_id = FileData.doors_array_id++;
    door.meta.index = static_cast<unsigned int>(FileData.doors.size());
    FileData.doors.push_back(std::move(door));

    return true;
}

static bool s_save_warp(const void* _FileData, LevelDoor& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.doors.size())
        return false;

    obj = FileData.doors[index];

    return true;
}

static bool s_load_layer(void* _FileData, LevelLayer& layer)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    //add captured value into array
    bool found = false;
    pge_size_t q = 0;
    for(q = 0; q < FileData.layers.size(); q++)
    {
        if(FileData.layers[q].name == layer.name)
        {
            found = true;
            break;
        }
    }

    if(found)
    {
        layer.meta.array_id = FileData.layers[q].meta.array_id;
        FileData.layers[q] = std::move(layer);
    }
    else
    {
        layer.meta.array_id = FileData.layers_array_id++;
        FileData.layers.push_back(std::move(layer));
    }

    return true;
}

static bool s_save_layer(const void* _FileData, LevelLayer& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.layers.size())
        return false;

    obj = FileData.layers[index];

    return true;
}

static bool s_load_event(void* _FileData, LevelSMBX64Event& event)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);

    // create a new padded list of the event sets
    PGELIST<LevelEvent_Sets> padded_sets;
    for(LevelEvent_Sets& sectionSet : event.sets)
    {
        if(
            ((sectionSet.id < 0) || (sectionSet.id >= static_cast<long>(padded_sets.size())))
        )//Append sections
        {
            if(sectionSet.id < 0 || sectionSet.id > 1000)
                throw PGE_FileFormats_misc::callback_error("Invalid section ID");

            long last = static_cast<long>(padded_sets.size() - 1);

            while(sectionSet.id >= static_cast<long>(padded_sets.size()))
            {
                LevelEvent_Sets set;
                set.id = last;
                padded_sets.push_back(set);
                last++;
            }
        }

        padded_sets[static_cast<pge_size_t>(sectionSet.id)] = std::move(sectionSet);
    }

    event.sets = std::move(padded_sets);

    //add captured value into array
    bool found = false;
    pge_size_t q = 0;

    for(q = 0; q < FileData.events.size(); q++)
    {
        if(FileData.events[q].name == event.name)
        {
            found = true;
            break;
        }
    }

    if(found)
    {
        event.meta.array_id = FileData.events[q].meta.array_id;
        FileData.events[q] = std::move(event);
    }
    else
    {
        event.meta.array_id = FileData.events_array_id++;
        FileData.events.push_back(std::move(event));
    }

    return true;
}

static bool s_save_event(const void* _FileData, LevelSMBX64Event& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.events.size())
        return false;

    obj = FileData.events[index];

    return true;
}

static bool s_load_var(void* _FileData, LevelVariable& v)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.variables.push_back(std::move(v));

    return true;
}

static bool s_save_var(const void* _FileData, LevelVariable& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.variables.size())
        return false;

    obj = FileData.variables[index];

    return true;
}

static bool s_load_arr(void* _FileData, LevelArray& a)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.arrays.push_back(std::move(a));

    return true;
}

static bool s_save_arr(const void* _FileData, LevelArray& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.arrays.size())
        return false;

    obj = FileData.arrays[index];

    return true;
}

static bool s_load_script(void* _FileData, LevelScript& s)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.scripts.push_back(std::move(s));

    return true;
}

static bool s_save_script(const void* _FileData, LevelScript& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.scripts.size())
        return false;

    obj = FileData.scripts[index];

    return true;
}

static bool s_load_levelitem38a(void* _FileData, LevelItemSetup38A& customcfg38A)
{
    if(customcfg38A.type < 0)
        throw PGE_FileFormats_misc::callback_error("Invalid 38A ID");

    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.custom38A_configs.push_back(std::move(customcfg38A));

    return true;
}

static bool s_save_levelitem38a(const void* _FileData, LevelItemSetup38A& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.custom38A_configs.size())
        return false;

    obj = FileData.custom38A_configs[index];

    return true;
}

static bool s_load_music_override(void* _FileData, LevelData::MusicOverrider& src)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.music_overrides.push_back(std::move(src));

    return true;
}

static bool s_save_music_override(const void* _FileData, LevelData::MusicOverrider& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.music_overrides.size())
        return false;

    obj = FileData.music_overrides[index];

    return true;
}

static bool s_load_junk_line(void* _FileData, PGESTRING& src)
{
    LevelData& FileData = *reinterpret_cast<LevelData*>(_FileData);
    FileData.unsupported_38a_lines.push_back(std::move(src));

    return true;
}

static bool s_save_junk_line(const void* _FileData, PGESTRING& obj, pge_size_t index)
{
    const LevelData& FileData = *reinterpret_cast<const LevelData*>(_FileData);

    if(index >= FileData.unsupported_38a_lines.size())
        return false;

    obj = FileData.unsupported_38a_lines[index];

    return true;
}

bool MDX_load_level(PGE_FileFormats_misc::TextInput &file, LevelData &FileData)
{
    FileFormats::CreateLevelData(FileData);
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

    return MDX_load_level(file, PGEFL_make_load_callbacks(FileData));
}

LevelLoadCallbacks PGEFL_make_load_callbacks(LevelData& target)
{
    LevelLoadCallbacks callbacks;

    callbacks.on_error = s_on_error;

    callbacks.load_head = s_load_head;
    callbacks.load_bookmark = s_load_bookmark;
    callbacks.load_crash_data = s_load_crash_data;
    callbacks.load_section = s_load_section;
    callbacks.load_startpoint = s_load_startpoint;
    callbacks.load_block = s_load_block;
    callbacks.load_bgo = s_load_bgo;
    callbacks.load_npc = s_load_npc;
    callbacks.load_phys = s_load_phys;
    callbacks.load_warp = s_load_warp;
    callbacks.load_layer = s_load_layer;
    callbacks.load_event = s_load_event;
    callbacks.load_var = s_load_var;
    callbacks.load_arr = s_load_arr;
    callbacks.load_script = s_load_script;
    callbacks.load_levelitem38a = s_load_levelitem38a;
    callbacks.load_music_override = s_load_music_override;
    callbacks.load_junk_line = s_load_junk_line;

    callbacks.userdata = reinterpret_cast<void*>(&target);

    return callbacks;
}

bool MDX_load_level_header(PGE_FileFormats_misc::TextInput &file, LevelData &FileData)
{
    LevelHead h;
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

    return MDX_load_level(file, PGEFL_make_header_load_callbacks(FileData));
}

LevelLoadCallbacks PGEFL_make_header_load_callbacks(LevelData& target)
{
    LevelLoadCallbacks callbacks;

    callbacks.on_error = s_on_error;

    callbacks.load_head = s_load_head_only;

    callbacks.userdata = reinterpret_cast<void*>(&target);

    return callbacks;
}

LevelSaveCallbacks PGEFL_make_save_callbacks(const LevelData& target)
{
    LevelSaveCallbacks callbacks;

    callbacks.save_head = s_save_head;
    callbacks.save_bookmark = s_save_bookmark;
    callbacks.save_crash_data = s_save_crash_data;
    callbacks.save_section = s_save_section;
    callbacks.save_startpoint = s_save_startpoint;
    callbacks.save_block = s_save_block;
    callbacks.save_bgo = s_save_bgo;
    callbacks.save_npc = s_save_npc;
    callbacks.save_phys = s_save_phys;
    callbacks.save_warp = s_save_warp;
    callbacks.save_layer = s_save_layer;
    callbacks.save_event = s_save_event;
    callbacks.save_var = s_save_var;
    callbacks.save_arr = s_save_arr;
    callbacks.save_script = s_save_script;
    callbacks.save_levelitem38a = s_save_levelitem38a;
    callbacks.save_music_override = s_save_music_override;
    callbacks.save_junk_line = s_save_junk_line;

    callbacks.userdata = reinterpret_cast<const void*>(&target);

    return callbacks;
}

bool MDX_save_level(PGE_FileFormats_misc::TextOutput &file, const LevelData &FileData)
{
    return MDX_save_level(file, PGEFL_make_save_callbacks(FileData));
}
