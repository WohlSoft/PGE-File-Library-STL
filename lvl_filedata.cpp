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

#include "file_formats.h"
#include "lvl_filedata.h"
#include "pge_file_lib_private.h"

#include <stack>

/*********************************************************************************/
/***************************SMBX64-Specific features******************************/
/*********************************************************************************/

//Built-in order priorities per SMBX-64 BGO's
static const int _smbx64_bgo_sort_priorities[190] =
{
    77, 75, 75, 75, 75, 75, 75, 75, 75, 75, 20, 20, 75, 10, 75, 75, 75, 75, 75, 75, 75, 75, 125, 125, 125, 26,
    75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 125, 125, 75, 80, 125, 125, 125, 30,
    75, 75, 75, 75, 75, 75, 75, 20, 20, 75, 75, 75, 26, 25, 75, 125, 125, 90, 90, 90, 90, 90, 10, 10, 10, 10, 30,
    75, 75, 26, 26, 75, 75, 75, 98, 98, 75, 75, 75, 98, 75, 75, 75, 75, 75, 75, 99, 75, 75, 75, 75, 98, 98, 125,
    98, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 76, 76, 76, 75, 75,
    75, 75, 75, 125, 125, 80, 80, 90, 75, 125, 75, 125, 75, 75, 75, 75, 75, 75, 75, 75, 125, 125, 125, 125, 25,
    25, 75, 75, 75, 75, 26, 26, 26, 26, 26, 26, 75, 75, 25, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,
    75, 125, 125, 75, 75
};

void FileFormats::smbx64LevelPrepare(LevelData &lvl)
{
    //Set SMBX64 specific option to BGO
    for(pge_size_t q = 0; q < lvl.bgo.size(); q++)
    {
        if(lvl.bgo[q].smbx64_sp < 0)
        {
            if((lvl.bgo[q].id > 0u) && (lvl.bgo[q].id <= 190u))
                lvl.bgo[q].smbx64_sp_apply = _smbx64_bgo_sort_priorities[lvl.bgo[q].id - 1];
        }
        else
            lvl.bgo[q].smbx64_sp_apply = lvl.bgo[q].smbx64_sp;
    }

    //Mark & Count Stars
    lvl.stars = smbx64CountStars(lvl);
}

int FileFormats::smbx64CountStars(LevelData &lvl)
{
    int stars = 0;
    for(pge_size_t q = 0; q < lvl.npc.size(); q++)
    {
        LevelNPC &npc = lvl.npc[q];
        npc.is_star = ((npc.id == 97) || (npc.id == 196)) && !npc.friendly;
        if(npc.is_star)
            stars += 1;
    }
    return stars;
}

template<class T, class U>
void quickSort(T &array, bool (*qLess)(const U&, const U&), bool (*qMore)(const U&, const U&))
{
    if(array.size() <= 1)
        return; //Nothing to sort!

    class my_stack : public std::stack< int >
    {
    public:
        using std::stack<int>::c; // expose the container
    };

    my_stack beg;
    my_stack end;
    U piv;
    int i = 0, L, R, swapv;
    beg.push(0);
    end.push((int)array.size());
#define ST(x) static_cast<pge_size_t>(x)
    while(i >= 0)
    {
        L = beg.c[ST(i)];
        R = end.c[ST(i)] - 1;
        if(L < R)
        {
            piv = array[ST(L)];
            while(L < R)
            {
                while(qMore(array[ST(R)], piv) && (L < R))
                    R--;
                if(L < R)
                    array[ST(L++)] = array[ST(R)];
                while(qLess(array[ST(L)], piv) && (L < R))
                    L++;
                if(L < R)
                    array[ST(R--)] = array[ST(L)];
            }
            array[ST(L)] = piv;
            beg.push(L + 1);
            end.push(end.c[ST(i)]);
            end.c[ST(i++)] = (L);
            if((end.c[ST(i)] - beg.c[ST(i)]) > (end.c[ST(i - 1)] - beg.c[ST(i - 1)]))
            {
                swapv = beg.c[ST(i)];
                beg.c[ST(i)] = beg.c[ST(i - 1)];
                beg.c[ST(i - 1)] = swapv;
                swapv = end.c[ST(i)];
                end.c[ST(i)] = end.c[ST(i - 1)];
                end.c[ST(i - 1)] = swapv;
            }
        }
        else
        {
            i--;
            beg.pop();
            end.pop();
        }
    }
#undef ST
}


/* Blocks sorting conditions for SMBX-64 standard*/

static inline bool blkMore(const LevelBlock &a, const LevelBlock &b)
{
    return (a.x > b.x) ||
          ((a.x == b.x) && (a.y >  b.y)) ||
          ((a.x == b.x) && (a.y == b.y) && (a.meta.array_id >= b.meta.array_id));
}

