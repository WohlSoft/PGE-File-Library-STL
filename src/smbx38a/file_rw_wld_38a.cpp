/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "smbx38a_private.h"


//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX38AWldFileHeader(const PGESTRING &filePath, WorldData& FileData)
{
    FileData.meta.ERROR_info.clear();
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = WorldData::SMBX38A;
    PGE_FileFormats_misc::TextFileInput inf;

    if(!inf.open(filePath, false))
    {
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadSMBX38AWldFileHeaderT(inf, FileData);
}

bool FileFormats::ReadSMBX38AWldFileHeaderRaw(PGESTRING &rawdata, const PGESTRING &filePath, WorldData &FileData)
{
    FileData.meta.ERROR_info.clear();
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = LevelData::SMBX38A;
    PGE_FileFormats_misc::RawTextInput inf;

    if(!inf.open(&rawdata, filePath))
    {
        FileData.meta.ERROR_info = "Can't open file";
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadSMBX38AWldFileHeaderT(inf, FileData);
}

bool FileFormats::ReadSMBX38AWldFileHeaderT(PGE_FileFormats_misc::TextInput &inf, WorldData &FileData)
{
#if !defined(_MSC_VER) || _MSC_VER > 1800
    PGE_FileFormats_misc::FileInfo in_1(inf.getFilePath());
    FileData.meta.filename = in_1.basename();
    FileData.meta.path = in_1.dirpath();
    FileData.meta.RecentFormat = WorldData::SMBX38A;
    FileData.meta.RecentFormatVersion = c_latest_version_smbx38a;
    FileData.nocharacter.clear();
    // Mark all 38A levels with a "SMBX-38A" key
    FileData.meta.configPackId = "SMBX-38A";

    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);

    try
    {
        CSVPGEReader readerBridge(&inf);
        auto dataReader = MakeCSVReaderForPGESTRING(&readerBridge, '|');
        PGESTRING fileIndentifier = dataReader.ReadField<PGESTRING>(1);
        dataReader.ReadDataLine();

        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        FileData.meta.RecentFormatVersion = toUInt(PGE_SubStr(fileIndentifier, 8, -1));

        if(FileData.meta.RecentFormatVersion > c_latest_version_smbx38a)
            throw std::logic_error("File format has newer version which is not supported yet");

        while(!inf.eof())
        {
            PGESTRING identifier = dataReader.ReadField<PGESTRING>(1);

            if(identifier == "WS1")
            {
                // ws1|wn|bp1,bp2,bp3,bp4,bp5|asn,gvn|dtp,nwm,rsd,dcp,sc,sm,asg,smb3,dss|sn,mis|acm|sc
                dataReader.ReadDataLine(
                    CSVDiscard(), // Skip the first field (this is already "identifier")
                    //  wn=episode name[***urlencode!***]
                    MakeCSVPostProcessor(&FileData.EpisodeTitle, PGEUrlDecodeFunc),
                    //  bp(n)=don't use player(n) as player's character
                    MakeCSVSubReader(
                        dataReader, ',',
                        MakeCSVOptional(&FileData.nocharacter1, false),
                        MakeCSVOptional(&FileData.nocharacter2, false),
                        MakeCSVOptional(&FileData.nocharacter3, false),
                        MakeCSVOptional(&FileData.nocharacter4, false),
                        MakeCSVOptional(&FileData.nocharacter5, false)
                    ),
                    MakeCSVSubReader(
                        dataReader, ',',
                        //  asn=auto start level file name[***urlencode!***]
                        MakeCSVPostProcessor(&FileData.IntroLevel_file, PGEUrlDecodeFunc),
                        //  gon=game over level file name[***urlencode!***]
                        MakeCSVOptional(&FileData.GameOverLevel_file, "", nullptr, PGEUrlDecodeFunc)
                    ),
                    MakeCSVSubReader(
                        dataReader, ',',
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
                        MakeCSVOptional(&FileData.showEverything, false),
                        //  dss=No Entry Scene
                        MakeCSVOptional(&FileData.disableEnterScreen, false)
                    ),
                    MakeCSVSubReader(
                        dataReader, ',',
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
            else if(identifier == "WS2")
            {
                // ws2|credits|creditsmusic
                dataReader.ReadDataLine(
                    CSVDiscard(),
                    //  credits=[1]
                    //  #DEFT#xxxxxx[***base64encode!***]
                    //  xxxxxx=name1 /n name2 /n ...
                    //  [2]
                    //  #CUST#xxxxxx[***base64encode!***]
                    //  xxxxxx=any string
                    MakeCSVPostProcessor(&FileData.authors, [&FileData](PGESTRING& value)
                    {
                        PGESTRING prefix = PGE_SubStr(value, 0, 6);
                        bool isCustom = (prefix == "#CUST#");

                        if((prefix == "#DEFT#") || (prefix == "#CUST#"))
                            PGE_RemStrRng(value, 0, 6);

                        value = PGE_BASE64DEC(value);
                        // Convert into LF
                        PGE_ReplSTRING(value, "\r\n", "\n");

                        // Do fill SMBX64 authors too
                        if(!isCustom)
                        {
                            PGESTRINGList authors;
                            PGE_SPLITSTRING(authors, value, "\n");
                            if(authors.size() > 0) FileData.author1 = authors[0];
                            if(authors.size() > 1) FileData.author2 = authors[1];
                            if(authors.size() > 2) FileData.author3 = authors[2];
                            if(authors.size() > 3) FileData.author4 = authors[3];
                            if(authors.size() > 4) FileData.author5 = authors[4];
                        }
                    }),
                    MakeCSVOptionalEmpty(&FileData.authors_music, "", nullptr, PGEUrlDecodeFunc)
                );
            }
            else if(identifier == "WS3")
            {
                PGESTRING cheatsList;
                dataReader.ReadDataLine(
                    CSVDiscard(),
                    //  list=xxxxxx[***base64encode!***] (list of forbidden)
                    //          xxxxxx=string1,string2...stringn
                    MakeCSVPostProcessor(&cheatsList, [&](PGESTRING& value)
                    {
                        PGESTRING list = PGE_URLDEC(value);
                        PGE_SPLITSTRING(FileData.cheatsList, list, ",");
                    })
                );
            }
            else if(identifier == "WS4")
            {
                dataReader.ReadDataLine(
                    CSVDiscard(),
                    //    se=save locker syntax[***urlencode!***][syntax]
                    MakeCSVPostProcessor(&FileData.saveLockerEx, PGEUrlDecodeFunc),
                    //    msg=message when save was locked[***urlencode!***]
                    MakeCSVPostProcessor(&FileData.saveLockerMsg, PGEUrlDecodeFunc)
                );
            }
            else
            {
                break; //Abort on first non-head line to don't fetch entire file
                //dataReader.ReadDataLine();
            }
        }
    }
    catch(const std::exception &err)
    {
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-38A-" + fromNum(FileData.meta.RecentFormatVersion) + " format\n"
                                   "Caused by: \n" + toPgeString(exception_to_pretty_string(err));
        FileData.meta.ERROR_linenum = inf.getCurrentLineNumber();
        FileData.meta.ERROR_linedata.clear();
        return false;
    }

    FileData.CurSection = 0;
    FileData.playmusic = 0;
    return true;
#else
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Unsupported on MSVC2013 or lower";
    return false;
#endif
}

bool FileFormats::ReadSMBX38AWldFileF(const PGESTRING &filePath, WorldData& FileData)
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

    return ReadSMBX38AWldFile(file, FileData);
}

bool FileFormats::ReadSMBX38AWldFileRaw(PGESTRING& rawdata, const PGESTRING &filePath, WorldData& FileData)
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

    return ReadSMBX38AWldFile(file, FileData);
}

bool FileFormats::ReadSMBX38AWldFile(PGE_FileFormats_misc::TextInput& in, WorldData& FileData)
{
    SMBX38A_FileBeginN();
    PGESTRING filePath = in.getFilePath();
    FileData.meta.ERROR_info.clear();

    CreateWorldData(FileData);

    FileData.meta.RecentFormat = WorldData::SMBX38A;
    FileData.meta.RecentFormatVersion = c_latest_version_smbx38a;

#if !defined(_MSC_VER) || _MSC_VER > 1800
    FileData.EpisodeTitle.clear();
    FileData.stars = 0;
    FileData.CurSection = 0;
    FileData.playmusic = false;

    //Enable strict mode for SMBX LVL file format
    FileData.meta.smbx64strict = false;

    //Begin all ArrayID's here;
    FileData.tile_array_id = 1;
    FileData.scene_array_id = 1;
    FileData.path_array_id = 1;
    FileData.level_array_id = 1;
    FileData.musicbox_array_id = 1;

    // Mark all 38A levels with a "SMBX-38A" key
    FileData.meta.configPackId = "SMBX-38A";

    WorldTerrainTile    tile;
    WorldScenery        scen;
    WorldPathTile       pathitem;
    WorldLevelTile      lvlitem;
    WorldMusicBox       musicbox;
    WorldAreaRect       arearect;
    WorldLayer          layer;
    WorldEvent38A       event;
    WorldItemSetup38A   customcfg;

    PGESTRING           identifier;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    in.seek(0, PGE_FileFormats_misc::TextFileInput::begin);

    try
    {
        CSVPGEReader readerBridge(&in);
        auto dataReader = MakeCSVReaderForPGESTRING(&readerBridge, '|');
        PGESTRING fileIndentifier = dataReader.ReadField<PGESTRING>(1);
        dataReader.ReadDataLine();

        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        FileData.meta.RecentFormatVersion = toUInt(PGE_SubStr(fileIndentifier, 8, -1));

        if(FileData.meta.RecentFormatVersion > c_latest_version_smbx38a)
            throw std::logic_error("File format has newer version which is not supported yet");

        while(!in.eof())
        {
            identifier = dataReader.ReadField<PGESTRING>(1);

            if(identifier == "WS1")
            {
                dataReader.ReadDataLine(
                    CSVDiscard(), // Skip the first field (this is already "identifier")
                    //  wn=episode name[***urlencode!***]
                    MakeCSVPostProcessor(&FileData.EpisodeTitle, PGEUrlDecodeFunc),
                    //  bp(n)=don't use player(n) as player's character
                    MakeCSVSubReader(
                        dataReader, ',',
                        MakeCSVOptional(&FileData.nocharacter1, false),
                        MakeCSVOptional(&FileData.nocharacter2, false),
                        MakeCSVOptional(&FileData.nocharacter3, false),
                        MakeCSVOptional(&FileData.nocharacter4, false),
                        MakeCSVOptional(&FileData.nocharacter5, false)
                    ),
                    MakeCSVSubReader(
                        dataReader, ',',
                        //  asn=auto start level file name[***urlencode!***]
                        MakeCSVPostProcessor(&FileData.IntroLevel_file, PGEUrlDecodeFunc),
                        //  gon=game over level file name[***urlencode!***]
                        MakeCSVOptional(&FileData.GameOverLevel_file, "", nullptr, PGEUrlDecodeFunc)
                    ),
                    MakeCSVSubReader(
                        dataReader, ',',
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
                        MakeCSVOptional(&FileData.showEverything, false),
                        //  dss=No Entry Scene
                        MakeCSVOptional(&FileData.disableEnterScreen, false)
                    ),
                    MakeCSVSubReader(
                        dataReader, ',',
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
            else if(identifier == "WS2")
            {
                dataReader.ReadDataLine(
                    CSVDiscard(),
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
                    }),
                    MakeCSVOptionalEmpty(&FileData.authors_music, "", nullptr, PGEUrlDecodeFunc)
                );
            }
            else if(identifier == "WS3")
            {
                PGESTRING cheatsList;

                dataReader.ReadDataLine(
                    CSVDiscard(),
                    //  list=xxxxxx[***base64encode!***] (list of forbidden)
                    //          xxxxxx=string1,string2...stringn
                    MakeCSVPostProcessor(&cheatsList, [&](PGESTRING& value)
                    {
                        PGESTRING list = PGE_URLDEC(value);
                        PGE_SPLITSTRING(FileData.cheatsList, list, ",");
                    })
                );
            }
            else if(identifier == "WS4")
            {
                dataReader.ReadDataLine(
                    CSVDiscard(),
                    //    se=save locker syntax[***urlencode!***][syntax]
                    MakeCSVPostProcessor(&FileData.saveLockerEx, PGEUrlDecodeFunc),
                    //    msg=message when save was locked[***urlencode!***]
                    MakeCSVPostProcessor(&FileData.saveLockerMsg, PGEUrlDecodeFunc)
                );
            }
            else if(identifier == "T")
            {
                tile = WorldTerrainTile();

                dataReader.ReadDataLine(
                    CSVDiscard(),
                    MakeCSVSubReader(
                        dataReader, ',',
                        &tile.id,
                        MakeCSVOptional(&tile.gfx_dx, 0),
                        MakeCSVOptional(&tile.gfx_dy, 0)
                    ),
                    &tile.x,
                    &tile.y,
                    MakeCSVOptional(&tile.layer, "Default", nullptr, PGELayerOrDefault)
                );

                tile.meta.array_id = FileData.tile_array_id++;
                FileData.tiles.push_back(tile);
            }
            else if(identifier == "S")
            {
                scen = WorldScenery();

                dataReader.ReadDataLine(
                    CSVDiscard(),
                    MakeCSVSubReader(
                        dataReader, ',',
                        &scen.id,
                        MakeCSVOptional(&scen.gfx_dx, 0),
                        MakeCSVOptional(&scen.gfx_dy, 0)
                    ),
                    &scen.x,
                    &scen.y,
                    MakeCSVOptional(&tile.layer, "Default", nullptr, PGELayerOrDefault)
                );

                scen.meta.array_id = FileData.scene_array_id++;
                FileData.scenery.push_back(scen);
            }
            else if(identifier == "P")
            {
                pathitem = WorldPathTile();

                dataReader.ReadDataLine(
                    CSVDiscard(),
                    MakeCSVSubReader(
                        dataReader, ',',
                        &pathitem.id,
                        MakeCSVOptional(&pathitem.gfx_dx, 0),
                        MakeCSVOptional(&pathitem.gfx_dy, 0)
                    ),
                    &pathitem.x,
                    &pathitem.y,
                    MakeCSVOptional(&tile.layer, "Default", nullptr, PGELayerOrDefault)
                );

                pathitem.meta.array_id = FileData.path_array_id++;
                FileData.paths.push_back(pathitem);
            }
            else if(identifier == "M")
            {
                musicbox = WorldMusicBox();
                arearect = WorldAreaRect();
                arearect.flags = WorldAreaRect::SETUP_CHANGE_MUSIC;

                //M|10|416|1312|    |     |32|32|1   |,0
                //M|1 |384|384 |    |     |32|32|1   |%66%61%72%74,1
                //M|id|x  |y   |name|layer|w |h |flag|te,eflag      |ie1,ie2,ie3
                dataReader.ReadDataLine(
                    CSVDiscard(),
                    //id=music id
                    &arearect.music_id,
                    //x=Area position x
                    &arearect.x,
                    //y=Area position y
                    &arearect.y,
                    //name=custom music name[***urlencode!***]
                    MakeCSVPostProcessor(&arearect.music_file, PGEUrlDecodeFunc),
                    //layer=layer name["" == "Default"][***urlencode!***]
                    MakeCSVOptional(&arearect.layer, "Default", nullptr, PGELayerOrDefault),
                    //w=width
                    MakeCSVOptional(&arearect.w, 32),
                    //h=height
                    MakeCSVOptional(&arearect.h, 32),
                    //flag=area settings[***Bitwise operation***]
                    //    0=False !0=True
                    //    b1=(flag & 1) World Music
                    //    b2=(flag & 2) Set Viewport
                    //    b3=(flag & 4) Ship Route
                    //    b4=(flag & 8) Forced Walking
                    //    b5=(flag & 16) Item-triggered events
                    MakeCSVOptional(&arearect.flags, WorldAreaRect::SETUP_CHANGE_MUSIC),
                    MakeCSVOptionalSubReader(
                        dataReader, ',',
                        //te:Touch Event[***urlencode!***]
                        MakeCSVOptional(&arearect.eventTouch, "", nullptr, PGEUrlDecodeFunc),
                        //eflag:    0=Triggered every time entering
                        //          1=Triggered on entrance and level completion
                        //          2=Triggered only once
                        MakeCSVOptional(&arearect.eventTouchPolicy, 0)
                    ),
                    MakeCSVOptionalSubReader(
                        dataReader, ',',
                        //ie1=Hammer Event[***urlencode!***]
                        MakeCSVOptional(&arearect.eventBreak, "", nullptr, PGELayerOrDefault),
                        //ie2=Warp Whistle Event[***urlencode!***]
                        MakeCSVOptional(&arearect.eventWarp, "", nullptr, PGELayerOrDefault),
                        //ie3=Anchor Event[***urlencode!***]
                        MakeCSVOptional(&arearect.eventAnchor, "", nullptr, PGELayerOrDefault)
                    )
                );

                if((arearect.flags == WorldAreaRect::SETUP_CHANGE_MUSIC) &&
                   (arearect.w == 32) && (arearect.h == 32))
                {
                    //Store as generic music-box point
                    musicbox.id         = arearect.music_id;
                    musicbox.music_file = arearect.music_file;
                    musicbox.x          = arearect.x;
                    musicbox.y          = arearect.y;
                    musicbox.layer      = arearect.layer;
                    musicbox.meta.array_id = FileData.musicbox_array_id++;
                    FileData.music.push_back(musicbox);
                }
                else
                {
                    //Store as separated "Area-rect" type
                    arearect.meta.array_id = FileData.arearect_array_id++;
                    FileData.arearects.push_back(arearect);
                }
            }
            else if(identifier == "L")
            {
                //L|id[,dx,dy]|x|y|fn|n|eu\el\ed\er|wx|wy|wlz|bg,pb,av,ls,f,nsc,otl,li,lcm|s|Layer|Lmt
                lvlitem = WorldLevelTile();
                lvlitem.left_exit_extra.exit_codes = {0, 0};
                lvlitem.top_exit_extra.exit_codes = {0, 0};
                lvlitem.right_exit_extra.exit_codes = {0, 0};
                lvlitem.bottom_exit_extra.exit_codes = {0, 0};

                dataReader.ReadDataLine(
                    CSVDiscard(), //-V681
                    MakeCSVSubReader(
                        dataReader, ',',
                        //id=level id
                        &lvlitem.id,
                        //dx=graphics extend x
                        MakeCSVOptional(&lvlitem.gfx_dx, 0),
                        //dx=graphics extend y
                        MakeCSVOptional(&lvlitem.gfx_dy, 0)
                    ),
                    //x=level position x
                    &lvlitem.x,
                    //x=level position y
                    &lvlitem.y,
                    //fn=level file name[***urlencode!***]
                    MakeCSVPostProcessor(&lvlitem.lvlfile,  PGEUrlDecodeFunc),
                    //n=level name[***urlencode!***]
                    MakeCSVPostProcessor(&lvlitem.title,    PGEUrlDecodeFunc),
                    //eu\el\ed\er=e[up\left\down\right]
                    //        e=c1,c2,c3,c4
                    //        c1,c2,c3=level exit type
                    //        c4=condidtion expression[***urlencode!***][syntax]
                    //        exit = (c1 || c2 || c3) && c4
                    MakeCSVSubReader(
                        dataReader, '\\',
                        MakeCSVSubReader(
                            dataReader, PGEChar(','),
                            &lvlitem.top_exit,
                            &lvlitem.top_exit_extra.exit_codes[0],
                            &lvlitem.top_exit_extra.exit_codes[1],
                            MakeCSVPostProcessor(&lvlitem.top_exit_extra.expression, PGEUrlDecodeFunc)
                        ),
                        MakeCSVSubReader(
                            dataReader, PGEChar(','),
                            &lvlitem.left_exit,
                            &lvlitem.left_exit_extra.exit_codes[0],
                            &lvlitem.left_exit_extra.exit_codes[1],
                            MakeCSVPostProcessor(&lvlitem.left_exit_extra.expression, PGEUrlDecodeFunc)
                        ),
                        MakeCSVSubReader(
                            dataReader, PGEChar(','),
                            &lvlitem.bottom_exit,
                            &lvlitem.bottom_exit_extra.exit_codes[0],
                            &lvlitem.bottom_exit_extra.exit_codes[1],
                            MakeCSVPostProcessor(&lvlitem.bottom_exit_extra.expression, PGEUrlDecodeFunc)
                        ),
                        MakeCSVSubReader(
                            dataReader, PGEChar(','),
                            &lvlitem.right_exit,
                            &lvlitem.right_exit_extra.exit_codes[0],
                            &lvlitem.right_exit_extra.exit_codes[1],
                            MakeCSVPostProcessor(&lvlitem.right_exit_extra.expression, PGEUrlDecodeFunc)
                        )
                    ),
                    //wx=go to world map position x
                    &lvlitem.gotox,
                    //wx=go to world map position y
                    &lvlitem.gotoy,
                    //wlz=nunber of doors to warp
                    &lvlitem.entertowarp,
                    MakeCSVSubReader(
                        dataReader, ',',
                        //bg=big background
                        MakeCSVOptional(&lvlitem.bigpathbg, false),
                        //pb=path background
                        MakeCSVOptional(&lvlitem.pathbg, false),
                        //av=always visible
                        MakeCSVOptional(&lvlitem.alwaysVisible, false),
                        //ls=is game start point
                        MakeCSVOptional(&lvlitem.gamestart, false),
                        //f=forced
                        MakeCSVOptional(&lvlitem.forceStart, false),
                        //nsc=no star coin count
                        MakeCSVOptional(&lvlitem.disableStarCoinsCount, false),
                        //otl=destory after clear
                        MakeCSVOptional(&lvlitem.destroyOnCompleting, false),
                        //li=level ID
                        MakeCSVOptional(&lvlitem.levelID, 0),
                        //lcm=Affected by Music Box
                        MakeCSVOptional(&lvlitem.controlledByAreaRects, false)
                    ),
                    //TODO: Implement this
                    //s=entrance syntax
                    //        s=ds1/ds2...dsn
                    //        ds=ds1,ds2[***urlencode!***][syntax]
                    //        ds1=condidtion expression
                    //        ds2=index
                    MakeCSVOptionalIterator(dataReader, '/', [&lvlitem](const PGESTRING & nextFieldStr)
                    {
                        WorldLevelTile::EnterCondition e;
                        auto fieldReader = MakeDirectReader(nextFieldStr);
                        auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');

                        fullReader.ReadDataLine(
                            MakeCSVPostProcessor(&e.condition,  PGEUrlDecodeFunc),
                            MakeCSVPostProcessor(&e.levelIndex, PGEUrlDecodeFunc)
                        );

                        lvlitem.enter_cond.push_back(e);
                    }),
                    //layer=layer name["" == "Default"][***urlencode!***]
                    MakeCSVOptional(&lvlitem.layer, "Default", nullptr, PGELayerOrDefault),
                    //TODO: Implement this
                    //Lmt=Level Movement Command
                    //    lmt=NodeInfo\PathInfo
                    //        NodeInfo=Node1:Node2:...:NodeN
                    //            Node=x,y,chance
                    //        PathInfo=Path1:Path2:...:PathN
                    //            Path=NodeID1,NodeID2
                    MakeCSVOptionalSubReader(
                        dataReader, '\\', //-V681
                        MakeCSVOptionalIterator(dataReader, ':', [&lvlitem](const PGESTRING & nextFieldStr)
                        {
                            WorldLevelTile::Movement::Node node;
                            auto fieldReader = MakeDirectReader(nextFieldStr);
                            auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                            fullReader.ReadDataLine(
                                &node.x,
                                &node.y,
                                &node.chance
                            );
                            lvlitem.movement.nodes.push_back(node);
                        }),
                        MakeCSVOptionalIterator(dataReader, ':', [&lvlitem](const PGESTRING & nextFieldStr)
                        {
                            WorldLevelTile::Movement::Line line;
                            auto fieldReader = MakeDirectReader(nextFieldStr);
                            auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                            fullReader.ReadDataLine(
                                &line.node1,
                                &line.node2
                            );
                            lvlitem.movement.paths.push_back(line);
                        })
                    )
                );

                lvlitem.meta.array_id = FileData.level_array_id++;
                FileData.levels.push_back(lvlitem);
            }
            else if(identifier == "WL")
            {
                layer = WorldLayer();

                dataReader.ReadDataLine(
                    CSVDiscard(),
                    MakeCSVPostProcessor(&layer.name, PGELayerOrDefault),
                    &layer.hidden
                );

                layer.meta.array_id = FileData.layers_array_id++;
                FileData.layers.push_back(layer);
            }
            else if(identifier == "WE")
            {
                int way = 0;
                int autostrat;
                event = WorldEvent38A();

                //TODO: Implement world map events support
                //next line: events
                //    WE|name|layer|layerm|world|other
                dataReader.ReadDataLine(
                    CSVDiscard(),
                    //    name=event name[***urlencode!***]
                    MakeCSVPostProcessor(&event.name, PGEUrlDecodeFunc),
                    //    layer=way/hidelist/showlist/togglelist
                    //        list=name1,name2,name3...namen
                    //            name[***urlencode!***]
                    //        if (way % 10 == 1) nosmoke = true;
                    //        if (way > 10) object_state = true; else layer_state = true;
                    MakeCSVSubReader(
                        dataReader, PGEChar('/'),
                        MakeCSVPostProcessor(&way, [&event](int way)->void
                        {
                            event.nosmoke = (way % 10) == 1;
                            event.layers_mode = (way > 10) ?
                                                    WorldEvent38A::LM_StateLayers :
                                                    WorldEvent38A::LM_StateObjects;
                        }),
                        MakeCSVIterator(dataReader, ',', [&event](const PGESTRING &layer)->void
                        {
                            event.layers_hide.push_back(PGE_URLDEC(layer));
                        }),
                        MakeCSVIterator(dataReader, ',', [&event](const PGESTRING &layer)->void
                        {
                            event.layers_show.push_back(PGE_URLDEC(layer));
                        }),
                        MakeCSVIterator(dataReader, ',', [&event](const PGESTRING &layer)->void
                        {
                            event.layers_toggle.push_back(PGE_URLDEC(layer));
                        })
                    ),
                    //    layerm=movementcommand1\movementcommand2\...\movementcommandn
                    //        movementcommand=way,layer,hp,vp,ap
                    //            way:0=speed,1=coordinate,2=moveto,4=spin
                    //            layer=layer name[***urlencode!***]
                    //            hp=Horizontal Parameter[***urlencode!***]
                    //            vp=Vertical Parameter[***urlencode!***]
                    //            ap=Additional Parameter[***urlencode!***]
                    MakeCSVIterator(
                        dataReader, '\\',
                        [&event](const PGESTRING & nextFieldStr)->void
                        {
                            WorldEvent38A_LayerMove mv;
                            auto fieldReader = MakeDirectReader(nextFieldStr);
                            auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                            int type;

                            fullReader.ReadDataLine(
                                &type,
                                MakeCSVPostProcessor(&mv.layer, PGEUrlDecodeFunc),
                                MakeCSVPostProcessor(&mv.expression_param_h, PGEUrlDecodeFunc),
                                MakeCSVPostProcessor(&mv.expression_param_v, PGEUrlDecodeFunc),
                                MakeCSVPostProcessor(&mv.expression_param_extra, PGEUrlDecodeFunc)
                            );

                            mv.type = static_cast<WorldEvent38A_LayerMove::MoveType>(type);
                            SMBX38A_Exp2Double(mv.expression_param_h, mv.param_h);
                            SMBX38A_Exp2Double(mv.expression_param_v, mv.param_v);
                            SMBX38A_Exp2Double(mv.expression_param_extra, mv.param_extra);
                            event.layers_move.push_back(mv);
                        }
                    ),
                    //    world=aw/cs,le,inpc,msgc,syntax,msg
                    //        aw=AutoStart Settings
                    //            0=Not Auto Start
                    //            1=Triggered on loading the world the first time.
                    //            2=Triggered every time loading the world.
                    //            3=Triggered on level exit.
                    //        cs=Start when match all condition[0=false !0=true]
                    //        le:0=This is a Normal Event.
                    //           1=This is a Level Enter/Exit Event.
                    //        inpc=Interrupt the process if 'false' returned
                    //        msgc=Show a message if 'false' returned
                    //        syntax=Condition expression[***urlencode!***]
                    //        msg=message[***urlencode!***]
                    MakeCSVSubReader(dataReader, PGEChar('/'),
                        MakeCSVPostProcessor(&autostrat, [&event](int as)->void
                        {
                            event.autostart = static_cast<WorldEvent38A::AutoStart>(as);;
                        }),
                        MakeCSVSubReader(dataReader, PGEChar(','),
                            &event.start_on_condition,
                            &event.is_level_enter_exit,
                            &event.interrupt_on_false,
                            &event.show_msg_on_interrupt,
                            MakeCSVPostProcessor(&event.autostart_condition, PGEUrlDecodeFunc),
                            MakeCSVPostProcessor(&event.interrupt_message, PGEUrlDecodeFunc)
                        )
                    ),
                    //    other=sd/ld/event,delay/script/msg/wwx,wwy,lockl
                    //        sd=play sound number
                    //        ld=lock keyboard (frames)
                    //        event=trigger event name[***urlencode!***]
                    //        delay=trigger delay[1 frame]
                    //        script=script name[***urlencode!***]
                    //        msg=show message after start event[***urlencode!***]
                    //        wwx=Warp Whistle: Map Warp Location x
                    //        wwy=Warp Whistle: Map Warp Location y
                    //            if (wwx == -1 && wwy == -1) [means not moving]
                    //        lockl=[Level ID]Affected by Anchor
                    MakeCSVSubReader(
                        dataReader,
                        PGEChar('/'),
                        &event.sound_id,
                        &event.lock_keyboard_delay,
                        MakeCSVSubReader(
                            dataReader, PGEChar(','),
                            MakeCSVPostProcessor(&event.trigger, PGEUrlDecodeFunc),
                            &event.trigger_timer
                        ),
                        MakeCSVPostProcessor(&event.trigger_script, PGEUrlDecodeFunc),
                        MakeCSVPostProcessor(&event.msg, PGEUrlDecodeFunc),
                        MakeCSVSubReader(
                            dataReader, PGEChar(','),
                            &event.move_to_x,
                            &event.move_to_y,
                            &event.level_anchor_id
                        )
                    )
                );
                event.meta.array_id = FileData.events38A_array_id++;
                FileData.events38A.push_back(event);
            }
            else if((identifier == "WCT") || (identifier == "WCS") || (identifier == "WCL") )
            {
                //custom object data:
                //    WCT|id|data	:custom tile
                //    WCS|id|data	:custom scene
                //    WCL|id|data	:custom level
                //    id=object id
                //    data=[HEX]value|[HEX]value...
                //    [HEX]=0001	:gfxwidth
                //    [HEX]=0002	:gfxheight
                //    [HEX]=0003	:frames
                customcfg = WorldItemSetup38A();
                if(identifier == "WCT")
                    customcfg.type = WorldItemSetup38A::TERRAIN;
                else if(identifier == "WCS")
                    customcfg.type = WorldItemSetup38A::SCENERY;
                else
                    customcfg.type = WorldItemSetup38A::LEVEL;

                dataReader.ReadDataLine(
                    CSVDiscard(),
                    &customcfg.id,
                    MakeCSVIterator(
                        dataReader, ',',
                        [&customcfg](const PGESTRING & nextFieldStr)
                        {
                            WorldItemSetup38A::Entry e;
                            SMBX38A_CC_decode(e.key, e.value, nextFieldStr);
                            customcfg.data.push_back(e);
                        }
                    )
                );
                FileData.custom38A_configs.push_back(customcfg);
            }
            else
            {
                // Unsupported line, just keep it
                PGESTRING str;
                dataReader.ReadRawLine(str);
                FileData.unsupported_38a_lines.push_back(str);
            }
        }//while is not EOF
    }
    catch(const std::exception &err)
    {
        // First we try to extract the line number out of the nested exception.
        auto *possibleNestedException = dynamic_cast<const std::nested_exception *>(&err); //-V641

        if(possibleNestedException)
        {
            try
            {
                std::rethrow_exception(possibleNestedException->nested_ptr());
            }
            catch(const parse_error &parseErr)
            {
                FileData.meta.ERROR_linenum = static_cast<long>(parseErr.get_line_number());
            }
            catch(...)
            {   //-V565
                // Do Nothing
            }
        }

        // Now fill in the error data.
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-38A-" + fromNum(FileData.meta.RecentFormatVersion) + " format\n"
                                   "Caused by: \n" + toPgeString(exception_to_pretty_string(err));

        // If we were unable to find error line number from the exception, then get the line number from the file reader.
        if(FileData.meta.ERROR_linenum == 0)
            FileData.meta.ERROR_linenum = in.getCurrentLineNumber();

        FileData.meta.ERROR_linedata.clear();
        return false;
    }
    catch(...)
    {
        /*
         * This is an attempt to fix crash on Windows 32 bit release assembly,
         * and possible, on some other platforms too
         */
        // Now fill in the error data.
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-38A-" + fromNum(FileData.meta.RecentFormatVersion) + " format\n"
                                   "Caused by unknown exception\n";
        if(!IsEmpty(identifier))
            FileData.meta.ERROR_info += "\n Field type " + identifier;
        // If we were unable to find error line number from the exception, then get the line number from the file reader.
        if(FileData.meta.ERROR_linenum == 0)
            FileData.meta.ERROR_linenum = in.getCurrentLineNumber();
        FileData.meta.ERROR_linedata.clear();
        return false;
    }

    FileData.CurSection = 0;
    FileData.playmusic = 0;
    FileData.meta.ReadFileValid = true;
    return true;
#else
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Unsupported on MSVC2013 or lower";
    return false;
#endif
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteSMBX38AWldFileF(const PGESTRING &filePath, WorldData& FileData, unsigned int format_version)
{
    PGE_FileFormats_misc::TextFileOutput file;
    FileData.meta.ERROR_info.clear();

    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open file for write";
        return false;
    }

    return WriteSMBX38AWldFile(file, FileData, format_version);
}

bool FileFormats::WriteSMBX38AWldFileRaw(WorldData& FileData, PGESTRING& rawdata, unsigned int format_version)
{
    PGE_FileFormats_misc::RawTextOutput file;
    FileData.meta.ERROR_info.clear();

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        FileData.meta.ERROR_info = "Failed to open raw string for write";
        return false;
    }

    return WriteSMBX38AWldFile(file, FileData, format_version);
}

bool FileFormats::WriteSMBX38AWldFile(PGE_FileFormats_misc::TextOutput& out, WorldData& FileData, unsigned int format_version)
{
    (void)out;
    FileData.meta.RecentFormat = WorldData::SMBX38A;
    FileData.meta.RecentFormatVersion = format_version;

#define layerNotDef(lr) ( ((lr) != "Default") ? PGE_URLENC(lr) : "" )

    out << "SMBXFile" << fromNum(FileData.meta.RecentFormatVersion) << "\n";


    out << "WS1";
    out << "|" << PGE_URLENC(FileData.EpisodeTitle);
    out << "|"
        << fromNum((int)FileData.nocharacter1)
        << ","
        << fromNum((int)FileData.nocharacter2)
        << ","
        << fromNum((int)FileData.nocharacter3)
        << ","
        << fromNum((int)FileData.nocharacter4)
        << ","
        << fromNum((int)FileData.nocharacter5);

    out << "|" << PGE_URLENC(FileData.IntroLevel_file);
    if(format_version >= 67)
        out << "," << PGE_URLENC(FileData.GameOverLevel_file);

    out << "|" << fromNum((int)FileData.restrictSinglePlayer)
        << "," << fromNum((int)FileData.HubStyledWorld)
        << "," << fromNum((int)FileData.restartlevel)
        << "," << fromNum((int)FileData.restrictCharacterSwitch)
        << "," << fromNum((int)FileData.restrictSecureGameSave)
        << "," << fromNum(FileData.saveResumePolicy)
        << "," << fromNum((int)FileData.saveAuto)
        << "," << fromNum((int)FileData.showEverything);
    if(format_version >= 68)
        out << "," << fromNum((int)FileData.disableEnterScreen);

    out << "|" << fromNum(FileData.stars)
        << "," << fromNum(FileData.inventoryLimit);

    out << "|" << fromNum((int)FileData.cheatsPolicy);
    out << "|" << fromNum((int)FileData.saveLocker);

    out << "\n";


    out << "WS2";
    out << "|";
    if(IsEmpty(FileData.authors))
    {
        PGESTRING out_authors;

        if(!IsEmpty(FileData.author1))
            out_authors += FileData.author1;

        if(!IsEmpty(FileData.author2))
        {
            if(IsEmpty(out_authors))
                out_authors += "\r\n";
            out_authors += FileData.author2;
        }

        if(!IsEmpty(FileData.author3))
        {
            if(IsEmpty(out_authors))
                out_authors += "\r\n";
            out_authors += FileData.author3;
        }

        if(!IsEmpty(FileData.author4))
        {
            if(IsEmpty(out_authors))
                out_authors += "\r\n";
            out_authors += FileData.author4;
        }

        if(!IsEmpty(FileData.author5))
        {
            if(IsEmpty(out_authors))
                out_authors += "\r\n";
            out_authors += FileData.author5;
        }

        if(!IsEmpty(out_authors))
        {
            out << "#DEFT#";
            out << PGE_BASE64ENC_nopad(out_authors);
        }
    }
    else
    {
        out << "#CUST#" << PGE_BASE64ENC_nopad(FileData.authors);
    }

    if(format_version >= 69)
        out << "|" << PGE_URLENC(FileData.authors_music);

    out << "\n";



    out << "WS3";
    out << "|";
    if(!FileData.cheatsList.empty())
    {
        PGESTRING cheatsList;
        for(auto &cheat : FileData.cheatsList)
        {
            if(IsEmpty(cheatsList))
                cheatsList += ",";
            cheatsList += cheat;
        }
        out << PGE_URLENC(cheatsList);
    }
    out << "\n";


    out << "WS4"
        << "|" << PGE_URLENC(FileData.saveLockerEx)
        << "|" << PGE_URLENC(FileData.saveLockerMsg)
        << "\n";




    for(auto &tile : FileData.tiles)
    {
        out << "T";
        out << "|" << fromNum(tile.id);

        if(format_version >= 67 && (tile.gfx_dx > 0 || tile.gfx_dy > 0))
        {
            out << "," << fromNum(tile.gfx_dx);
            out << "," << fromNum(tile.gfx_dy);
        }

        out << "|" << fromNum(tile.x);
        out << "|" << fromNum(tile.y);
        if(format_version >= 66)
            out << "|" << layerNotDef(tile.layer);
        out << "\n";
    }



    for(auto &scene : FileData.scenery)
    {
        out << "S";
        out << "|" << fromNum(scene.id);

        if(format_version >= 67 && (scene.gfx_dx > 0 || scene.gfx_dy > 0))
        {
            out << "," << fromNum(scene.gfx_dx);
            out << "," << fromNum(scene.gfx_dy);
        }

        out << "|" << fromNum(scene.x);
        out << "|" << fromNum(scene.y);
        if(format_version >= 66)
            out << "|" << layerNotDef(scene.layer);
        out << "\n";
    }


    for(auto &path : FileData.paths)
    {
        out << "P";
        out << "|" << fromNum(path.id);

        if(format_version >= 67 && (path.gfx_dx > 0 || path.gfx_dy > 0))
        {
            out << "," << fromNum(path.gfx_dx);
            out << "," << fromNum(path.gfx_dy);
        }

        out << "|" << fromNum(path.x);
        out << "|" << fromNum(path.y);
        if(format_version >= 66)
            out << "|" << layerNotDef(path.layer);
        out << "\n";
    }


    for(auto &mus : FileData.music)
    {
        out << "M";
        out << "|" << fromNum(mus.id);
        out << "|" << fromNum(mus.x);
        out << "|" << fromNum(mus.y);
        out << "|" << PGE_URLENC(mus.music_file);
        if(format_version >= 66)
        {
            out << "|" << layerNotDef(mus.layer);
            out << "|32|32|1|,0"; // Dummy line
        }
        out << "\n";
    }

    for(auto &mus : FileData.arearects)
    {
        out << "M";
        out << "|" << fromNum(mus.music_id);
        out << "|" << fromNum(mus.x);
        out << "|" << fromNum(mus.y);
        out << "|" << PGE_URLENC(mus.music_file);

        if(format_version >= 66)
        {
            out << "|" << layerNotDef(mus.layer);
            out << "|" << fromNum(mus.w);
            out << "|" << fromNum(mus.h);

            uint32_t flags = mus.flags;
            if(format_version < 69) // Unsupported flag
                flags &= ~WorldAreaRect::SETUP_AUTO_WALKING;

            out << "|" << fromNum(flags);
            out << "|" << PGE_URLENC(mus.eventTouch)
                << "," << fromNum(mus.eventTouchPolicy);
            if(!IsEmpty(mus.eventBreak) ||
               !IsEmpty(mus.eventWarp) ||
               !IsEmpty(mus.eventAnchor))
            {
                out << "|" << PGE_URLENC(mus.eventBreak)
                    << "," << PGE_URLENC(mus.eventWarp)
                    << "," << PGE_URLENC(mus.eventAnchor);
            }
        }
        out << "\n";
    }



    for(auto &level : FileData.levels)
    {
        out << "L";
        out << "|" << fromNum(level.id);

        if(format_version >= 67 && (level.gfx_dx > 0 || level.gfx_dy > 0))
        {
            out << "," << fromNum(level.gfx_dx);
            out << "," << fromNum(level.gfx_dy);
        }

        out << "|" << fromNum(level.x);
        out << "|" << fromNum(level.y);
        out << "|" << PGE_URLENC(level.lvlfile);
        out << "|" << PGE_URLENC(level.title);

        out << "|" << fromNum(level.top_exit);
        out << "," << fromNum(level.top_exit_extra.exit_codes.size() > 0 ? level.top_exit_extra.exit_codes[0] : 0);
        out << "," << fromNum(level.top_exit_extra.exit_codes.size() > 1 ? level.top_exit_extra.exit_codes[1] : 0);
        out << "," << PGE_URLENC(level.top_exit_extra.expression);

        out << "\\" << fromNum(level.left_exit);
        out << "," << fromNum(level.left_exit_extra.exit_codes.size() > 0 ? level.left_exit_extra.exit_codes[0] : 0);
        out << "," << fromNum(level.left_exit_extra.exit_codes.size() > 1 ? level.left_exit_extra.exit_codes[1] : 0);
        out << "," << PGE_URLENC(level.left_exit_extra.expression);

        out << "\\" << fromNum(level.bottom_exit);
        out << "," << fromNum(level.bottom_exit_extra.exit_codes.size() > 0 ? level.bottom_exit_extra.exit_codes[0] : 0);
        out << "," << fromNum(level.bottom_exit_extra.exit_codes.size() > 1 ? level.bottom_exit_extra.exit_codes[1] : 0);
        out << "," << PGE_URLENC(level.bottom_exit_extra.expression);

        out << "\\" << fromNum(level.right_exit);
        out << "," << fromNum(level.right_exit_extra.exit_codes.size() > 0 ? level.right_exit_extra.exit_codes[0] : 0);
        out << "," << fromNum(level.right_exit_extra.exit_codes.size() > 1 ? level.right_exit_extra.exit_codes[1] : 0);
        out << "," << PGE_URLENC(level.right_exit_extra.expression);

        out << "|" << fromNum(level.gotox);
        out << "|" << fromNum(level.gotoy);
        out << "|" << fromNum(level.entertowarp);

        out << "|" << fromNum((int)level.bigpathbg);
        out << "," << fromNum((int)level.pathbg);
        out << "," << fromNum((int)level.alwaysVisible);
        out << "," << fromNum((int)level.gamestart);
        out << "," << fromNum((int)level.forceStart);
        out << "," << fromNum((int)level.disableStarCoinsCount);
        out << "," << fromNum((int)level.destroyOnCompleting);
        out << "," << fromNum(level.levelID);
        out << "," << fromNum((int)level.controlledByAreaRects);

        bool first = true;

        out << "|";
        for(auto &cond : level.enter_cond)
        {
            if(first)
                first = false;
            else
                out << "/";

            out << PGE_URLENC(cond.condition) << "," << PGE_URLENC(cond.levelIndex);
        }

        if(format_version >= 66)
        {
            out << "|" << layerNotDef(level.layer);
            out << "|";
            first = true;
            for(auto &node : level.movement.nodes)
            {
                if(first)
                    first = false;
                else
                    out << ":";

                out << fromNum(node.x) << "," << fromNum(node.y) << "," << fromNum(node.chance);
            }

            out << "\\";
            for(auto &path : level.movement.paths)
            {
                if(first)
                    first = false;
                else
                    out << ":";

                out << fromNum(path.node1) << "," << fromNum(path.node2);
            }
        }

        out << "\n";
    }


    for(auto &layer : FileData.layers)
    {
        out << "WL";
        out << "|" << PGE_URLENC(layer.name);
        out << "|" << fromNum((int)layer.hidden);
        out << "\n";
    }



    for(auto &event : FileData.events38A)
    {
        out << "WE";
        out << "|" << PGE_URLENC(event.name);

        int way = 0;
        way += event.nosmoke ? 1 : 0;
        way += event.layers_mode == WorldEvent38A::LM_StateLayers ? 20 : 0;

        out << "|" << fromNum(way);

        bool first = true;

        out << "/";
        for(auto &layer : event.layers_hide)
        {
            if(first)
                first = false;
            else
                out << ",";

            out << PGE_URLENC(layer);
        }

        first = true;
        out << "/";

        for(auto &layer : event.layers_show)
        {
            if(first)
                first = false;
            else
                out << ",";

            out << PGE_URLENC(layer);
        }

        first = true;
        out << "/";

        for(auto &layer : event.layers_toggle)
        {
            if(first)
                first = false;
            else
                out << ",";

            out << PGE_URLENC(layer);
        }

        out << "|";

        first = true;
        for(auto &mv : event.layers_move)
        {
            if(first)
                first = false;
            else
                out << "\\";

            //Convert all floats into strings if expression fields are empty
            PGESTRING expression_ph = mv.expression_param_h;
            PGESTRING expression_pv = mv.expression_param_v;
            PGESTRING expression_pe = mv.expression_param_extra;
            SMBX38A_Num2Exp_URLEN(mv.param_h, expression_ph);
            SMBX38A_Num2Exp_URLEN(mv.param_v, expression_pv);
            SMBX38A_Num2Exp_URLEN(mv.param_extra, expression_pe);
            out << fromNum(mv.type);
            out << "," << PGE_URLENC(mv.layer);
            out << "," << expression_ph;
            out << "," << expression_pv;
            out << "," << expression_pe;
        }

        out << "|" << fromNum((int)event.autostart);
        out << "/" << fromNum((int)event.start_on_condition);
        out << "," << fromNum((int)event.is_level_enter_exit);
        out << "," << fromNum((int)event.interrupt_on_false);
        out << "," << fromNum((int)event.show_msg_on_interrupt);
        out << "," << PGE_URLENC(event.autostart_condition);
        out << "," << PGE_URLENC(event.interrupt_message);

        out << "|" << fromNum(event.sound_id);
        out << "/";
        if(event.lock_keyboard_delay_ms != 0 && event.lock_keyboard_delay == 0)
            out << fromNum((int)(event.lock_keyboard_delay_ms * 1000 / 65.0));
        else
            out << fromNum(event.lock_keyboard_delay);

        out << "/" << PGE_URLENC(event.trigger);
        out << "," << fromNum(event.trigger_timer);
        out << "/" << PGE_URLENC(event.trigger_script);
        out << "/" << PGE_URLENC(event.msg);
        out << "/" << fromNum(event.move_to_x);
        out << "," << fromNum(event.move_to_y);
        out << "," << fromNum(event.level_anchor_id);

        out << "\n";
    }



    //next line: Custom block
    if(format_version >= 67)
    {
        for(const WorldItemSetup38A &is : FileData.custom38A_configs)
        {
            //    V|name|value
            switch(is.type)
            {
            case WorldItemSetup38A::TERRAIN:
                out << "WCT";
                break;
            case WorldItemSetup38A::SCENERY:
                out << "WCS";
                break;
            case WorldItemSetup38A::LEVEL:
                out << "WCL";
                break;
            default:
                out << "CUnk";
                break;
            }
            //    id = object id
            out << "|" << fromNum(is.id);

            for(pge_size_t j = 0; j < is.data.size(); j++)
            {
                const WorldItemSetup38A::Entry &e = is.data[j];
                out << ((j == 0) ? "|" : ",");
                out << SMBX38A_CC_encode(e.key, e.value);
            }
            out << "\n";
        }
    }


    for(auto &l : FileData.unsupported_38a_lines)
    {
        out << l;
        out << "\n";
    }

    return true;
}

