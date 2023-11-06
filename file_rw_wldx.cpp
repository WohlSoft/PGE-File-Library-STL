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

#include "file_formats.h"
#include "file_strlist.h"
#include "wld_filedata.h"
#include "pge_x.h"
#include "pge_x_macro.h"
#include "pge_file_lib_sys.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

//WorldData FileFormats::ReadExtendedWorldFile(PGEFILE &inf)
//{
//    QTextStream in(&inf);   //Read File
//    in.setCodec("UTF-8");

//    return ReadExtendedWldFile( in.readAll(), inf.fileName() );
//}

bool FileFormats::ReadExtendedWldFileHeader(const PGESTRING &filePath, WorldData &FileData)
{
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = LevelData::PGEX;
    PGE_FileFormats_misc::TextFileInput inf;

    if(!inf.open(filePath, true))
    {
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadExtendedWldFileHeaderT(inf, FileData);
}

bool FileFormats::ReadExtendedWldFileHeaderRaw(PGESTRING &rawdata, const PGESTRING &filePath, WorldData &FileData)
{
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = LevelData::PGEX;
    PGE_FileFormats_misc::RawTextInput inf;

    if(!inf.open(&rawdata, filePath))
    {
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadExtendedWldFileHeaderT(inf, FileData);
}

bool FileFormats::ReadExtendedWldFileHeaderT(PGE_FileFormats_misc::TextInput &inf, WorldData &FileData)
{
    PGESTRING line;
    int str_count = 0;
    bool valid = false;
    PGE_FileFormats_misc::FileInfo in_1(inf.getFilePath());
    FileData.meta.filename = in_1.basename();
    FileData.meta.path = in_1.dirpath();
    FileData.meta.RecentFormat = LevelData::PGEX;

    FileData.nocharacter.clear();

    //Find level header part
    do
    {
        str_count++;
        line = inf.readLine();
    }
    while((line != "HEAD") && (!inf.eof()));

    PGESTRINGList header;
    bool closed = false;

    if(line != "HEAD")//Header not found, this world map is head-less
        goto skipHeaderParse;

    str_count++;
    line = inf.readLine();

    while((line != "HEAD_END") && (!inf.eof()))
    {
        header.push_back(line);
        str_count++;
        line = inf.readLine();

        if(line == "HEAD_END")
            closed = true;
    }

    if(!closed)
        goto badfile;

    for(pge_size_t zzz = 0; zzz < header.size(); zzz++)
    {
        PGESTRING &header_line = header[zzz];
        PGELIST<PGESTRINGList >data = PGEFile::splitDataLine(header_line, &valid);

        for(pge_size_t i = 0; i < data.size(); i++)
        {
            if(data[i].size() != 2) goto badfile;

            if(data[i][0] == "TL") //Episode Title
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.EpisodeTitle = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "DC") //Disabled characters
            {
                if(PGEFile::IsBoolArray(data[i][1]))
                    FileData.nocharacter = PGEFile::X2BollArr(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "IT") //Intro level
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.IntroLevel_file = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "GO") //Game Over level
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.GameOverLevel_file = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "HB") //Hub Styled
            {
                if(PGEFile::IsBool(data[i][1]))
                    FileData.HubStyledWorld = static_cast<bool>(toInt(data[i][1]) != 0);
                else
                    goto badfile;
            }
            else if(data[i][0] == "RL") //Restart level on fail
            {
                if(PGEFile::IsBool(data[i][1]))
                    FileData.restartlevel = static_cast<bool>(toInt(data[i][1]) != 0);
                else
                    goto badfile;
            }
            else if(data[i][0] == "SZ") //Starz number
            {
                if(PGEFile::IsIntU(data[i][1]))
                    FileData.stars = toUInt(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "CD") //Credits list
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.authors = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "CM") //Credits scene background music
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.authors_music = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "SSS") //Per-level stars count showing policy
            {
                if(PGEFile::IsIntS(data[i][1]))
                    FileData.starsShowPolicy = toInt(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "XTRA") //Extra settings
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.custom_params = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "CPID") //Config pack ID string
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.meta.configPackId = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
        }
    }

skipHeaderParse:
    FileData.CurSection = 0;
    FileData.playmusic = 0;
    FileData.meta.ReadFileValid = true;
    return true;
