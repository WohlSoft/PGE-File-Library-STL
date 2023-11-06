/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_file_lib_sys.h"
#include "file_formats.h"
#include "wld_filedata.h"
#include "file_strlist.h"
#include "smbx64.h"
#include "smbx64_macro.h"
#include "CSVUtils.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX64WldFileHeader(const PGESTRING &filePath, WorldData &FileData)
{
    FileData.meta.ERROR_info.clear();
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = LevelData::SMBX64;
    FileData.meta.RecentFormatVersion = 64;
    PGE_FileFormats_misc::TextFileInput inf;

    if(!inf.open(filePath, false))
    {
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ReadFileValid = false;
        return false;
    }
    return ReadSMBX64WldFileHeaderT(inf, FileData);
}

bool FileFormats::ReadSMBX64WldFileHeaderRaw(PGESTRING &rawdata, const PGESTRING &filePath, WorldData &FileData)
{
    FileData.meta.ERROR_info.clear();
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = LevelData::SMBX64;
    FileData.meta.RecentFormatVersion = 64;
    PGE_FileFormats_misc::RawTextInput inf;

    if(!inf.open(&rawdata, filePath))
    {
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ReadFileValid = false;
        return false;
    }
    return ReadSMBX64WldFileHeaderT(inf, FileData);
}

bool FileFormats::ReadSMBX64WldFileHeaderT(PGE_FileFormats_misc::TextInput &inf, WorldData &FileData)
{
    PGE_FileFormats_misc::FileInfo in_1(inf.getFilePath());
    FileData.meta.filename = in_1.basename();
    FileData.meta.path = in_1.dirpath();
    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    SMBX64_FileBegin();
#define nextLineH() line = inf.readCVSLine()
    FileData.meta.RecentFormat = WorldData::SMBX64;
    FileData.meta.RecentFormatVersion = 64;

    FileData.meta.untitled = false;
    FileData.meta.modified = false;

    //Enable strict mode for SMBX WLD file format
    FileData.meta.smbx64strict = true;
    FileData.nocharacter.clear();

    try
    {
        nextLineH();   //Read first Line
        SMBX64::ReadUInt(&file_format, line); //File format number
        FileData.meta.RecentFormatVersion = file_format;

        nextLineH();
        SMBX64::ReadStr(&FileData.EpisodeTitle, line); //Episode name

        if(ge(55))
        {
            nextLineH();
            SMBX64::ReadCSVBool(&FileData.nocharacter1, line);//Edisode without Mario
            nextLineH();
            SMBX64::ReadCSVBool(&FileData.nocharacter2, line);//Edisode without Luigi
            nextLineH();
            SMBX64::ReadCSVBool(&FileData.nocharacter3, line);//Edisode without Peach
            nextLineH();
            SMBX64::ReadCSVBool(&FileData.nocharacter4, line);//Edisode without Toad
            if(ge(56))
            {
                nextLineH();
                SMBX64::ReadCSVBool(&FileData.nocharacter5, line);//Edisode without Link
            }
            //Convert into the bool array
            FileData.nocharacter.push_back(FileData.nocharacter1);
            FileData.nocharacter.push_back(FileData.nocharacter2);
            FileData.nocharacter.push_back(FileData.nocharacter3);
            FileData.nocharacter.push_back(FileData.nocharacter4);
            FileData.nocharacter.push_back(FileData.nocharacter5);
        }

        if(ge(3))
        {
            nextLineH();
            SMBX64::ReadStr(&FileData.IntroLevel_file, line);//Autostart level
            nextLineH();
            SMBX64::ReadCSVBool(&FileData.HubStyledWorld, line); //Don't use world map on this episode
            nextLineH();
            SMBX64::ReadCSVBool(&FileData.restartlevel, line);//Restart level on playable character's death
        }

        if(ge(20))
        {
            nextLineH();
            SMBX64::ReadUInt(&FileData.stars, line);//Stars number
        }

        if(file_format >= 17)
        {
            nextLineH();
            SMBX64::ReadStr(&FileData.author1, line); //Author 1
            nextLineH();
            SMBX64::ReadStr(&FileData.author2, line); //Author 2
            nextLineH();
            SMBX64::ReadStr(&FileData.author3, line); //Author 3
            nextLineH();
            SMBX64::ReadStr(&FileData.author4, line); //Author 4
            nextLineH();
            SMBX64::ReadStr(&FileData.author5, line); //Author 5

            FileData.authors.clear();
            FileData.authors += (IsEmpty(FileData.author1)) ? "" : FileData.author1 + "\n";
            FileData.authors += (IsEmpty(FileData.author2)) ? "" : FileData.author2 + "\n";
            FileData.authors += (IsEmpty(FileData.author3)) ? "" : FileData.author3 + "\n";
            FileData.authors += (IsEmpty(FileData.author4)) ? "" : FileData.author4 + "\n";
            FileData.authors += (IsEmpty(FileData.author5)) ? "" : FileData.author5;
        }

        FileData.meta.ReadFileValid = true;
        return true;
    }
    catch(const std::exception &err)
    {
        if(file_format > 0)
            FileData.meta.ERROR_info = "Detected file format: SMBX-" + fromNum(file_format) + " is invalid\n";
        else
            FileData.meta.ERROR_info = "It is not an SMBX world map file\n";
#ifdef PGE_FILES_QT
        FileData.meta.ERROR_info += QString::fromStdString(exception_to_pretty_string(err));
#else
        FileData.meta.ERROR_info += exception_to_pretty_string(err);
#endif
        FileData.meta.ERROR_linenum = inf.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = std::move(line);
        FileData.meta.ReadFileValid = false;
        PGE_CutLength(FileData.meta.ERROR_linedata, 50);
        PGE_FilterBinary(FileData.meta.ERROR_linedata);
        return false;
    }
#undef nextLineH
}



