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
#endif

#include "file_formats.h"
#include "pge_file_lib_private.h"

bool FileFormats::OpenLevelFile(const PGESTRING &filePath, LevelData &FileData)
{
    PGE_FileFormats_misc::TextFileInput file;

    if(!file.open(filePath, true))
    {
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        return false;
    }

    return OpenLevelFileT(file, FileData);
}

bool FileFormats::OpenLevelRaw(PGESTRING &rawdata, const PGESTRING &filePath, LevelData &FileData)
{
    PGE_FileFormats_misc::RawTextInput file;

    if(!file.open(&rawdata, filePath))
    {
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        return false;
    }

    return OpenLevelFileT(file, FileData);
}

bool FileFormats::OpenLevelFileT(PGE_FileFormats_misc::TextInput &file, LevelData &FileData)
{
    PGESTRING firstLine;
    CreateLevelData(FileData);

    FileData.meta.ERROR_info.clear();
    firstLine = file.read(8);
    file.seek(0, PGE_FileFormats_misc::TextInput::begin);

    if(PGE_StartsWith(firstLine, "SMBXFile"))
    {
        //Read SMBX65-38A LVL File
        if(!ReadSMBX38ALvlFile(file, FileData))
            return false;
    }
    else if(PGE_FileFormats_misc::PGE_DetectSMBXFile(firstLine))
    {
        //Disable UTF8 for SMBX64 files
        if(!file.reOpen(false))
        {
            FileData.meta.ReadFileValid = false;
            return false;
        }
        //Read SMBX LVL File
        if(!ReadSMBX64LvlFile(file, FileData))
            return false;
    }
    else
    {
        //Read PGE LVLX File
        if(!ReadExtendedLvlFile(file, FileData))
            return false;
    }

    if(PGE_FileFormats_misc::TextFileInput::exists(file.getFilePath() + ".meta"))
    {
        if(!ReadNonSMBX64MetaDataF(file.getFilePath() + ".meta", FileData.metaData))
            FileData.meta.ERROR_info = "Can't open meta-file";
    }

    return true;
}

bool FileFormats::OpenLevelFileHeader(const PGESTRING &filePath, LevelData &data)
{
    PGE_FileFormats_misc::TextFileInput file;
    data.meta.ERROR_info.clear();

    if(!file.open(filePath, true))
    {
        data.meta.ReadFileValid = false;
        data.meta.ERROR_info = "Can't open file";
        data.meta.ERROR_linedata.clear();
        data.meta.ERROR_linenum = -1;
        return false;
    }
    return OpenLevelFileHeaderT(file, data);
}

bool FileFormats::OpenLevelFileHeaderRaw(PGESTRING &rawdata, const PGESTRING &filePath, LevelData &data)
{
    PGE_FileFormats_misc::RawTextInput file;
    data.meta.ERROR_info.clear();

    if(!file.open(&rawdata, filePath))
    {
        data.meta.ReadFileValid = false;
        data.meta.ERROR_info = "Can't open file";
        data.meta.ERROR_linedata.clear();
        data.meta.ERROR_linenum = -1;
        return false;
    }
    return OpenLevelFileHeaderT(file, data);
}

bool FileFormats::OpenLevelFileHeaderT(PGE_FileFormats_misc::TextInput &file, LevelData &data)
{
    PGESTRING firstLine;

    CreateLevelHeader(data);

    firstLine = file.readLine();
    file.seek(0, PGE_FileFormats_misc::TextInput::begin);

    if(PGE_StartsWith(firstLine, "SMBXFile"))
    {
        //Read SMBX65-38A LVL File
        return ReadSMBX38ALvlFileHeaderT(file, data);
    }
    else if(PGE_FileFormats_misc::PGE_DetectSMBXFile(firstLine))
    {
        //Disable UTF8 for SMBX64 files
        if(!file.reOpen(false))
        {
            data.meta.ReadFileValid = false;
            return false;
        }
        //Read SMBX LVL File
        return ReadSMBX64LvlFileHeaderT(file, data);
    }
    else
    {
        //Read PGE LVLX File
        return ReadExtendedLvlFileHeaderT(file, data);
    }
}