badfile:
    FileData.meta.ERROR_info = "Invalid file format";
    FileData.meta.ERROR_linenum = str_count;
    FileData.meta.ERROR_linedata = line;
    FileData.meta.ReadFileValid = false;
    PGE_CutLength(FileData.meta.ERROR_linedata, 50);
    PGE_FilterBinary(FileData.meta.ERROR_linedata);
    return false;
}

bool FileFormats::ReadExtendedWldFileF(const PGESTRING &filePath, WorldData &FileData)
{
    PGE_FileFormats_misc::TextFileInput file;
    FileData.meta.ERROR_info.clear();

    if(!file.open(filePath, true))
    {
        FileData.meta.ERROR_info = "Failed to open file for read";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadExtendedWldFile(file, FileData);
}

bool FileFormats::ReadExtendedWldFileRaw(PGESTRING &rawdata, const PGESTRING &filePath,  WorldData &FileData)
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

    return ReadExtendedWldFile(file, FileData);
}

bool FileFormats::ReadExtendedWldFile(PGE_FileFormats_misc::TextInput &in, WorldData &FileData)
{
    PGESTRING errorString;
    PGEX_FileBegin();
    PGESTRING filePath = in.getFilePath();
    CreateWorldData(FileData);
    FileData.meta.RecentFormat = WorldData::PGEX;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    FileData.meta.untitled = false;
    FileData.meta.modified = false;
    WorldTerrainTile tile;
    WorldScenery scen;
    WorldPathTile pathitem;
    WorldMusicBox musicbox;
    WorldAreaRect arearect;
    WorldLevelTile lvlitem;
    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEX_FileParseTree(in.readAll());
    PGEX_FetchSection() //look sections
    {
        PGEX_FetchSection_begin()
        ///////////////////HEADER//////////////////////
        PGEX_Section("HEAD")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count += 8;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("TL", FileData.EpisodeTitle)        //Episode Title
                    PGEX_BoolArrVal("DC", FileData.nocharacter)     //Disabled characters
                    PGEX_StrVal("IT", FileData.IntroLevel_file)     //Intro level
                    PGEX_StrVal("GO", FileData.GameOverLevel_file)  //Game Over level
                    PGEX_BoolVal("HB", FileData.HubStyledWorld)     //Hub Styled
                    PGEX_BoolVal("RL", FileData.restartlevel)       //Restart level on fail
                    PGEX_UIntVal("SZ", FileData.stars)              //Starz number
                    PGEX_StrVal("CD", FileData.authors)     //Credits list
                    PGEX_StrVal("CM", FileData.authors_music)     //Credits scene background music
                    PGEX_SIntVal("SSS", FileData.starsShowPolicy) //Per-level stars count showing policy
                    PGEX_StrVal("XTRA", FileData.custom_params)     //World-wide Extra settings
                    PGEX_StrVal("CPID", FileData.meta.configPackId)//Config pack ID string
                }
            }
        }//head
        ///////////////////////////////MetaDATA/////////////////////////////////////////////
        PGEX_Section("META_BOOKMARKS")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                Bookmark meta_bookmark;
                meta_bookmark.bookmarkName.clear();
                meta_bookmark.x = 0;
                meta_bookmark.y = 0;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("BM", meta_bookmark.bookmarkName) //Bookmark name
                    PGEX_SIntVal("X", meta_bookmark.x) // Position X
                    PGEX_SIntVal("Y", meta_bookmark.y) // Position Y
                }
                FileData.metaData.bookmarks.push_back(meta_bookmark);
            }
        }
        ////////////////////////meta bookmarks////////////////////////
        PGEX_Section("META_SYS_CRASH")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                PGEX_Values() //Look markers and values
                {
                    FileData.metaData.crash.used = true;
                    PGEX_ValueBegin()
                    PGEX_BoolVal("UT", FileData.metaData.crash.untitled) //Untitled
                    PGEX_BoolVal("MD", FileData.metaData.crash.modifyed) //Modyfied
                    PGEX_SIntVal("FF", FileData.metaData.crash.fmtID) //Recent File format
                    PGEX_UIntVal("FV", FileData.metaData.crash.fmtVer) //Recent File format version
                    PGEX_StrVal("N",  FileData.metaData.crash.filename)  //Filename
                    PGEX_StrVal("P",  FileData.metaData.crash.path)  //Path
                    PGEX_StrVal("FP", FileData.metaData.crash.fullPath)  //Full file Path
                }
            }
        }//meta sys crash
        ///////////////////////////////MetaDATA//End////////////////////////////////////////
        ///////////////////TILES//////////////////////
        PGEX_Section("TILES")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                tile = CreateWldTile();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", tile.id) //Tile ID
                    PGEX_SIntVal("X",  tile.x) //X Position
                    PGEX_SIntVal("Y",  tile.y) //Y Position
                    PGEX_StrVal("XTRA", tile.meta.custom_params)//Custom JSON data tree
                }
                tile.meta.array_id = FileData.tile_array_id++;
                tile.meta.index = static_cast<unsigned int>(FileData.tiles.size());
                FileData.tiles.push_back(tile);
            }
        }//TILES
        ///////////////////SCENERY//////////////////////
        PGEX_Section("SCENERY")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                scen = CreateWldScenery();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", scen.id)  //Scenery ID
                    PGEX_SIntVal("X", scen.x) //X Position
                    PGEX_SIntVal("Y", scen.y) //Y Position
                    PGEX_StrVal("XTRA", scen.meta.custom_params)//Custom JSON data tree
                }
                scen.meta.array_id = FileData.scene_array_id++;
                scen.meta.index = static_cast<unsigned int>(FileData.scenery.size());
                FileData.scenery.push_back(scen);
            }
        }//SCENERY
        ///////////////////PATHS//////////////////////
        PGEX_Section("PATHS")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                pathitem = CreateWldPath();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", pathitem.id)  //Path ID
                    PGEX_SIntVal("X", pathitem.x) //X Position
                    PGEX_SIntVal("Y", pathitem.y) //Y Position
                    PGEX_StrVal("XTRA", pathitem.meta.custom_params)//Custom JSON data tree
                }
                pathitem.meta.array_id = FileData.path_array_id++;
                pathitem.meta.index =  static_cast<unsigned int>(FileData.paths.size());
                FileData.paths.push_back(pathitem);
            }
        }//PATHS
        ///////////////////MUSICBOXES//////////////////////
        PGEX_Section("MUSICBOXES")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                musicbox = CreateWldMusicbox();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", musicbox.id) //MISICBOX ID
                    PGEX_SIntVal("X", musicbox.x) //X Position
                    PGEX_SIntVal("Y", musicbox.y) //X Position
                    PGEX_StrVal("MF", musicbox.music_file)  //Custom music file
                    PGEX_StrVal("XTRA", musicbox.meta.custom_params)//Custom JSON data tree
                }
                musicbox.meta.array_id = FileData.musicbox_array_id++;
                musicbox.meta.index =  static_cast<unsigned int>(FileData.music.size());
                FileData.music.push_back(musicbox);
            }
        }//MUSICBOXES
        ///////////////////AREARECTS//////////////////////
        PGEX_Section("AREARECTS")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                arearect = WorldAreaRect();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_USIntVal("F", arearect.flags)  //Flags
                    PGEX_SIntVal("X", arearect.x) //X Position
                    PGEX_SIntVal("Y", arearect.y) //X Position
                    PGEX_USIntVal("W", arearect.w) //Width
                    PGEX_USIntVal("H", arearect.h) //Height

                    // unused stuff
                    PGEX_UIntVal("MI", arearect.music_id) //MUSICBOX ID
                    PGEX_StrVal("MF", arearect.music_file)  //Custom music file
                    PGEX_StrVal("LR", arearect.layer)
                    PGEX_StrVal("EB", arearect.eventBreak)
                    PGEX_StrVal("EW", arearect.eventWarp)
                    PGEX_StrVal("EA", arearect.eventAnchor)
                    PGEX_StrVal("ET", arearect.eventTouch)
                    PGEX_SIntVal("TP", arearect.eventTouchPolicy)
                    PGEX_StrVal("XTRA", arearect.meta.custom_params)//Custom JSON data tree
                }
                arearect.meta.array_id = FileData.arearect_array_id++;
                arearect.meta.index =  static_cast<unsigned int>(FileData.arearects.size());
                FileData.arearects.push_back(arearect);
            }
        }//AREARECTS
        ///////////////////LEVELS//////////////////////
        PGEX_Section("LEVELS")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                lvlitem = CreateWldLevel();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", lvlitem.id) //LEVEL IMAGE ID
                    PGEX_SIntVal("X",  lvlitem.x) //X Position
                    PGEX_SIntVal("Y",  lvlitem.y) //X Position
                    PGEX_StrVal("LF", lvlitem.lvlfile)  //Target level file
                    PGEX_StrVal("LT", lvlitem.title)   //Level title
                    PGEX_UIntVal("EI", lvlitem.entertowarp) //Entrance Warp ID (if 0 - start level from default points)
                    PGEX_SIntVal("ET", lvlitem.top_exit) //Open top path on exit type
                    PGEX_SIntVal("EL", lvlitem.left_exit) //Open left path on exit type
                    PGEX_SIntVal("ER", lvlitem.right_exit) //Open right path on exit type
                    PGEX_SIntVal("EB", lvlitem.bottom_exit) //Open bottom path on exit type
                    PGEX_SIntVal("WX", lvlitem.gotox) //Goto world map X
                    PGEX_SIntVal("WY", lvlitem.gotoy) //Goto world map Y
                    PGEX_BoolVal("AV", lvlitem.alwaysVisible) //Always visible
                    PGEX_BoolVal("SP", lvlitem.gamestart) //Is Game start point
                    PGEX_BoolVal("BP", lvlitem.pathbg) //Path background
                    PGEX_BoolVal("BG", lvlitem.bigpathbg) //Big path background
                    PGEX_SIntVal("SSS", lvlitem.starsShowPolicy) // Stars count showing policy
                    PGEX_StrVal("XTRA", lvlitem.meta.custom_params)//Custom JSON data tree
                }
                lvlitem.meta.array_id = FileData.level_array_id++;
                lvlitem.meta.index = static_cast<unsigned int>(FileData.levels.size());
                FileData.levels.push_back(lvlitem);
            }
        }//LEVELS
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////
    FileData.meta.ERROR_info.clear(); //If no errors, clear string;
    FileData.meta.ReadFileValid = true;
    return true;