static inline bool blkLess(const LevelBlock &a, const LevelBlock &b)
{
    return (a.x < b.x) ||
          ((a.x == b.x) && (a.y <  b.y)) ||
          ((a.x == b.x) && (a.y == b.y) && (a.meta.array_id <= b.meta.array_id));
}

void FileFormats::smbx64LevelSortBlocks(LevelData &lvl)
{
    quickSort<PGELIST<LevelBlock>, LevelBlock>(lvl.blocks, blkLess, blkMore);
}


/* BGO sorting conditions for SMBX-64 standard*/

static inline bool smbx64BgoMore(const LevelBGO &a, const LevelBGO &b)
{
    bool sp_gr = (a.smbx64_sp_apply > b.smbx64_sp_apply);
    bool sp_eq = (a.smbx64_sp_apply == b.smbx64_sp_apply);

    bool arrayId_ge = a.meta.array_id >= b.meta.array_id;

    return  sp_gr || (sp_eq && arrayId_ge);
}

static inline bool smbx64BgoLess(const LevelBGO &a, const LevelBGO &b)
{
    bool sp_lt = (a.smbx64_sp_apply < b.smbx64_sp_apply);
    bool sp_eq = (a.smbx64_sp_apply == b.smbx64_sp_apply);

    bool arrayId_le = a.meta.array_id <= b.meta.array_id;

    return  sp_lt || (sp_eq && arrayId_le);
}

void FileFormats::smbx64LevelSortBGOs(LevelData &lvl)
{
    quickSort<PGELIST<LevelBGO>, LevelBGO>(lvl.bgo, smbx64BgoLess, smbx64BgoMore);
}


/* BGO sorting conditions for SMBX2 */

static inline bool sbx2BgoMore(const LevelBGO &a, const LevelBGO &b)
{
    bool sp_gr = (a.smbx64_sp_apply > b.smbx64_sp_apply);
    bool sp_eq = (a.smbx64_sp_apply == b.smbx64_sp_apply);
    bool zOffset_gr = a.z_offset > b.z_offset;
    bool zOffset_eq = PGE_floatEqual(a.z_offset, b.z_offset, 8);

    bool arrayId_ge = a.meta.array_id >= b.meta.array_id;

    return sp_gr || (sp_eq && (zOffset_gr || (zOffset_eq && arrayId_ge)));
}

static inline bool smbx2BgoLess(const LevelBGO &a, const LevelBGO &b)
{
    bool sp_lt = (a.smbx64_sp_apply < b.smbx64_sp_apply);
    bool sp_eq = (a.smbx64_sp_apply == b.smbx64_sp_apply);
    bool zOffset_lt = a.z_offset < b.z_offset;
    bool zOffset_eq = PGE_floatEqual(a.z_offset, b.z_offset, 8);

    bool arrayId_le = a.meta.array_id <= b.meta.array_id;

    return sp_lt || (sp_eq && (zOffset_lt || (zOffset_eq && arrayId_le)));
}

void FileFormats::smbx2bLevelSortBGOs(LevelData &lvl)
{
    quickSort<PGELIST<LevelBGO>, LevelBGO>(lvl.bgo, smbx2BgoLess, sbx2BgoMore);
}

static inline bool arrayIdBgoMore(const LevelBGO &a, const LevelBGO &b)
{
    return a.meta.array_id >= b.meta.array_id;
}

static inline bool arrayIdBgoLess(const LevelBGO &a, const LevelBGO &b)
{
    return a.meta.array_id <= b.meta.array_id;
}

void FileFormats::arrayIdLevelSortBGOs(LevelData &lvl)
{
    quickSort<PGELIST<LevelBGO>, LevelBGO>(lvl.bgo, arrayIdBgoLess, arrayIdBgoMore);
}

int FileFormats::smbx64LevelCheckLimits(LevelData &lvl)
{
    int errorCode = SMBX64_FINE;
    //Sections limit
    if(lvl.sections.size() > 21) errorCode |= SMBX64EXC_SECTIONS;
    //Blocks limit
    if(lvl.blocks.size() > 16384) errorCode |= SMBX64EXC_BLOCKS;
    //BGO limits
    if(lvl.bgo.size() > 8000) errorCode |= SMBX64EXC_BGOS;
    //NPC limits
    if(lvl.npc.size() > 5000) errorCode |= SMBX64EXC_NPCS;
    //Warps limits
    if(lvl.doors.size() > 199) errorCode |= SMBX64EXC_WARPS;
    //Physical Environment zones
    if(lvl.physez.size() > 450) errorCode |= SMBX64EXC_WATERBOXES;
    //Layers limits
    if(lvl.layers.size() > 100) errorCode |= SMBX64EXC_LAYERS;
    //Events limits
    if(lvl.events.size() > 100) errorCode |= SMBX64EXC_EVENTS;

    return errorCode;
}
/*********************************************************************************/



