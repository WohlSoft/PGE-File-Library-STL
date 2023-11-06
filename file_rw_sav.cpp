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

#ifdef PGE_FILES_QT
#include <QFileInfo>
#include <QDir>
#else
#include <stdlib.h>
#include <limits.h> /* PATH_MAX */
#endif

#include "file_formats.h"
#include "file_strlist.h"
#include "save_filedata.h"
#include "smbx64.h"
#include "smbx64_macro.h"
#include "CSVUtils.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX64SavFileF(const PGESTRING &filePath, GamesaveData &FileData)
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
    return ReadSMBX64SavFile(file, FileData);
}

bool FileFormats::ReadSMBX64SavFileRaw(PGESTRING &rawdata, const PGESTRING &filePath,  GamesaveData &FileData)
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
    return ReadSMBX64SavFile(file, FileData);
}

bool FileFormats::ReadSMBX64SavFile(PGE_FileFormats_misc::TextInput &in, GamesaveData &FileData)
{
    SMBX64_FileBegin();
    PGESTRING filePath = in.getFilePath();
    FileData.meta.ERROR_info.clear();
    //SMBX64_File( RawData );

    int i;                  //counters
    int arrayIdCounter = 0;
    //GamesaveData FileData;
    FileData = CreateGameSaveData();

    FileData.meta.untitled = false;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    //Enable strict mode for SMBX LVL file format
    FileData.meta.smbx64strict = true;

    try
    {
        ///////////////////////////////////////Begin file///////////////////////////////////////
        nextLine();
        SMBX64::ReadUInt(&file_format, line);//File format number
        FileData.meta.RecentFormatVersion = file_format;
        nextLine();
        SMBX64::ReadSInt(&FileData.lives, line); //Number of lives
        nextLine();
        SMBX64::ReadUInt(&FileData.coins, line); //Number of coins
        nextLine();
        SMBX64::ReadSInt(&FileData.worldPosX, line);  //World map pos X
        nextLine();
        SMBX64::ReadSInt(&FileData.worldPosY, line);  //World map pos Y

        for(i = 0; i < (ge(56) ? 5 : 2) ; i++)
        {
            saveCharState charState;
            charState = CreateSavCharacterState();
            nextLine();
            SMBX64::ReadUInt(&charState.state, line);//Character's power up state
            nextLine();
            SMBX64::ReadUInt(&charState.itemID, line); //ID of item in the slot
            if(ge(10))
            {
                nextLine();    //Type of mount
                SMBX64::ReadUInt(&charState.mountType, line);
            }
            nextLine();
            SMBX64::ReadUInt(&charState.mountID, line); //ID of mount
            if(lt(10))
            {
                if(charState.mountID > 0) charState.mountType = 1;
            }
            if(ge(56))
            {
                nextLine();    //ID of mount
                SMBX64::ReadUInt(&charState.health, line);
            }
            FileData.characterStates.push_back(charState);
        }

        nextLine();
        SMBX64::ReadUInt(&FileData.musicID, line);//ID of music
        nextLine();
        if(IsEmpty(line) || in.eof())
            goto successful;

        if(ge(56))
        {
            SMBX64::ReadCSVBool(&FileData.gameCompleted, line);   //Game was complited
        }

        arrayIdCounter = 1;

        nextLine();
        while((line != "next") && (!in.eof()))
        {
            visibleItem level;
            level.first = (unsigned int)arrayIdCounter;
            level.second = false;
            SMBX64::ReadCSVBool(&level.second, line); //Is level shown

            FileData.visibleLevels.push_back(level);
            arrayIdCounter++;
            nextLine();
        }

        arrayIdCounter = 1;
        nextLine();
        while((line != "next") && (!in.eof()))
        {
            visibleItem level;
            level.first = (unsigned int)arrayIdCounter;
            level.second = false;
            SMBX64::ReadCSVBool(&level.second, line); //Is path shown

            FileData.visiblePaths.push_back(level);
            arrayIdCounter++;
            nextLine();
        }

        arrayIdCounter = 1;
        nextLine();
        while((line != "next") && (!in.eof()))
        {
            visibleItem level;
            level.first = (unsigned int)arrayIdCounter;
            level.second = false;
            SMBX64::ReadCSVBool(&level.second, line); //Is Scenery shown

            FileData.visibleScenery.push_back(level);
            arrayIdCounter++;
            nextLine();
        }

        if(ge(7))
        {
            nextLine();
            while((line != "next") && (!IsNULL(line)))
            {
                starOnLevel gottenStar;
                gottenStar.first.clear();
                gottenStar.second = 0;

                SMBX64::ReadStr(&gottenStar.first, line);//Level file
                if(ge(16))
                {
                    nextLine();    //Section ID
                    SMBX64::ReadUInt(&gottenStar.second, line);
                }

                FileData.gottenStars.push_back(gottenStar);
                nextLine();
            }
        }

        if(ge(21))
        {
            nextLine();
            if(IsEmpty(line) || in.eof())
                goto successful;
            SMBX64::ReadUInt(&FileData.totalStars, line);//Total Number of stars
        }

successful:
        ///////////////////////////////////////EndFile///////////////////////////////////////
        FileData.meta.ReadFileValid = true;
        return true;
    }
    catch(const std::exception &err)
    {
        if(file_format > 0)
            FileData.meta.ERROR_info = "Detected file format: SMBX-" + fromNum(file_format) + " is invalid\n";
        else
            FileData.meta.ERROR_info = "It is not an SMBX game save file\n";
        #ifdef PGE_FILES_QT
        FileData.meta.ERROR_info += QString::fromStdString(exception_to_pretty_string(err));
        #else
        FileData.meta.ERROR_info += exception_to_pretty_string(err);
        #endif
        FileData.meta.ERROR_linenum = in.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = line;
        FileData.meta.ReadFileValid = false;
        PGE_CutLength(FileData.meta.ERROR_linedata, 50);
        PGE_FilterBinary(FileData.meta.ERROR_linedata);
        return false;
    }
}