bool FileFormats::SaveLevelFile(LevelData &FileData, const PGESTRING &filePath, LevelFileFormat format, unsigned int FormatVersion)
{
    FileData.meta.ERROR_info.clear();
    switch(format)
    {
    case LVL_PGEX:
    {
        FileData.stars = smbx64CountStars(FileData);
        if(!FileFormats::WriteExtendedLvlFileF(filePath, FileData))
        {
            FileData.meta.ERROR_info += "Cannot save file " + filePath + ".";
            return false;
        }
        return true;
    }
    //break;
    case LVL_SMBX64:
    {
        //Apply SMBX64-specific things to entire array
        smbx64LevelPrepare(FileData);

        if(!FileFormats::WriteSMBX64LvlFileF(filePath, FileData, FormatVersion))
        {
            FileData.meta.ERROR_info += "Cannot save file " + filePath + ".";
            return false;
        }

        //save additional meta data
        if(!FileData.metaData.bookmarks.empty())
        {
            if(!FileFormats::WriteNonSMBX64MetaDataF(filePath + ".meta", FileData.metaData))
            {
                FileData.meta.ERROR_info += "Cannot save file " + filePath + ".meta.";
                return false;
            }
        }
        return true;
    }
    //break;
    case LVL_SMBX38A:
    {
        if(!FileFormats::WriteSMBX38ALvlFileF(filePath, FileData))
        {
            FileData.meta.ERROR_info = "Cannot save file " + filePath + ".";
            return false;
        }
        return true;
    }
        //break;
    }
    FileData.meta.ERROR_info = "Unsupported file type";
    return false;
}

bool FileFormats::SaveLevelData(LevelData &FileData, PGESTRING &RawData, LevelFileFormat format, unsigned int FormatVersion)
{
    FileData.meta.ERROR_info.clear();
    switch(format)
    {
    case LVL_PGEX:
    {
        FileData.stars = smbx64CountStars(FileData);
        WriteExtendedLvlFileRaw(FileData, RawData);
        return true;
    }
    //break;
    case LVL_SMBX64:
    {
        smbx64LevelPrepare(FileData);
        WriteSMBX64LvlFileRaw(FileData, RawData, FormatVersion);
        return true;
    }
    //break;
    case LVL_SMBX38A:
    {
        FileFormats::WriteSMBX38ALvlFileRaw(FileData, RawData);
        return true;
    }
        //break;
    }
    FileData.meta.ERROR_info = "Unsupported file type";
    FileData.meta.ReadFileValid = false;
    return false;
}



bool FileFormats::OpenWorldFile(const PGESTRING &filePath, WorldData &data)
{
    PGE_FileFormats_misc::TextFileInput file;

    if(!file.open(filePath, true))
    {
        data.meta.ReadFileValid = false;
        data.meta.ERROR_info = "Can't open file";
        data.meta.ERROR_linedata.clear();
        data.meta.ERROR_linenum = -1;
        return false;
    }

    return OpenWorldFileT(file, data);
}

bool FileFormats::OpenWorldRaw(PGESTRING &rawdata, const PGESTRING &filePath, WorldData &FileData)
{
    PGE_FileFormats_misc::RawTextInput file;

    if(!file.open(&rawdata, filePath))
    {
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ERROR_linedata.clear();
        FileData.meta.ERROR_linenum = -1;
        return false;
    }

    return OpenWorldFileT(file, FileData);
}

bool FileFormats::OpenWorldFileT(PGE_FileFormats_misc::TextInput &file, WorldData &data)
{
    PGESTRING firstLine;

    CreateWorldData(data);

    data.meta.ERROR_info.clear();
    firstLine = file.read(8);
    file.seek(0, PGE_FileFormats_misc::TextInput::begin);

    if(PGE_StartsWith(firstLine, "SMBXFile"))
    {
        //Read SMBX-38A WLD File
        if(!ReadSMBX38AWldFile(file, data))
            return false;
    }
    else if(PGE_FileFormats_misc::PGE_DetectSMBXFile(firstLine))
    {
        //Disable UTF8 for SMBX64 files
        if(!file.reOpen(false))
        {
            data.meta.ReadFileValid = false;
            return false;
        }
        //Read SMBX WLD File
        if(!ReadSMBX64WldFile(file, data))
            return false;
    }
    else
    {
        //Read PGE WLDX File
        if(!ReadExtendedWldFile(file, data))
            return false;
    }

    if(PGE_FileFormats_misc::TextFileInput::exists(file.getFilePath() + ".meta"))
    {
        if(!ReadNonSMBX64MetaDataF(file.getFilePath() + ".meta", data.metaData))
            data.meta.ERROR_info = "Can't open meta-file";
    }

    return true;
}

