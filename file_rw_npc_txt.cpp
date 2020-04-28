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
#include "file_strlist.h"
#include "smbx64.h"

#include <functional>

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
bool FileFormats::ReadNpcTXTFileF(PGESTRING filePath, NPCConfigFile &FileData, bool IgnoreBad)
{
    FileData.errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath, false))
    {
        FileData.errorString = "Failed to open file for read";
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadNpcTXTFile(file, FileData, IgnoreBad);
}

bool FileFormats::ReadNpcTXTFileRAW(PGESTRING &rawdata, NPCConfigFile &FileData, bool IgnoreBad)
{
    FileData.errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;
    if(!file.open(&rawdata))
    {
        FileData.errorString = "Failed to open raw string for read";
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadNpcTXTFile(file, FileData, IgnoreBad);
}



static inline PGESTRING invalidLine_SINT(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be signed intger number!>\n";
}

static inline PGESTRING invalidLine_UINT(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be unsigned intger number!>\n";
}

static inline PGESTRING invalidLine_FLT(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be floating point number!>\n";
}

static inline PGESTRING invalidLine_BOOL(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be 1 or 0!>\n";
}

bool FileFormats::ReadNpcTXTFile(PGE_FileFormats_misc::TextInput &inf, NPCConfigFile &fileData, bool ignoreBad)
{
    PGESTRING line;           //Current Line data
    PGESTRINGList params;
    fileData = CreateEmpytNpcTXT();
    bool doLog = !ignoreBad;

    auto handlerSInt = [&](bool &dest_en, int32_t &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsSInt(input))
        {
            if(doLog)
                fileData.unknownLines += invalidLine_SINT(inf.getCurrentLineNumber(), line);
        }
        else
        {
            if(input.size() > 9)
                dest = 0;
            else
            {
                dest = toInt(input);
                dest_en = true;
            }
        }
    };

    auto handlerUInt = [&](bool &dest_en, uint32_t &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsUInt(input))
        {
            if(doLog)
                fileData.unknownLines += invalidLine_UINT(inf.getCurrentLineNumber(), line);
        }
        else
        {
            if(input.size() > 9)
                dest = 0;
            else
            {
                dest = toUInt(input);
                dest_en = true;
            }
        }
    };

    auto handlerBool = [&](bool &dest_en, bool &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsBool(input))
        {
            if(doLog)
                fileData.unknownLines += invalidLine_BOOL(inf.getCurrentLineNumber(), line);
        }
        else
        {
            if(input.size() > 9)
                dest = 0;
            else
            {
                dest = static_cast<bool>(toInt(input) != 0);
                dest_en = true;
            }
        }
    };

    auto handlerDouble = [&](bool &dest_en, double &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsFloat(input))
        {
            if(doLog)
                fileData.unknownLines += invalidLine_FLT(inf.getCurrentLineNumber(), line);
        }
        else
        {
            dest = toDouble(input);
            dest_en = true;
        }
    };

    auto handlerString = [&](bool &dest_en, PGESTRING &dest, PGESTRING &input)
    {
        dest    = removeQuotes(input);
        dest_en = !IsEmpty(input);
    };

    typedef PGEHASH<PGESTRING, std::function<void(PGESTRING&)>> NpcCfgHandlerMap;
    #define SINT_ENTRY(param_name) { #param_name, [&](PGESTRING &param) { handlerSInt(fileData.en_##param_name, fileData.param_name, param);} }
    #define UINT_ENTRY(param_name) { #param_name, [&](PGESTRING &param) { handlerUInt(fileData.en_##param_name, fileData.param_name, param);} }
    #define BOOL_ENTRY(param_name) { #param_name, [&](PGESTRING &param) { handlerBool(fileData.en_##param_name, fileData.param_name, param);} }
    #define STR_ENTRY(param_name)  { #param_name, [&](PGESTRING &param) { handlerString(fileData.en_##param_name, fileData.param_name, param);} }
    #define FLT_ENTRY(param_name)  { #param_name, [&](PGESTRING &param) { handlerDouble(fileData.en_##param_name, fileData.param_name, param);} }

    NpcCfgHandlerMap paramsHandler =
    {
        SINT_ENTRY(gfxoffsetx),
        SINT_ENTRY(gfxoffsety),
        UINT_ENTRY(width),
        UINT_ENTRY(height),
        UINT_ENTRY(gfxwidth),
        UINT_ENTRY(gfxheight),
        UINT_ENTRY(score),
        UINT_ENTRY(health),
        BOOL_ENTRY(playerblock),
        BOOL_ENTRY(playerblocktop),
        BOOL_ENTRY(npcblock),
        BOOL_ENTRY(npcblocktop),
        BOOL_ENTRY(grabside),
        BOOL_ENTRY(grabtop),
        BOOL_ENTRY(jumphurt),
        BOOL_ENTRY(nohurt),
        BOOL_ENTRY(noblockcollision),
        BOOL_ENTRY(cliffturn),
        BOOL_ENTRY(noyoshi),
        BOOL_ENTRY(foreground),
        FLT_ENTRY(speed),
        BOOL_ENTRY(nofireball),
        BOOL_ENTRY(nogravity),
        UINT_ENTRY(frames),
        UINT_ENTRY(framespeed),
        UINT_ENTRY(framestyle),
        BOOL_ENTRY(noiceball),
        // Non-SMBX64 parameters (not working in SMBX <=1.3)
        BOOL_ENTRY(nohammer),
        BOOL_ENTRY(noshell),
        STR_ENTRY(name),
        STR_ENTRY(description),
        STR_ENTRY(image),
        STR_ENTRY(icon),
        STR_ENTRY(script),
        STR_ENTRY(group),
        STR_ENTRY(category),
        UINT_ENTRY(grid),
        SINT_ENTRY(gridoffsetx),
        SINT_ENTRY(gridoffsety),
        UINT_ENTRY(gridalign),
    };

    //Read NPC.TXT File config
