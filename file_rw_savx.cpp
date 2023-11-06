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
#include "pge_x.h"
#include "pge_x_macro.h"

#ifdef PGE_FILES_QT
#include <QDir>
#include <QFileInfo>
#endif

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
bool FileFormats::ReadExtendedSaveFileF(const PGESTRING &filePath, GamesaveData &FileData)
{
    FileData.meta.ERROR_info.clear();
    PGE_FileFormats_misc::TextFileInput file;

    if(!file.open(filePath, true))
    {
        FileData.meta.ERROR_info = "Failed to open file for read";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadExtendedSaveFile(file, FileData);
}

bool FileFormats::ReadExtendedSaveFileRaw(PGESTRING &rawdata, const PGESTRING &filePath, GamesaveData &FileData)
{
    FileData.meta.ERROR_info.clear();
    PGE_FileFormats_misc::RawTextInput file;

    if(!file.open(&rawdata, filePath))
    {
        FileData.meta.ERROR_info = "Failed to open raw string for read";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadExtendedSaveFile(file, FileData);
}

bool FileFormats::ReadExtendedSaveFile(PGE_FileFormats_misc::TextInput &in, GamesaveData &FileData)
{
    FileData = CreateGameSaveData();
    PGESTRING errorString;
    PGEX_FileBegin();
    saveCharState plr_state;
    visibleItem        vz_item;
    starOnLevel        star_level;
    saveLevelInfo      level_info;
    saveUserData::DataSection user_data_entry;
    //Add path data
    PGESTRING fPath = in.getFilePath();

    if(!IsEmpty(fPath))
    {
        PGE_FileFormats_misc::FileInfo in_1(fPath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    FileData.characterStates.clear();
    FileData.currentCharacter.clear();
    FileData.meta.untitled = false;
    FileData.meta.modified = false;
    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEX_FileParseTree(in.readAll());
    PGEX_FetchSection()
    {
        PGEX_FetchSection_begin()
        ///////////////////HEADER//////////////////////
        PGEX_Section("SAVE_HEADER")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_USIntVal("LV", FileData.lives)
                    PGEX_UIntVal("CN", FileData.coins)
                    PGEX_UIntVal("PT", FileData.points)
                    PGEX_UIntVal("TS", FileData.totalStars)
                    PGEX_SIntVal("WX", FileData.worldPosX)
                    PGEX_SIntVal("WY", FileData.worldPosY)
                    PGEX_ULongVal("HW", FileData.last_hub_warp)
                    PGEX_UIntVal("MI", FileData.musicID)
                    PGEX_StrVal("MF", FileData.musicFile)
                    PGEX_BoolVal("GC", FileData.gameCompleted)
                }
            }
        }//Header
        ///////////////////CHARACTERS//////////////////////
        PGEX_Section("CHARACTERS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                plr_state = CreateSavCharacterState();
                PGEX_Values()
                {
                    PGEX_ValueBegin()
                    PGEX_ULongVal("ID", plr_state.id)
                    PGEX_ULongVal("ST", plr_state.state)
                    PGEX_ULongVal("IT", plr_state.itemID)
                    PGEX_UIntVal("MT", plr_state.mountType)
                    PGEX_UIntVal("MI", plr_state.mountID)
                    PGEX_UIntVal("HL", plr_state.health)
                }
                FileData.characterStates.push_back(plr_state);
            }
        }//CHARACTERS
        ///////////////////CHARACTERS_PER_PLAYERS//////////////////////
        PGEX_Section("CHARACTERS_PER_PLAYERS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                unsigned long character = 0;
                PGEX_Values()
                {
                    PGEX_ValueBegin()
                    PGEX_ULongVal("ID", character)
                }
                FileData.currentCharacter.push_back(character);
            }
        }//CHARACTERS_PER_PLAYERS
        ///////////////////VIZ_LEVELS//////////////////////
        PGEX_Section("VIZ_LEVELS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                vz_item.first = 0;
                vz_item.second = false;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", vz_item.first)
                    PGEX_BoolVal("V", vz_item.second)
                }
                FileData.visibleLevels.push_back(vz_item);
            }
        }//VIZ_LEVELS
        ///////////////////VIZ_PATHS//////////////////////
        PGEX_Section("VIZ_PATHS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                vz_item.first = 0;
                vz_item.second = false;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", vz_item.first)
                    PGEX_BoolVal("V", vz_item.second)
                }
                FileData.visiblePaths.push_back(vz_item);
            }
        }//VIZ_PATHS
        ///////////////////VIZ_SCENERY//////////////////////
        PGEX_Section("VIZ_SCENERY")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                vz_item.first = 0;
                vz_item.second = false;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", vz_item.first)
                    PGEX_BoolVal("V", vz_item.second)
                }
                FileData.visibleScenery.push_back(vz_item);
            }
        }//VIZ_SCENERY
        ///////////////////STARS//////////////////////
        PGEX_Section("STARS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                star_level.first.clear();
                star_level.second = 0;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("L", star_level.first)
                    PGEX_SIntVal("S", star_level.second)
                }
                FileData.gottenStars.push_back(star_level);
            }
        }//STARS
        ///////////////////LEVEL INFO//////////////////////
        PGEX_Section("LEVEL_INFO")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                level_info = saveLevelInfo();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("L", level_info.level_filename)
                    PGEX_UIntVal("S", level_info.max_stars)
                    PGEX_UIntVal("M", level_info.max_medals)
                    PGEX_BoolArrVal("MG", level_info.medals_got)
                    PGEX_BoolArrVal("MB", level_info.medals_best)
                }
                FileData.levelInfo.push_back(level_info);
            }
        }//LEVEL_INFO
        ///////////////////USERDATA//////////////////////
        PGEX_Section("USERDATA")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                user_data_entry.data.clear();
                user_data_entry.name = "default";
                user_data_entry.location_name.clear();
                user_data_entry.location = saveUserData::DATA_WORLD;
                PGESTRINGList data;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_SIntVal("L", user_data_entry.location)
                    PGEX_StrVal("SN", user_data_entry.name)
                    PGEX_StrVal("LN", user_data_entry.location_name)
                    PGEX_StrArrVal("D", data)
                }
                for(PGESTRING &s : data)
                {
                    saveUserData::DataEntry e;
                    PGESTRINGList dp;
                    PGE_SPLITSTRING(dp, s, "=");
                    if(dp.size() < 2)
                        goto badfile;
                    e.key = PGE_ReplSTRING(PGEFile::X2STRING(dp[0]), "\\q", "=");
                    e.value = PGE_ReplSTRING(PGEFile::X2STRING(dp[1]), "\\q", "=");
                    user_data_entry.data.push_back(e);
                }
                FileData.userData.store.push_back(user_data_entry);
            }
        }//USERDATA
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////
    errorString.clear(); //If no errors, clear string;
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