bool FileFormats::ReadSMBX64WldFileF(const PGESTRING &filePath, WorldData &FileData)
{
    FileData.meta.ERROR_info.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath, false))
    {
        FileData.meta.ERROR_info = "Failed to open file for read";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }
    return ReadSMBX64WldFile(file, FileData);
}

bool FileFormats::ReadSMBX64WldFileRaw(PGESTRING &rawdata, const PGESTRING &filePath,  WorldData &FileData)
{
    PGE_FileFormats_misc::RawTextInput file;
    FileData.meta.ERROR_info.clear();
    if(!file.open(&rawdata, filePath))
    {
        FileData.meta.ERROR_info = "Failed to open raw string for read";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }
    return ReadSMBX64WldFile(file, FileData);
}

bool FileFormats::ReadSMBX64WldFile(PGE_FileFormats_misc::TextInput &in, WorldData &FileData)
{
    SMBX64_FileBegin();
    PGESTRING filePath = in.getFilePath();

    CreateWorldData(FileData);

    FileData.meta.RecentFormat = WorldData::SMBX64;
    FileData.meta.RecentFormatVersion = 64;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    FileData.meta.untitled = false;
    FileData.meta.modified = false;

    //Enable strict mode for SMBX WLD file format
    FileData.meta.smbx64strict = true;

    WorldTerrainTile tile;
    WorldScenery scen;
    WorldPathTile pathitem;
    WorldLevelTile lvlitem;
    WorldMusicBox musicbox;

    try
    {
        ///////////////////////////////////////Begin file///////////////////////////////////////
        //File format number
        nextLine();
        SMBX64::ReadUInt(&file_format, line);
        FileData.meta.RecentFormatVersion = file_format;

        //Episode title
        nextLine();
        SMBX64::ReadStr(&FileData.EpisodeTitle, line);

        if(ge(55))
        {
            nextLine();
            SMBX64::ReadCSVBool(&FileData.nocharacter1, line);//Edisode without Mario
            nextLine();
            SMBX64::ReadCSVBool(&FileData.nocharacter2, line);//Edisode without Luigi
            nextLine();
            SMBX64::ReadCSVBool(&FileData.nocharacter3, line);//Edisode without Peach
            nextLine();
            SMBX64::ReadCSVBool(&FileData.nocharacter4, line);//Edisode without Toad
            if(ge(56))
            {
                nextLine();
                SMBX64::ReadCSVBool(&FileData.nocharacter5, line);//Edisode without Link
            }
            //Convert into the bool array
            FileData.nocharacter.push_back(FileData.nocharacter1);
            FileData.nocharacter.push_back(FileData.nocharacter2);
            FileData.nocharacter.push_back(FileData.nocharacter3);
            FileData.nocharacter.push_back(FileData.nocharacter4);
            FileData.nocharacter.push_back(FileData.nocharacter5);
        }

        if(ge(3))
        {
            nextLine();
            SMBX64::ReadStr(&FileData.IntroLevel_file, line);//Autostart level
            nextLine();
            SMBX64::ReadCSVBool(&FileData.HubStyledWorld, line); //Don't use world map on this episode
            nextLine();
            SMBX64::ReadCSVBool(&FileData.restartlevel, line);//Restart level on playable character's death
        }

        if(ge(20))
        {
            nextLine();
            SMBX64::ReadUInt(&FileData.stars, line);//Stars number
        }

        if(file_format >= 17)
        {
            nextLine();
            SMBX64::ReadStr(&FileData.author1, line); //Author 1
            nextLine();
            SMBX64::ReadStr(&FileData.author2, line); //Author 2
            nextLine();
            SMBX64::ReadStr(&FileData.author3, line); //Author 3
            nextLine();
            SMBX64::ReadStr(&FileData.author4, line); //Author 4
            nextLine();
            SMBX64::ReadStr(&FileData.author5, line); //Author 5

            FileData.authors.clear();
            FileData.authors += (IsEmpty(FileData.author1)) ? "" : FileData.author1 + "\n";
            FileData.authors += (IsEmpty(FileData.author2)) ? "" : FileData.author2 + "\n";
            FileData.authors += (IsEmpty(FileData.author3)) ? "" : FileData.author3 + "\n";
            FileData.authors += (IsEmpty(FileData.author4)) ? "" : FileData.author4 + "\n";
            FileData.authors += (IsEmpty(FileData.author5)) ? "" : FileData.author5;
        }


        ////////////Tiles Data//////////
        nextLine();
        while((line != "next") && (!in.eof()))
        {
            tile = CreateWldTile();
            SMBX64::ReadSIntFromFloat(&tile.x, line);//Tile x
            nextLine();
            SMBX64::ReadSIntFromFloat(&tile.y, line);//Tile y
            nextLine();
            SMBX64::ReadUInt(&tile.id, line);//Tile ID

            tile.meta.array_id = FileData.tile_array_id;
            FileData.tile_array_id++;
            tile.meta.index = (unsigned int)FileData.tiles.size(); //Apply element index

            FileData.tiles.push_back(tile);
            nextLine();
        }

        ////////////Scenery Data//////////
        nextLine();
        while((line != "next")  && (!in.eof()))
        {
            scen = CreateWldScenery();
            SMBX64::ReadSIntFromFloat(&scen.x, line);//Scenery x
            nextLine();
            SMBX64::ReadSIntFromFloat(&scen.y, line);//Scenery y
            nextLine();
            SMBX64::ReadUInt(&scen.id, line);//Scenery ID

            scen.meta.array_id = FileData.scene_array_id;
            FileData.scene_array_id++;
            scen.meta.index = (unsigned int)FileData.scenery.size(); //Apply element index

            FileData.scenery.push_back(scen);

            nextLine();
        }

        ////////////Paths Data//////////
        nextLine();
        while((line != "next") && (!in.eof()))
        {
            pathitem = CreateWldPath();
            SMBX64::ReadSIntFromFloat(&pathitem.x, line);//Path x
            nextLine();
            SMBX64::ReadSIntFromFloat(&pathitem.y, line);//Path y
            nextLine();
            SMBX64::ReadUInt(&pathitem.id, line); //Path ID

            pathitem.meta.array_id = FileData.path_array_id;
            FileData.path_array_id++;
            pathitem.meta.index = (unsigned int)FileData.paths.size(); //Apply element index

            FileData.paths.push_back(pathitem);

            nextLine();
        }

        ////////////LevelBox Data//////////
        nextLine();
        while((line != "next")  && (!in.eof()))
        {
            lvlitem = CreateWldLevel();

            SMBX64::ReadSIntFromFloat(&lvlitem.x, line);//Level x
            nextLine();
            SMBX64::ReadSIntFromFloat(&lvlitem.y, line);//Level y
            nextLine();
            SMBX64::ReadUInt(&lvlitem.id, line);//Level ID
            nextLine();
            SMBX64::ReadStr(&lvlitem.lvlfile, line);//Level file
            nextLine();
            SMBX64::ReadStr(&lvlitem.title, line);//Level title
            nextLine();
            SMBX64::ReadSInt(&lvlitem.top_exit, line);//Top exit
            nextLine();
            SMBX64::ReadSInt(&lvlitem.left_exit, line);//Left exit
            nextLine();
            SMBX64::ReadSInt(&lvlitem.bottom_exit, line);//bottom exit
            nextLine();
            SMBX64::ReadSInt(&lvlitem.right_exit, line);//right exit
            if(ge(4))
            {
                nextLine();    //Enter via Level's warp
                SMBX64::ReadUInt(&lvlitem.entertowarp, line);
            }

            if(ge(22))
            {
                nextLine();
                SMBX64::ReadCSVBool(&lvlitem.alwaysVisible, line);//Always Visible
                nextLine();
                SMBX64::ReadCSVBool(&lvlitem.pathbg, line);//Path background
                nextLine();
                SMBX64::ReadCSVBool(&lvlitem.gamestart, line);//Game start point
                nextLine();
                SMBX64::ReadSInt(&lvlitem.gotox, line);//Goto x on World map
                nextLine();
                SMBX64::ReadSInt(&lvlitem.gotoy, line);//Goto y on World map
                nextLine();
                SMBX64::ReadCSVBool(&lvlitem.bigpathbg, line);//Big Path background
            }
            else
            {
                if(lvlitem.id == 1)
                    lvlitem.gamestart = true;
            }

            lvlitem.meta.array_id = FileData.level_array_id;
            FileData.level_array_id++;
            lvlitem.meta.index = (unsigned int)FileData.levels.size(); //Apply element index

            FileData.levels.push_back(lvlitem);

            nextLine();
        }

        ////////////MusicBox Data//////////
        nextLine();
        while((line != "next") && (!IsEmpty(line)) && (!in.eof()))
        {
            musicbox = CreateWldMusicbox();
            SMBX64::ReadSIntFromFloat(&musicbox.x, line);//MusicBox x
            nextLine();
            SMBX64::ReadSIntFromFloat(&musicbox.y, line);//MusicBox y
            nextLine();
            SMBX64::ReadUInt(&musicbox.id, line);//MusicBox ID

            musicbox.meta.array_id = FileData.musicbox_array_id;
            FileData.musicbox_array_id++;
            musicbox.meta.index = (unsigned int)FileData.music.size(); //Apply element index

            FileData.music.push_back(musicbox);

            nextLine();
        }
        nextLine(); // Read last line
        ///////////////////////////////////////EndFile///////////////////////////////////////
        FileData.meta.ReadFileValid = true;
        return true;
    }
    catch(const std::exception &err)
    {
        if(file_format > 0)
            FileData.meta.ERROR_info = "Detected file format: SMBX-" + fromNum(file_format) + " is invalid\n";
        else
            FileData.meta.ERROR_info = "It is not an SMBX world map file\n";
#ifdef PGE_FILES_QT
        FileData.meta.ERROR_info += QString::fromStdString(exception_to_pretty_string(err));
#else
        FileData.meta.ERROR_info += exception_to_pretty_string(err);
#endif
        FileData.meta.ERROR_linenum  = in.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = std::move(line);
        FileData.meta.ReadFileValid  = false;
        PGE_CutLength(FileData.meta.ERROR_linedata, 50);
        PGE_FilterBinary(FileData.meta.ERROR_linedata);
        return false;
    }
}


