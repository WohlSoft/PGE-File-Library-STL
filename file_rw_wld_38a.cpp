/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_file_lib_sys.h"
#include "file_formats.h"
#include "file_strlist.h"

#include "smbx38a_private.h"

// Settings
static constexpr int newest_file_format = 67;

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX38AWldFileHeader(PGESTRING filePath, WorldData& FileData)
{
    errorString.clear();
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = WorldData::SMBX38A;
    #if !defined(_MSC_VER) || _MSC_VER > 1800
    PGE_FileFormats_misc::TextFileInput inf;

    if(!inf.open(filePath, false))
    {
        FileData.meta.ReadFileValid = false;
        return false;
    }

    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.meta.filename = in_1.basename();
    FileData.meta.path = in_1.dirpath();
    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);

    try
    {
        CSVPGEReader readerBridge(&inf);
        auto dataReader = MakeCSVReaderForPGESTRING(&readerBridge, '|');
        PGESTRING fileIndentifier = dataReader.ReadField<PGESTRING>(1);
        dataReader.ReadDataLine();

        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        while(!inf.eof())
        {
            PGESTRING identifier = dataReader.ReadField<PGESTRING>(1);

            if(identifier == "ws1")
            {
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        //  wn=episode name[***urlencode!***]
                                        MakeCSVPostProcessor(&FileData.EpisodeTitle, PGEUrlDecodeFunc),
                                        //  bp(n)=don't use player(n) as player's character
                                        MakeCSVSubReader(dataReader, ',',
                                                    MakeCSVOptional(&FileData.nocharacter1, false),
                                                    MakeCSVOptional(&FileData.nocharacter2, false),
                                                    MakeCSVOptional(&FileData.nocharacter3, false),
                                                    MakeCSVOptional(&FileData.nocharacter4, false),
                                                    MakeCSVOptional(&FileData.nocharacter5, false)
                                        ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  asn=auto start level file name[***urlencode!***]
                                                        MakeCSVPostProcessor(&FileData.IntroLevel_file, PGEUrlDecodeFunc),
                                                        //  gon=game over level file name[***urlencode!***]
                                                        MakeCSVPostProcessor(&FileData.GameOverLevel_file, PGEUrlDecodeFunc)
                                                        ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  dtp=disable two player[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictSinglePlayer, false),
                                                        //  nwm=no world map[0=false !0=true]
                                                        MakeCSVOptional(&FileData.HubStyledWorld, false),
                                                        //  rsd=restart last level on player's character death[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restartlevel, false),
                                                        //  dcp=disable change player[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictCharacterSwitch, false),
                                                        //  sc=save machine code to sav file[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictSecureGameSave, false),
                                                        //  sm=save mode
                                                        MakeCSVOptional(&FileData.saveResumePolicy, 0),
                                                        //  asg=auto save game[0=false !0=true]
                                                        MakeCSVOptional(&FileData.saveAuto, false),
                                                        //  smb3=smb3 style world map[0=false !0=true]
                                                        MakeCSVOptional(&FileData.showEverything, false)
                                                    ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  sn=star number
                                                        MakeCSVOptional(&FileData.stars, 0),
                                                        //  mis=max item number in world inventory
                                                        MakeCSVOptional(&FileData.inventoryLimit, 0)
                                                        ),
                                                        //  acm=anti cheat mode[0=don't allow in list !0=allow in list]
                                                        &FileData.cheatsPolicy,
                                                        //  sc=enable save locker[0=false !0=true]
                                                        &FileData.saveLocker
                                        );
                FileData.charactersFromS64();
            }
            else if(identifier == "ws2")
            {
                dataReader.ReadDataLine(CSVDiscard(),
                                        //  credits=[1]
                                        //  #DEFT#xxxxxx[***base64encode!***]
                                        //  xxxxxx=name1 /n name2 /n ...
                                        //  [2]
                                        //  #CUST#xxxxxx[***base64encode!***]
                                        //  xxxxxx=any string
                                        MakeCSVPostProcessor(&FileData.authors, [](PGESTRING& value)
                                        {
                                            PGESTRING prefix = PGE_SubStr(value, 0, 6);
                                            if((prefix == "#DEFT#") || (prefix == "#CUST#"))
                                                PGE_RemStrRng(value, 0, 6);
                                            value = PGE_BASE64DEC(value);
                                        })
                                        );
            }
            else if(identifier == "ws3")
            {
                PGESTRING cheatsList;
                dataReader.ReadDataLine(CSVDiscard(),
                                        //  list=xxxxxx[***base64encode!***] (list of forbidden)
                                        //          xxxxxx=string1,string2...stringn
                                        MakeCSVPostProcessor(&cheatsList, [&](PGESTRING& value)
                                        {
                                            PGESTRING list = PGE_URLDEC(value);
                                            PGE_SPLITSTRING(FileData.cheatsList, list, ",");
                                        })
                                        );
            }
            else
                dataReader.ReadDataLine();
        }
    }
    catch(const std::exception &err)
    {
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(newest_file_format) + "format\n"
                                   "Caused by: \n" + PGESTRING(exception_to_pretty_string(err).c_str());
        FileData.meta.ERROR_linenum = inf.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = "";
        return false;
    }

    FileData.CurSection = 0;
    FileData.playmusic = 0;
    return true;
    #else
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Unsupported on MSVC2013";
    return false;
    #endif
}

bool FileFormats::ReadSMBX38AWldFileF(PGESTRING filePath, WorldData& FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;

    if(!file.open(filePath, false))
    {
        errorString = "Failed to open file for read";
        FileData.meta.ERROR_info = errorString;
        FileData.meta.ERROR_linedata = "";
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadSMBX38AWldFile(file, FileData);
}

bool FileFormats::ReadSMBX38AWldFileRaw(PGESTRING& rawdata, PGESTRING filePath, WorldData& FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;

    if(!file.open(&rawdata, filePath))
    {
        errorString = "Failed to open raw string for read";
        FileData.meta.ERROR_info = errorString;
        FileData.meta.ERROR_linedata = "";
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadSMBX38AWldFile(file, FileData);
}

bool FileFormats::ReadSMBX38AWldFile(PGE_FileFormats_misc::TextInput& in, WorldData& FileData)
{
    errorString = "WIP";
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linedata = "";
    FileData.meta.ERROR_linenum = -1;
    FileData.meta.ReadFileValid = false;
    return false;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteSMBX38AWldFileF(PGESTRING filePath, WorldData& FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;

    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open file for write";
        return false;
    }

    return WriteSMBX38AWldFile(file, FileData);
}

bool FileFormats::WriteSMBX38AWldFileRaw(WorldData& FileData, PGESTRING& rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open raw string for write";
        return false;
    }

    return WriteSMBX38AWldFile(file, FileData);
}

bool FileFormats::WriteSMBX38AWldFile(PGE_FileFormats_misc::TextOutput& out, WorldData& FileData)
{
    errorString = "WIP";
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linedata = "";
    FileData.meta.ERROR_linenum = -1;
    FileData.meta.ReadFileValid = false;
    return false;
}