//********************************************************************
//*******************Structure initializators*************************
//********************************************************************

//Default dataSets
LevelNPC    FileFormats::CreateLvlNpc()
{
    return LevelNPC();
}

LevelDoor  FileFormats::CreateLvlWarp()
{
    return LevelDoor();
}

LevelBlock  FileFormats::CreateLvlBlock()
{
    return LevelBlock();
}

LevelBGO FileFormats::CreateLvlBgo()
{
    return LevelBGO();
}

LevelPhysEnv FileFormats::CreateLvlPhysEnv()
{
    return LevelPhysEnv();
}

LevelSMBX64Event FileFormats::CreateLvlEvent()
{
    LevelSMBX64Event dummyEvent;

    dummyEvent.name = "";
    dummyEvent.msg = "";
    dummyEvent.sound_id = 0;
    dummyEvent.end_game = 0;
    dummyEvent.trigger = "";
    dummyEvent.trigger_timer = 0;
    dummyEvent.nosmoke = false;
    dummyEvent.ctrls_enable = false;
    dummyEvent.ctrl_altjump = false;
    dummyEvent.ctrl_altrun = false;
    dummyEvent.ctrl_down = false;
    dummyEvent.ctrl_drop = false;
    dummyEvent.ctrl_jump = false;
    dummyEvent.ctrl_left = false;
    dummyEvent.ctrl_right = false;
    dummyEvent.ctrl_run = false;
    dummyEvent.ctrl_start = false;
    dummyEvent.ctrl_up = false;
    dummyEvent.ctrl_lock_keyboard = false;
    dummyEvent.autostart = LevelSMBX64Event::AUTO_None;
    dummyEvent.autostart_condition = "";
    dummyEvent.movelayer = "";
    dummyEvent.layer_speed_x = 0.0;
    dummyEvent.layer_speed_y = 0.0;
    dummyEvent.move_camera_x = 0.0;
    dummyEvent.move_camera_y = 0.0;
    dummyEvent.scroll_section = 0;
    dummyEvent.trigger_api_id = 0;
    dummyEvent.layers_hide.clear();
    dummyEvent.layers_show.clear();
    dummyEvent.layers_toggle.clear();

    LevelEvent_Sets events_sets;
    dummyEvent.sets.clear();
    for(int j = 0; j < 21; j++)
    {
        events_sets.id = j;
        events_sets.music_id = -1;
        events_sets.background_id = -1;
        events_sets.position_left = -1;
        events_sets.position_top = 0;
        events_sets.position_bottom = 0;
        events_sets.position_right = 0;
        dummyEvent.sets.push_back(events_sets);
    }

    return dummyEvent;
}

LevelVariable FileFormats::CreateLvlVariable(PGESTRING vname)
{
    LevelVariable var;
    var.name = vname;
    var.value = "";
    var.is_global = false;
    return var;
}

LevelScript FileFormats::CreateLvlScript(PGESTRING name, int lang)
{
    LevelScript scr;
    scr.language = lang;
    scr.name = name;
    scr.script = "";
    return scr;
}

LevelSection FileFormats::CreateLvlSection()
{
    return LevelSection();
}

LevelLayer FileFormats::CreateLvlLayer()
{
    return LevelLayer();
}

PlayerPoint FileFormats::CreateLvlPlayerPoint(unsigned int id)
{
    PlayerPoint dummyPlayer;
    dummyPlayer.id = id;
    switch(id)
    {
    case 1:
        dummyPlayer.h = 54;
        break;
    case 2:
        dummyPlayer.h = 60;
        break;
    default:
        dummyPlayer.h = 32; //-V112
    }

    return dummyPlayer;
}