badfile:    //If file format not corrects
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linenum = str_count;
    FileData.meta.ERROR_linedata = line;
    FileData.meta.ReadFileValid = false;
    PGE_CutLength(FileData.meta.ERROR_linedata, 50);
    PGE_FilterBinary(FileData.meta.ERROR_linedata);
    return false;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteExtendedWldFileF(const PGESTRING &filePath, WorldData &FileData)
{
    PGE_FileFormats_misc::TextFileOutput file;
    FileData.meta.ERROR_info.clear();

    if(!file.open(filePath, true, false, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open file for write";
        return false;
    }

    return WriteExtendedWldFile(file, FileData);
}

bool FileFormats::WriteExtendedWldFileRaw(WorldData &FileData, PGESTRING &rawdata)
{
    PGE_FileFormats_misc::RawTextOutput file;
    FileData.meta.ERROR_info.clear();

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open raw string for write";
        return false;
    }

    return WriteExtendedWldFile(file, FileData);
}

bool FileFormats::WriteExtendedWldFile(PGE_FileFormats_misc::TextOutput &out, WorldData &FileData)
{
    pge_size_t i = 0;
    FileData.meta.RecentFormat = WorldData::PGEX;

    //HEAD section
    {
        PGESTRING outHeader;

        if(!IsEmpty(FileData.EpisodeTitle))
            outHeader += PGEFile::value("TL", PGEFile::WriteStr(FileData.EpisodeTitle)); // Episode title

        {
            bool needToAdd = false;
            for(pge_size_t z = 0; z < FileData.nocharacter.size(); z++)
            {
                bool x = FileData.nocharacter[z];
                if(x) needToAdd = true;
            }

            if(needToAdd)
                outHeader += PGEFile::value("DC", PGEFile::WriteBoolArr(FileData.nocharacter)); // Disabled characters
        }

        if(!IsEmpty(FileData.IntroLevel_file))
            outHeader += PGEFile::value("IT", PGEFile::WriteStr(FileData.IntroLevel_file)); // Intro level
        if(!IsEmpty(FileData.GameOverLevel_file))
            outHeader += PGEFile::value("GO", PGEFile::WriteStr(FileData.GameOverLevel_file)); // Game Over level
        if(FileData.HubStyledWorld)
            outHeader += PGEFile::value("HB", PGEFile::WriteBool(FileData.HubStyledWorld)); // Hub-styled episode
        if(FileData.restartlevel)
            outHeader += PGEFile::value("RL", PGEFile::WriteBool(FileData.restartlevel)); // Restart on fail
        if(FileData.stars > 0)
            outHeader += PGEFile::value("SZ", PGEFile::WriteInt(FileData.stars));      // Total stars number
        if(!IsEmpty(FileData.authors))
            outHeader += PGEFile::value("CD", PGEFile::WriteStr(FileData.authors));   // Credits
        if(!IsEmpty(FileData.authors_music))
            outHeader += PGEFile::value("CM", PGEFile::WriteStr(FileData.authors_music));   // Credits scene background music
        if(FileData.starsShowPolicy != WorldData::STARS_UNSPECIFIED)
            outHeader += PGEFile::value("SSS", PGEFile::WriteInt(FileData.starsShowPolicy));
        if(!IsEmpty(FileData.custom_params))
            outHeader += PGEFile::value("XTRA", PGEFile::WriteStr(FileData.custom_params));   // World-wide extra settings
        if(!IsEmpty(FileData.meta.configPackId))
            outHeader += PGEFile::value("CPID", PGEFile::WriteStr(FileData.meta.configPackId));

        if(!IsEmpty(outHeader))
            out << "HEAD\n" << outHeader << "\n" << "HEAD_END\n";
    }

    //////////////////////////////////////MetaData////////////////////////////////////////////////
    //Bookmarks
    if(!FileData.metaData.bookmarks.empty())
    {
        out << "META_BOOKMARKS\n";

        for(i = 0; i < FileData.metaData.bookmarks.size(); i++)
        {
            Bookmark &bm = FileData.metaData.bookmarks[i];
            //Bookmark name
            out << PGEFile::value("BM", PGEFile::WriteStr(bm.bookmarkName));
            out << PGEFile::value("X", PGEFile::WriteRoundFloat(bm.x));
            out << PGEFile::value("Y", PGEFile::WriteRoundFloat(bm.y));
            out << "\n";
        }

        out << "META_BOOKMARKS_END\n";
    }

    //Some System information
    if(FileData.metaData.crash.used)
    {
        out << "META_SYS_CRASH\n";
        out << PGEFile::value("UT", PGEFile::WriteBool(FileData.metaData.crash.untitled));
        out << PGEFile::value("MD", PGEFile::WriteBool(FileData.metaData.crash.modifyed));
        out << PGEFile::value("FF", PGEFile::WriteInt(FileData.metaData.crash.fmtID));
        out << PGEFile::value("FV", PGEFile::WriteInt(FileData.metaData.crash.fmtVer));
        out << PGEFile::value("N",  PGEFile::WriteStr(FileData.metaData.crash.filename));
        out << PGEFile::value("P",  PGEFile::WriteStr(FileData.metaData.crash.path));
        out << PGEFile::value("FP", PGEFile::WriteStr(FileData.metaData.crash.fullPath));
        out << "\n";
        out << "META_SYS_CRASH_END\n";
    }
    //////////////////////////////////////MetaData///END//////////////////////////////////////////

    if(!FileData.tiles.empty())
    {
        out << "TILES\n";

        for(i = 0; i < FileData.tiles.size(); i++)
        {
            WorldTerrainTile &tt = FileData.tiles[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(tt.id));
            out << PGEFile::value("X", PGEFile::WriteInt(tt.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(tt.y));
            if(!IsEmpty(tt.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(tt.meta.custom_params));
            out << "\n";
        }

        out << "TILES_END\n";
    }

    if(!FileData.scenery.empty())
    {
        out << "SCENERY\n";

        for(i = 0; i < FileData.scenery.size(); i++)
        {
            WorldScenery &ws = FileData.scenery[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(ws.id));
            out << PGEFile::value("X", PGEFile::WriteInt(ws.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(ws.y));
            if(!IsEmpty(ws.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(ws.meta.custom_params));
            out << "\n";
        }

        out << "SCENERY_END\n";
    }

    if(!FileData.paths.empty())
    {
        out << "PATHS\n";

        for(i = 0; i < FileData.paths.size(); i++)
        {
            WorldPathTile &wp = FileData.paths[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(wp.id));
            out << PGEFile::value("X", PGEFile::WriteInt(wp.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(wp.y));
            if(!IsEmpty(wp.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(wp.meta.custom_params));
            out << "\n";
        }

        out << "PATHS_END\n";
    }

    if(!FileData.music.empty())
    {
        out << "MUSICBOXES\n";

        for(i = 0; i < FileData.music.size(); i++)
        {
            WorldMusicBox &wm = FileData.music[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(wm.id));
            out << PGEFile::value("X", PGEFile::WriteInt(wm.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(wm.y));
            if(!IsEmpty(wm.music_file))
                out << PGEFile::value("MF", PGEFile::WriteStr(wm.music_file));
            if(!IsEmpty(wm.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(wm.meta.custom_params));
            out << "\n";
        }

        out << "MUSICBOXES_END\n";
    }

    if(!FileData.arearects.empty())
    {
        out << "AREARECTS\n";

        WorldAreaRect defA;

        for(i = 0; i < FileData.arearects.size(); i++)
        {
            WorldAreaRect &a = FileData.arearects[i];
            out << PGEFile::value("F", PGEFile::WriteInt(a.flags));
            out << PGEFile::value("X", PGEFile::WriteInt(a.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(a.y));
            out << PGEFile::value("W", PGEFile::WriteInt(a.w));
            out << PGEFile::value("H", PGEFile::WriteInt(a.h));

            // unused stuff
            if(a.music_id)
                out << PGEFile::value("MI", PGEFile::WriteInt(a.music_id));
            if(!IsEmpty(a.music_file))
                out << PGEFile::value("MF", PGEFile::WriteStr(a.music_file));
            if(!IsEmpty(a.layer) && a.layer != defA.layer)
                out << PGEFile::value("LR", PGEFile::WriteStr(a.layer));
            if(!IsEmpty(a.eventBreak))
                out << PGEFile::value("EB", PGEFile::WriteStr(a.eventBreak));
            if(!IsEmpty(a.eventWarp))
                out << PGEFile::value("EW", PGEFile::WriteStr(a.eventWarp));
            if(!IsEmpty(a.eventAnchor))
                out << PGEFile::value("EA", PGEFile::WriteStr(a.eventAnchor));
            if(!IsEmpty(a.eventTouch))
                out << PGEFile::value("ET", PGEFile::WriteStr(a.eventTouch));
            if(a.eventTouchPolicy != defA.eventTouchPolicy)
                out << PGEFile::value("TP", PGEFile::WriteInt(a.eventTouchPolicy));
            if(!IsEmpty(a.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(a.meta.custom_params));
            out << "\n";
        }

        out << "AREARECTS_END\n";
    }

    if(!FileData.levels.empty())
    {
        out << "LEVELS\n";
        WorldLevelTile defLvl = CreateWldLevel();

        for(i = 0; i < FileData.levels.size(); i++)
        {
            WorldLevelTile &lt = FileData.levels[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(lt.id));
            out << PGEFile::value("X",  PGEFile::WriteInt(lt.x));
            out << PGEFile::value("Y",  PGEFile::WriteInt(lt.y));
            if(!IsEmpty(lt.title))
                out << PGEFile::value("LT", PGEFile::WriteStr(lt.title));
            if(!IsEmpty(lt.lvlfile))
                out << PGEFile::value("LF", PGEFile::WriteStr(lt.lvlfile));
            if(lt.entertowarp != defLvl.entertowarp)
                out << PGEFile::value("EI", PGEFile::WriteInt(lt.entertowarp));
            if(lt.left_exit != defLvl.left_exit)
                out << PGEFile::value("EL", PGEFile::WriteInt(lt.left_exit));
            if(lt.top_exit != defLvl.top_exit)
                out << PGEFile::value("ET", PGEFile::WriteInt(lt.top_exit));
            if(lt.right_exit != defLvl.right_exit)
                out << PGEFile::value("ER", PGEFile::WriteInt(lt.right_exit));
            if(lt.bottom_exit != defLvl.bottom_exit)
                out << PGEFile::value("EB", PGEFile::WriteInt(lt.bottom_exit));
            if(lt.gotox != defLvl.gotox)
                out << PGEFile::value("WX", PGEFile::WriteInt(lt.gotox));
            if(lt.gotoy != defLvl.gotoy)
                out << PGEFile::value("WY", PGEFile::WriteInt(lt.gotoy));
            if(lt.alwaysVisible)
                out << PGEFile::value("AV", PGEFile::WriteBool(lt.alwaysVisible));
            if(lt.gamestart)
                out << PGEFile::value("SP", PGEFile::WriteBool(lt.gamestart));
            if(lt.pathbg)
                out << PGEFile::value("BP", PGEFile::WriteBool(lt.pathbg));
            if(lt.bigpathbg)
                out << PGEFile::value("BG", PGEFile::WriteBool(lt.bigpathbg));
            if(lt.starsShowPolicy != WorldLevelTile::STARS_UNSPECIFIED)
                out << PGEFile::value("SSS", PGEFile::WriteInt(lt.starsShowPolicy));
            if(!IsEmpty(lt.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(lt.meta.custom_params));
            out << "\n";
        }

        out << "LEVELS_END\n";
    }

    return true;
}