bool FileFormats::WriteExtendedSaveFileF(const PGESTRING &filePath, GamesaveData &FileData)
{
    FileData.meta.ERROR_info.clear();
    PGE_FileFormats_misc::TextFileOutput file;

    if(!file.open(filePath, true, false, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open file for write";
        return false;
    }

    return WriteExtendedSaveFile(file, FileData);
}

bool FileFormats::WriteExtendedSaveFileRaw(GamesaveData &FileData, PGESTRING &rawdata)
{
    FileData.meta.ERROR_info.clear();
    PGE_FileFormats_misc::RawTextOutput file;

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open raw string for write";
        return false;
    }

    return WriteExtendedSaveFile(file, FileData);
}

bool FileFormats::WriteExtendedSaveFile(PGE_FileFormats_misc::TextOutput &out, GamesaveData &FileData)
{
    pge_size_t i;
    out << "SAVE_HEADER\n";
    out << PGEFile::value("LV", PGEFile::WriteInt(FileData.lives));
    out << PGEFile::value("CN", PGEFile::WriteInt(FileData.coins));
    out << PGEFile::value("PT", PGEFile::WriteInt(FileData.points));
    out << PGEFile::value("TS", PGEFile::WriteInt(FileData.totalStars));
    out << PGEFile::value("WX", PGEFile::WriteInt(FileData.worldPosX));
    out << PGEFile::value("WY", PGEFile::WriteInt(FileData.worldPosY));
    out << PGEFile::value("HW", PGEFile::WriteInt(FileData.last_hub_warp));
    out << PGEFile::value("MI", PGEFile::WriteInt(FileData.musicID));
    out << PGEFile::value("MF", PGEFile::WriteStr(FileData.musicFile));
    out << PGEFile::value("GC", PGEFile::WriteBool(FileData.gameCompleted));
    out << "\n";
    out << "SAVE_HEADER_END\n";

    if(!FileData.characterStates.empty())
    {
        out << "CHARACTERS\n";

        for(i = 0; i < FileData.characterStates.size(); i++)
        {
            saveCharState &chState = FileData.characterStates[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(chState.id));
            out << PGEFile::value("ST", PGEFile::WriteInt(chState.state));
            out << PGEFile::value("IT", PGEFile::WriteInt(chState.itemID));
            out << PGEFile::value("MT", PGEFile::WriteInt(chState.mountType));
            out << PGEFile::value("MI", PGEFile::WriteInt(chState.mountID));
            out << PGEFile::value("HL", PGEFile::WriteInt(chState.health));
            out << "\n";
        }

        out << "CHARACTERS_END\n";
    }

    if(!FileData.currentCharacter.empty())
    {
        out << "CHARACTERS_PER_PLAYERS\n";

        for(i = 0; i < FileData.currentCharacter.size(); i++)
        {
            out << PGEFile::value("ID", PGEFile::WriteInt(FileData.currentCharacter[i]));
            out << "\n";
        }

        out << "CHARACTERS_PER_PLAYERS_END\n";
    }

    if(!FileData.visibleLevels.empty())
    {
        out << "VIZ_LEVELS\n";

        for(i = 0; i < FileData.visibleLevels.size(); i++)
        {
            visibleItem &slevel = FileData.visibleLevels[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(slevel.first));
            out << PGEFile::value("V", PGEFile::WriteBool(slevel.second));
            out << "\n";
        }

        out << "VIZ_LEVELS_END\n";
    }

    if(!FileData.visiblePaths.empty())
    {
        out << "VIZ_PATHS\n";

        for(i = 0; i < FileData.visiblePaths.size(); i++)
        {
            visibleItem &slevel = FileData.visiblePaths[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(slevel.first));
            out << PGEFile::value("V", PGEFile::WriteBool(slevel.second));
            out << "\n";
        }

        out << "VIZ_PATHS_END\n";
    }

    if(!FileData.visibleScenery.empty())
    {
        out << "VIZ_SCENERY\n";

        for(i = 0; i < FileData.visibleScenery.size(); i++)
        {
            visibleItem &slevel = FileData.visibleScenery[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(slevel.first));
            out << PGEFile::value("V", PGEFile::WriteBool(slevel.second));
            out << "\n";
        }

        out << "VIZ_SCENERY_END\n";
    }

    if(!FileData.gottenStars.empty())
    {
        out << "STARS\n";

        for(i = 0; i < FileData.gottenStars.size(); i++)
        {
            starOnLevel &slevel = FileData.gottenStars[i];
            out << PGEFile::value("L", PGEFile::WriteStr(slevel.first));
            out << PGEFile::value("S", PGEFile::WriteInt(slevel.second));
            out << "\n";
        }

        out << "STARS_END\n";
    }

    if(!FileData.levelInfo.empty())
    {
        out << "LEVEL_INFO\n";

        for(i = 0; i < FileData.levelInfo.size(); i++)
        {
            saveLevelInfo &slinfo = FileData.levelInfo[i];
            out << PGEFile::value("L", PGEFile::WriteStr(slinfo.level_filename));
            out << PGEFile::value("S", PGEFile::WriteInt(slinfo.max_stars));
            out << PGEFile::value("M", PGEFile::WriteInt(slinfo.max_medals));

            if(!slinfo.medals_got.empty())
                out << PGEFile::value("MG", PGEFile::WriteBoolArr(slinfo.medals_got));

            if(!slinfo.medals_best.empty())
                out << PGEFile::value("MB", PGEFile::WriteBoolArr(slinfo.medals_best));

            out << "\n";
        }

        out << "LEVEL_INFO_END\n";
    }

    if(!FileData.userData.store.empty())
    {
        out << "USERDATA\n";

        for(const auto &e : FileData.userData.store)
        {
            if((e.location & saveUserData::DATA_VOLATILE_FLAG) != 0)
                continue;// Don't save volatile fields into the file!

            int location_clean = (e.location & saveUserData::DATA_LOCATION_MASK);
            out << PGEFile::value("L", PGEFile::WriteInt(location_clean));

            if(!IsEmpty(e.name) && (e.name != "default"))
                out << PGEFile::value("SN", PGEFile::WriteStr(e.name));

            if(!IsEmpty(e.location_name))
                out << PGEFile::value("LN", PGEFile::WriteStr(e.location_name));

            PGESTRINGList data;
            for(const auto &d : e.data)
            {
                PGESTRING key   = PGE_ReplSTRING(d.key, "=", "\\q"),
                          value = PGE_ReplSTRING(d.value, "=", "\\q");
                data.PGESTRING_EMPLACE(PGEFile::WriteStr(key) + "=" + PGEFile::WriteStr(value));
            }

            out << PGEFile::value("D", PGEFile::WriteStrArr(data));
            out << "\n";
        }
        out << "USERDATA_END\n";
    }

    out << "\n";
    return true;
}