#define NextLine(line) line = inf.readCVSLine();

    do
    {
        NextLine(line)
        if(IsEmpty(PGE_RemSubSTRING(line, " ")))
            continue;//Skip empty strings

        params.clear();

        // split the Parameter and value (example: chicken=2)
#ifdef PGE_FILES_QT
        int splitSign = line.indexOf('=');
        if(splitSign < 0) // Invalid line
        {
            if(doLog)
                fileData.unknownLines += fromNum(inf.getCurrentLineNumber()) + ": " + line + " <wrong syntax!>\n";
            continue;
        }
        params.push_back(line.mid(0, splitSign));
        params.push_back(line.mid(splitSign + 1, -1));
#else
        size_t splitSign = line.find('=');
        if(splitSign == std::string::npos)
        {
            if(doLog)
                fileData.unknownLines += fromNum(inf.getCurrentLineNumber()) + ": " + line + " <wrong syntax!>\n";
            continue;
        }
        params.push_back(line.substr(0, splitSign));
        params.push_back(line.substr(splitSign + 1, std::string::npos));
#endif

        if(params.size() != 2) // If string does not contain strings with "=" as separator
        {
            if(doLog)
                fileData.unknownLines += fromNum(inf.getCurrentLineNumber()) + ": " + line + " <wrong syntax!>\n";
            if(doLog || (params.size() < 2))
                continue;
        }

        params[0] = PGESTR_Trim(params[0]);
        params[0] = PGESTR_Simpl(params[0]);
        params[0] = PGE_RemSubSTRING(params[0], " "); //Delete spaces
        params[0] = PGESTR_toLower(params[0]);//To lower case

        params[1] = PGESTR_Trim(params[1]); // Trim it!

        NpcCfgHandlerMap::iterator hand = paramsHandler.find(params[0]);
        if(hand != paramsHandler.end())
        {
            PGEMAPVAL(hand)(params[1]);
        }
        else
        {
            // Custom value
            fileData.entries[params[0]] = params[1];
            if(doLog) //[DEPRECATED] Store unknown value into warnings list
                fileData.unknownLines += fromNum(inf.getCurrentLineNumber()) + ": " + line + "\n";
        }
    }
    while(!inf.eof());

    fileData.ReadFileValid = true;
    return true;
}




//*********************************************************
//****************WRITE FILE*******************************
//*********************************************************
bool FileFormats::WriteNPCTxtFileF(PGESTRING filePath, NPCConfigFile &FileData)
{
    PGE_FileFormats_misc::TextFileOutput file;
    FileData.errorString.clear();
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.errorString = "Failed to open file for write";
        return false;
    }
    return WriteNPCTxtFile(file, FileData);
}

bool FileFormats::WriteNPCTxtFileRaw(NPCConfigFile &FileData, PGESTRING &rawdata)
{
    PGE_FileFormats_misc::RawTextOutput file;
    FileData.errorString.clear();
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.errorString = "Failed to open target raw string for write";
        return false;
    }
    return WriteNPCTxtFile(file, FileData);
}