//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteSMBX64WldFileF(const PGESTRING &filePath, WorldData &FileData, unsigned int file_format)
{
    PGE_FileFormats_misc::TextFileOutput file;
    FileData.meta.ERROR_info.clear();
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open file for write";
        return false;
    }
    return WriteSMBX64WldFile(file, FileData, file_format);
}

bool FileFormats::WriteSMBX64WldFileRaw(WorldData &FileData, PGESTRING &rawdata, unsigned int file_format)
{
    PGE_FileFormats_misc::RawTextOutput file;
    FileData.meta.ERROR_info.clear();
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open raw string for write";
        return false;
    }
    return WriteSMBX64WldFile(file, FileData, file_format);
}

bool FileFormats::WriteSMBX64WldFile(PGE_FileFormats_misc::TextOutput &out, WorldData &FileData, unsigned int file_format)
{
    //Prevent out of range: 0....64
    if(file_format > 64)
        file_format = 64;

    FileData.meta.RecentFormat = WorldData::SMBX64;
    FileData.meta.RecentFormatVersion = file_format;

    out << SMBX64::WriteSInt(file_format);              //Format version
    out << SMBX64::WriteStr(FileData.EpisodeTitle);   //Episode title

    FileData.nocharacter1 = (FileData.nocharacter.size() > 0) ? FileData.nocharacter[0] : false;
    FileData.nocharacter2 = (FileData.nocharacter.size() > 1) ? FileData.nocharacter[1] : false;
    FileData.nocharacter3 = (FileData.nocharacter.size() > 2) ? FileData.nocharacter[2] : false;
    FileData.nocharacter4 = (FileData.nocharacter.size() > 3) ? FileData.nocharacter[3] : false;
    FileData.nocharacter5 = (FileData.nocharacter.size() > 4) ? FileData.nocharacter[4] : false;

    if(file_format >= 55)
    {
        out << SMBX64::WriteCSVBool(FileData.nocharacter1);
        out << SMBX64::WriteCSVBool(FileData.nocharacter2);
        out << SMBX64::WriteCSVBool(FileData.nocharacter3);
        out << SMBX64::WriteCSVBool(FileData.nocharacter4);
        if(file_format >= 56)
            out << SMBX64::WriteCSVBool(FileData.nocharacter5);
    }
    if(file_format >= 3)
    {
        out << SMBX64::WriteStr(FileData.IntroLevel_file);
        out << SMBX64::WriteCSVBool(FileData.HubStyledWorld);
        out << SMBX64::WriteCSVBool(FileData.restartlevel);
    }
    if(file_format >= 20)
        out << SMBX64::WriteSInt(FileData.stars);

    PGESTRINGList credits;
    PGE_SPLITSTRING(credits, FileData.authors, "\n");
    pge_size_t credits_size = credits.size();
    FileData.author1 = (credits_size > 0) ? credits[0] : PGESTRING();
    FileData.author2 = (credits_size > 1) ? credits[1] : PGESTRING();
    FileData.author3 = (credits_size > 2) ? credits[2] : PGESTRING();
    FileData.author4 = (credits_size > 3) ? credits[3] : PGESTRING();
    FileData.author5 = (credits_size > 4) ? credits[4] : PGESTRING();

    if(file_format >= 17)
    {
        out << SMBX64::WriteStr(FileData.author1);
        out << SMBX64::WriteStr(FileData.author2);
        out << SMBX64::WriteStr(FileData.author3);
        out << SMBX64::WriteStr(FileData.author4);
        out << SMBX64::WriteStr(FileData.author5);
    }

    for(const auto &t : FileData.tiles)
    {
        out << SMBX64::WriteSInt(t.x);
        out << SMBX64::WriteSInt(t.y);
        out << SMBX64::WriteSInt(t.id);
    }
    out << "\"next\"\n";//Separator

    for(const auto &s : FileData.scenery)
    {
        out << SMBX64::WriteSInt(s.x);
        out << SMBX64::WriteSInt(s.y);
        out << SMBX64::WriteSInt(s.id);
    }
    out << "\"next\"\n";//Separator

    for(const auto &p : FileData.paths)
    {
        out << SMBX64::WriteSInt(p.x);
        out << SMBX64::WriteSInt(p.y);
        out << SMBX64::WriteSInt(p.id);
    }
    out << "\"next\"\n";//Separator

    for(const auto &lvl : FileData.levels)
    {
        out << SMBX64::WriteSInt(lvl.x);
        out << SMBX64::WriteSInt(lvl.y);
        out << SMBX64::WriteSInt(lvl.id);
        out << SMBX64::WriteStr(lvl.lvlfile);
        out << SMBX64::WriteStr(lvl.title);
        out << SMBX64::WriteSInt(lvl.top_exit);
        out << SMBX64::WriteSInt(lvl.left_exit);
        out << SMBX64::WriteSInt(lvl.bottom_exit);
        out << SMBX64::WriteSInt(lvl.right_exit);
        if(file_format >= 4)
            out << SMBX64::WriteSInt(lvl.entertowarp);
        if(file_format >= 22)
        {
            out << SMBX64::WriteCSVBool(lvl.alwaysVisible);
            out << SMBX64::WriteCSVBool(lvl.pathbg);
            out << SMBX64::WriteCSVBool(lvl.gamestart);
            out << SMBX64::WriteSInt(lvl.gotox);
            out << SMBX64::WriteSInt(lvl.gotoy);
            out << SMBX64::WriteCSVBool(lvl.bigpathbg);
        }
    }
    out << "\"next\"\n";//Separator

    for(const auto &mus : FileData.music)
    {
        out << SMBX64::WriteSInt(mus.x);
        out << SMBX64::WriteSInt(mus.y);
        out << SMBX64::WriteSInt(mus.id);
    }
    out << "\"next\"\n";//Separator

    return true;
}