void FileFormats::LevelAddInternalEvents(LevelData &FileData)
{
    LevelLayer layers;
    LevelSMBX64Event events;

    layers = CreateLvlLayer();

    //Append system layers if not exist
    bool def = false, desb = false, spawned = false;

    for(pge_size_t lrID = 0; lrID < FileData.layers.size(); lrID++)
    {
        LevelLayer &lr = FileData.layers[lrID];
        if(lr.name == "Default") def = true;
        else if(lr.name == "Destroyed Blocks") desb = true;
        else if(lr.name == "Spawned NPCs") spawned = true;
    }

    if(!def)
    {
        layers.hidden = false;
        layers.name = "Default";
        FileData.layers.push_back(layers);
    }
    if(!desb)
    {
        layers.hidden = true;
        layers.name = "Destroyed Blocks";
        FileData.layers.push_back(layers);
    }
    if(!spawned)
    {
        layers.hidden = false;
        layers.name = "Spawned NPCs";
        FileData.layers.push_back(layers);
    }

    //Append system events if not exist
    //Level - Start
    //P Switch - Start
    //P Switch - End
    bool lstart = false, pstart = false, pend = false;
    for(pge_size_t evID = 0; evID < FileData.events.size(); evID++)
    {
        LevelSMBX64Event &ev = FileData.events[evID];
        if(ev.name == "Level - Start") lstart = true;
        else if(ev.name == "P Switch - Start") pstart = true;
        else if(ev.name == "P Switch - End") pend = true;
    }

    events = CreateLvlEvent();

    if(!lstart)
    {
        events.meta.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "Level - Start";
        FileData.events.push_back(events);
    }
    if(!pstart)
    {
        events.meta.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "P Switch - Start";
        FileData.events.push_back(events);
    }
    if(!pend)
    {
        events.meta.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "P Switch - End";
        FileData.events.push_back(events);
    }
}


void FileFormats::CreateLevelHeader(LevelData &NewFileData)
{
    NewFileData = LevelData();
}

void FileFormats::CreateLevelData(LevelData &NewFileData)
{
    CreateLevelHeader(NewFileData);

    NewFileData.bgo_array_id = 1;
    NewFileData.blocks_array_id = 1;
    NewFileData.doors_array_id = 1;
    NewFileData.events_array_id = 1;
    NewFileData.layers_array_id = 1;
    NewFileData.npc_array_id = 1;
    NewFileData.physenv_array_id = 1;

    NewFileData.sections.clear();
    //Create Section array
    LevelSection section;
    for(int i = 0; i < 21; i++)
    {
        section = CreateLvlSection();
        section.id = i;
        NewFileData.sections.push_back(section);
    }

    NewFileData.players.clear();
    //Create players array
    //PlayerPoint players = dummyLvlPlayerPoint();
    //    for(int i=0; i<2;i++)
    //    {
    //        players.id++;
    //        NewFileData.players.push_back(players);
    //    }

    NewFileData.blocks.clear();
    NewFileData.bgo.clear();
    NewFileData.npc.clear();
    NewFileData.doors.clear();
    NewFileData.physez.clear();

    NewFileData.layers.clear();
    NewFileData.variables.clear();
    NewFileData.arrays.clear();
    NewFileData.scripts.clear();

    NewFileData.custom38A_configs.clear();

    //Create system layers
    //Default
    //Destroyed Blocks
    //Spawned NPCs

    LevelLayer layers;
    layers.hidden = false;
    layers.locked = false;
    layers.name = "Default";
    layers.meta.array_id = NewFileData.layers_array_id++;
    NewFileData.layers.push_back(layers);

    layers.hidden = true;
    layers.locked = false;
    layers.name = "Destroyed Blocks";
    layers.meta.array_id = NewFileData.layers_array_id++;
    NewFileData.layers.push_back(layers);

    layers.hidden = false;
    layers.locked = false;
    layers.name = "Spawned NPCs";
    layers.meta.array_id = NewFileData.layers_array_id++;
    NewFileData.layers.push_back(layers);

    NewFileData.events.clear();
    //Create system events
    //Level - Start
    //P Switch - Start
    //P Switch - End

    LevelSMBX64Event events = CreateLvlEvent();

    events.meta.array_id = NewFileData.events_array_id;
    NewFileData.events_array_id++;

    events.name = "Level - Start";
    NewFileData.events.push_back(events);

    events.meta.array_id = NewFileData.events_array_id;
    NewFileData.events_array_id++;

    events.name = "P Switch - Start";
    NewFileData.events.push_back(events);

    events.meta.array_id = NewFileData.events_array_id;
    NewFileData.events_array_id++;

    events.name = "P Switch - End";
    NewFileData.events.push_back(events);
}

LevelData FileFormats::CreateLevelData()
{
    LevelData NewFileData;
    CreateLevelData(NewFileData);
    return NewFileData;
}



bool LevelData::eventIsExist(PGESTRING title)
{
    for(auto &e : events)
    {
        if(e.name == title)
            return true;
    }
    return false;
}

bool LevelData::layerIsExist(PGESTRING title)
{
    for(auto &l : layers)
    {
        if(l.name == title)
            return true;
    }
    return false;
}

bool LevelSMBX64Event::ctrlKeyPressed()
{
    return ctrl_up ||
           ctrl_down ||
           ctrl_left ||
           ctrl_right ||
           ctrl_jump ||
           ctrl_altjump ||
           ctrl_run ||
           ctrl_altrun ||
           ctrl_drop ||
           ctrl_start;
}
