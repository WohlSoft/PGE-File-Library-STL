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
#include "wld_filedata.h"
#include "pge_file_lib_private.h"

#include <stack>

int FileFormats::smbx64WorldCheckLimits(WorldData &wld)
{
    int errorCode = 0;

    //Tiles limit
    if(wld.tiles.size() > 20000) errorCode |= SMBX64EXC_TILES;
    //Sceneries limit
    if(wld.scenery.size() > 5000) errorCode |= SMBX64EXC_SCENERIES;
    //Paths limit
    if(wld.paths.size() > 2000) errorCode |= SMBX64EXC_PATHS;
    //Levels limit
    if(wld.levels.size() > 400) errorCode |= SMBX64EXC_LEVELS;
    //Music boxes limit
    if(wld.music.size() > 1000) errorCode |= SMBX64EXC_MUSICBOXES;

    return errorCode;
}


WorldTerrainTile FileFormats::CreateWldTile()
{
    return WorldTerrainTile();
}

WorldScenery FileFormats::CreateWldScenery()
{
    return WorldScenery();
}

WorldPathTile FileFormats::CreateWldPath()
{
    return WorldPathTile();
}

WorldLevelTile FileFormats::CreateWldLevel()
{
    return WorldLevelTile();
}

WorldMusicBox FileFormats::CreateWldMusicbox()
{
    return WorldMusicBox();
}

void FileFormats::CreateWorldHeader(WorldData &NewFileData)
{
    NewFileData = WorldData();
}


void FileFormats::CreateWorldData(WorldData &NewFileData)
{
    CreateWorldHeader(NewFileData);

    NewFileData.tiles.clear();
    NewFileData.tile_array_id = 0;
    NewFileData.scenery.clear();
    NewFileData.scene_array_id = 0;
    NewFileData.paths.clear();
    NewFileData.path_array_id = 0;
    NewFileData.levels.clear();
    NewFileData.level_array_id = 0;
    NewFileData.music.clear();
    NewFileData.musicbox_array_id = 0;
    NewFileData.arearects.clear();
    NewFileData.arearect_array_id = 0;
    NewFileData.layers.clear();
    NewFileData.layers_array_id = 0;
}

WorldData FileFormats::CreateWorldData()
{
    WorldData NewFileData;
    CreateWorldData(NewFileData);
    return NewFileData;
}


template<class T, class U>
void quickSortByArrayId(T &array)
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
    end.push(static_cast<int>(array.size()));
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
                while(
                      (array[ST(R)].meta.array_id >= piv.meta.array_id) &&
                      (L < R)) R--;
                if(L < R) array[ST(L++)] = array[ST(R)];

                while(
                    (
                        (array[ST(L)].meta.array_id <= piv.meta.array_id)
                    ) && (L < R)) L++;
                if(L < R) array[ST(R--)] = array[ST(L)];
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


void FileFormats::WorldPrepare(WorldData &wld)
{
    quickSortByArrayId<PGELIST<WorldTerrainTile>, WorldTerrainTile>(wld.tiles);
    quickSortByArrayId<PGELIST<WorldScenery>, WorldScenery>(wld.scenery);
    quickSortByArrayId<PGELIST<WorldPathTile>, WorldPathTile>(wld.paths);
    quickSortByArrayId<PGELIST<WorldLevelTile>, WorldLevelTile>(wld.levels);
    quickSortByArrayId<PGELIST<WorldMusicBox>, WorldMusicBox>(wld.music);
}