bool FileFormats::OpenWorldFileHeader(const PGESTRING &filePath, WorldData &data)
{
    PGE_FileFormats_misc::TextFileInput file;
    data.meta.ERROR_info.clear();

    if(!file.open(filePath, true))
    {
        data.meta.ReadFileValid = false;
        data.meta.ERROR_info = "Can't open file";
        data.meta.ERROR_linedata.clear();
        data.meta.ERROR_linenum = -1;
        return false;
    }

    return OpenWorldFileHeaderT(file, data);
}

bool FileFormats::OpenWorldFileHeaderRaw(PGESTRING &rawdata, const PGESTRING &filePath, WorldData &data)
{
    PGE_FileFormats_misc::RawTextInput file;
    data.meta.ERROR_info.clear();

    if(!file.open(&rawdata, filePath))
    {
        data.meta.ReadFileValid = false;
        data.meta.ERROR_info = "Can't open file";
        data.meta.ERROR_linedata.clear();
        data.meta.ERROR_linenum = -1;
        return false;
    }

    return OpenWorldFileHeaderT(file, data);
}

bool FileFormats::OpenWorldFileHeaderT(PGE_FileFormats_misc::TextInput &file, WorldData &data)
{
    PGESTRING firstLine;

    CreateWorldHeader(data);

    firstLine = file.readLine();
    file.seek(0, PGE_FileFormats_misc::TextInput::begin);

    if(PGE_StartsWith(firstLine, "SMBXFile"))
    {
        //Read SMBX-38A WLD File
        return ReadSMBX38AWldFileHeaderT(file, data);
    }
    else if(PGE_FileFormats_misc::PGE_DetectSMBXFile(firstLine))
    {
        //Disable UTF8 for SMBX64 files
        if(!file.reOpen(false))
        {
            data.meta.ReadFileValid = false;
            return false;
        }
        //Read SMBX WLD File
        return ReadSMBX64WldFileHeaderT(file, data);
    }
    else
    {
        //Read PGE WLDX File
        return ReadExtendedWldFileHeaderT(file, data);
    }
}

bool FileFormats::SaveWorldFile(WorldData &FileData, const PGESTRING &filePath, FileFormats::WorldFileFormat format, unsigned int FormatVersion)
{
    FileData.meta.ERROR_info.clear();
    switch(format)
    {
    case WLD_PGEX:
    {
        if(!FileFormats::WriteExtendedWldFileF(filePath, FileData))
        {
            FileData.meta.ERROR_info += "Cannot save file " + filePath + ".";
            return false;
        }
        return true;
    }
    //break;
    case WLD_SMBX64:
    {
        if(!FileFormats::WriteSMBX64WldFileF(filePath, FileData, FormatVersion))
        {
            FileData.meta.ERROR_info += "Cannot save file " + filePath + ".";
            return false;
        }

        //save additional meta data
        if(!FileData.metaData.bookmarks.empty())
        {
            if(!FileFormats::WriteNonSMBX64MetaDataF(filePath + ".meta", FileData.metaData))
            {
                FileData.meta.ERROR_info += "Cannot save file " + filePath + ".meta.";
                return false;
            }
        }
        return true;
    }
    //break;
    case WLD_SMBX38A:
    {
        if(!FileFormats::WriteSMBX38AWldFileF(filePath, FileData))
        {
            FileData.meta.ERROR_info += "Cannot save file " + filePath + ".";
            return false;
        }
        return true;
    }
        //break;
    }
    FileData.meta.ERROR_info = "Unsupported file type";
    return false;
}

bool FileFormats::SaveWorldData(WorldData &FileData, PGESTRING &RawData, FileFormats::WorldFileFormat format, unsigned int FormatVersion)
{
    FileData.meta.ERROR_info.clear();
    switch(format)
    {
    case WLD_PGEX:
    {
        WriteExtendedWldFileRaw(FileData, RawData);
        return true;
    }
    //break;
    case WLD_SMBX64:
    {
        WriteSMBX64WldFileRaw(FileData, RawData, FormatVersion);
        return true;
    }
    //break;
    case WLD_SMBX38A:
    {
        WriteSMBX38AWldFileRaw(FileData, RawData);
        return true;
    }
        //break;
    }
    FileData.meta.ERROR_info = "Unsupported file type";
    return false;
}