//Convert NPC Options structore to text for saving
bool FileFormats::WriteNPCTxtFile(PGE_FileFormats_misc::TextOutput &out, NPCConfigFile &fileData)
{
    if(fileData.en_gfxoffsetx)
        out << "gfxoffsetx=" + fromNum(fileData.gfxoffsetx) + "\n";
    if(fileData.en_gfxoffsety)
        out << "gfxoffsety=" + fromNum(fileData.gfxoffsety) + "\n";
    if(fileData.en_gfxwidth)
        out << "gfxwidth=" + fromNum(fileData.gfxwidth) + "\n";
    if(fileData.en_gfxheight)
        out << "gfxheight=" + fromNum(fileData.gfxheight) + "\n";
    if(fileData.en_foreground)
        out << "foreground=" + fromBoolToNum(fileData.foreground) + "\n";
    if(fileData.en_width)
        out << "width=" + fromNum(fileData.width) + "\n";
    if(fileData.en_height)
        out << "height=" + fromNum(fileData.height) + "\n";

    if(fileData.en_score)
        out << "score=" + fromNum(fileData.score) + "\n";
    if(fileData.en_health)
        out << "health=" + fromNum(fileData.health) + "\n";

    if(fileData.en_playerblock)
        out << "playerblock=" + fromBoolToNum(fileData.playerblock) + "\n";

    if(fileData.en_playerblocktop)
        out << "playerblocktop=" + fromBoolToNum(fileData.playerblocktop) + "\n";

    if(fileData.en_npcblock)
        out << "npcblock=" + fromBoolToNum(fileData.npcblock) + "\n";

    if(fileData.en_npcblocktop)
        out << "npcblocktop=" + fromBoolToNum(fileData.npcblocktop) + "\n";
    if(fileData.en_grabside)
        out << "grabside=" + fromBoolToNum(fileData.grabside) + "\n";
    if(fileData.en_grabtop)
        out << "grabtop=" + fromBoolToNum(fileData.grabtop) + "\n";
    if(fileData.en_jumphurt)
        out << "jumphurt=" + fromBoolToNum(fileData.jumphurt) + "\n";
    if(fileData.en_nohurt)
        out << "nohurt=" + fromBoolToNum(fileData.nohurt) + "\n";
    if(fileData.en_speed)
        out << "speed=" + fromNum(fileData.speed) + "\n";
    if(fileData.en_noblockcollision)
        out << "noblockcollision=" + fromBoolToNum(fileData.noblockcollision) + "\n";
    if(fileData.en_cliffturn)
        out << "cliffturn=" + fromBoolToNum(fileData.cliffturn) + "\n";
    if(fileData.en_noyoshi)
        out << "noyoshi=" + fromBoolToNum(fileData.noyoshi) + "\n";
    if(fileData.en_nofireball)
        out << "nofireball=" + fromBoolToNum(fileData.nofireball) + "\n";
    if(fileData.en_nogravity)
        out << "nogravity=" + fromBoolToNum(fileData.nogravity) + "\n";
    if(fileData.en_noiceball)
        out << "noiceball=" + fromBoolToNum(fileData.noiceball) + "\n";
    if(fileData.en_frames)
        out << "frames=" + fromNum(fileData.frames) + "\n";
    if(fileData.en_framespeed)
        out << "framespeed=" + fromNum(fileData.framespeed) + "\n";
    if(fileData.en_framestyle)
        out << "framestyle=" + fromNum(fileData.framestyle) + "\n";

    //Extended
    if(fileData.en_nohammer)
        out << "nohammer=" + fromBoolToNum(fileData.nohammer) + "\n";
    if(fileData.en_noshell)
        out << "noshell=" + fromBoolToNum(fileData.noshell) + "\n";
    if(fileData.en_name && !IsEmpty(fileData.name))
        out << "name=" + SMBX64::WriteStr(fileData.name);
    if(fileData.en_description && !IsEmpty(fileData.description))
        out << "description=" + SMBX64::WriteStr(fileData.description);
    if(fileData.en_image && !IsEmpty(fileData.image))
        out << "image=" + SMBX64::WriteStr(fileData.image);
    if(fileData.en_icon && !IsEmpty(fileData.icon))
        out << "icon=" + SMBX64::WriteStr(fileData.icon);
    if(fileData.en_script && !IsEmpty(fileData.script))
        out << "script=" + SMBX64::WriteStr(fileData.script);
    if(fileData.en_group && !IsEmpty(fileData.group))
        out << "group=" + SMBX64::WriteStr(fileData.group);
    if(fileData.en_category && !IsEmpty(fileData.category))
        out << "category=" + SMBX64::WriteStr(fileData.category);
    if(fileData.en_grid)
        out << "grid=" + fromNum(fileData.grid) + "\n";
    if(fileData.en_gridoffsetx)
        out << "gridoffsetx=" + fromNum(fileData.gridoffsetx) + "\n";
    if(fileData.en_gridoffsety)
        out << "gridoffsety=" + fromNum(fileData.gridoffsety) + "\n";
    if(fileData.en_gridalign)
        out << "gridalign=" + fromNum(fileData.gridalign) + "\n";

    // Custom values
    for(NPCConfigFile::EntriesMap::iterator it = fileData.entries.begin(); it != fileData.entries.end(); ++it)
    {
        out << PGEMAPKEY(it) << "=" << PGEMAPVAL(it) + "\n";
    }

    return true;
}
