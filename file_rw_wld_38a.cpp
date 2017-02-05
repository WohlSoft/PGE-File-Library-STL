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
            else if(identifier == "ws4")
            {
                dataReader.ReadDataLine(CSVDiscard(),
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
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(newest_file_format) + " format\n"
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
    SMBX38A_FileBeginN();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();

    CreateWorldData(FileData);

    FileData.meta.RecentFormat = WorldData::SMBX38A;

    #if !defined(_MSC_VER) || _MSC_VER > 1800
    FileData.EpisodeTitle = "" ;
    FileData.stars = 0;
    FileData.CurSection = 0;
    FileData.playmusic = 0;

    //Enable strict mode for SMBX LVL file format
    FileData.meta.smbx64strict = false;

    //Begin all ArrayID's here;
    FileData.tile_array_id = 1;
    FileData.scene_array_id = 1;
    FileData.path_array_id = 1;
    FileData.level_array_id = 1;
    FileData.musicbox_array_id = 1;

    WorldTerrainTile    tile;
    WorldScenery        scen;
    WorldPathTile       pathitem;
    WorldLevelTile      lvlitem;
    WorldMusicBox       musicbox;
    WorldLayer          layer;

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

        while(!in.eof())
        {
            PGESTRING identifier = dataReader.ReadField<PGESTRING>(1);

            if(identifier == "WS1")
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
                                                        MakeCSVOptional(&FileData.GameOverLevel_file, "", nullptr, PGEUrlDecodeFunc)
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
            else if(identifier == "WS2")
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
            else if(identifier == "WS3")
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
            else if(identifier == "WS4")
            {
                dataReader.ReadDataLine(CSVDiscard(),
                                        //    se=save locker syntax[***urlencode!***][syntax]
                                        MakeCSVPostProcessor(&FileData.saveLockerEx, PGEUrlDecodeFunc),
                                        //    msg=message when save was locked[***urlencode!***]
                                        MakeCSVPostProcessor(&FileData.saveLockerMsg, PGEUrlDecodeFunc)
                                        );
            }
            else if(identifier == "T")
            {
                tile = WorldTerrainTile();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &tile.id,
                                                         MakeCSVOptional(&tile.gfx_dx, 0),
                                                         MakeCSVOptional(&tile.gfx_dy, 0)
                                                         ),
                                        &tile.x,
                                        &tile.y,
                                        MakeCSVPostProcessor(&tile.layer, PGELayerOrDefault)
                                        );
                tile.meta.array_id = FileData.tile_array_id++;
                FileData.tiles.push_back(tile);
            }
            else if(identifier == "S")
            {
                scen = WorldScenery();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &scen.id,
                                                         MakeCSVOptional(&scen.gfx_dx, 0),
                                                         MakeCSVOptional(&scen.gfx_dy, 0)
                                                         ),
                                        &scen.x,
                                        &scen.y,
                                        MakeCSVPostProcessor(&scen.layer, PGELayerOrDefault)
                                        );
                scen.meta.array_id = FileData.scene_array_id++;
                FileData.scenery.push_back(scen);
            }
            else if(identifier == "P")
            {
                pathitem = WorldPathTile();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &pathitem.id,
                                                         MakeCSVOptional(&pathitem.gfx_dx, 0),
                                                         MakeCSVOptional(&pathitem.gfx_dy, 0)
                                                         ),
                                        &pathitem.x,
                                        &pathitem.y,
                                        MakeCSVPostProcessor(&pathitem.layer, PGELayerOrDefault)
                                        );
                pathitem.meta.array_id = FileData.path_array_id++;
                FileData.paths.push_back(pathitem);
            }
            else if(identifier == "M")
            {
                musicbox = WorldMusicBox();
                long w = 0;
                long h = 0;
                long flags = 0;
                PGESTRING eventTouch = 0;
                long eflag = 0;
                PGESTRING eventHammer = 0;
                PGESTRING eventWarpWhistle = 0;
                PGESTRING eventAnchor = 0;
                //M|10|416|1312|    |     |32|32|1   |,0
                //M|id|x  |y   |name|layer|w |h |flag|te,eflag|ie1,ie2,ie3
                dataReader.ReadDataLine(CSVDiscard(),
                                        //id=music id
                                        &musicbox.id,
                                        //x=Area position x
                                        &musicbox.x,
                                        //y=Area position y
                                        &musicbox.y,
                                        //name=custom music name[***urlencode!***]
                                        MakeCSVPostProcessor(&musicbox.music_file, PGEUrlDecodeFunc),
                                        //layer=layer name["" == "Default"][***urlencode!***]
                                        MakeCSVOptional(&musicbox.layer, "Default", nullptr, PGELayerOrDefault),
                                        //w=width
                                        MakeCSVOptional(&w, 0),
                                        //h=height
                                        MakeCSVOptional(&h, 0),
                                        //flag=area settings[***Bitwise operation***]
                                        //    0=False !0=True
                                        //    b1=(flag & 1) World Music
                                        //    b2=(flag & 2) Set Viewport
                                        //    b3=(flag & 4) Ship Route
                                        //    b4=(flag & 8) Forced Walking
                                        //    b5=(flag & 16) Item-triggered events
                                        MakeCSVOptional(&flags, 0),
                                        //FIXME: both subreaders must be optional!
                                        MakeCSVSubReader(dataReader, ',',
                                                         //te:Touch Event[***urlencode!***]
                                                         MakeCSVOptional(&eventTouch, "", nullptr, PGELayerOrDefault),
                                                         //eflag:    0=Triggered every time entering
                                                         //          1=Triggered on entrance and level completion
                                                         //          2=Triggered only once
                                                         MakeCSVOptional(&eflag, 0)
                                                         ),
                                        MakeCSVSubReader(dataReader, ',',
                                                         //ie1=Hammer Event[***urlencode!***]
                                                         MakeCSVOptional(&eventHammer, "", nullptr, PGELayerOrDefault),
                                                         //ie2=Warp Whistle Event[***urlencode!***]
                                                         MakeCSVOptional(&eventWarpWhistle, "", nullptr, PGELayerOrDefault),
                                                         //ie3=Anchor Event[***urlencode!***]
                                                         MakeCSVOptional(&eventAnchor, "", nullptr, PGELayerOrDefault)
                                                         )
                                        );
                //TODO: Implement the structure of separated "AREA" type
                //to let special areas be independent from music boxes item types
                musicbox.meta.array_id = FileData.musicbox_array_id++;
                FileData.music.push_back(musicbox);
            }
            else if(identifier == "L")
            {
                //L|id[,dx,dy]|x|y|fn|n|eu\el\ed\er|wx|wy|wlz|bg,pb,av,ls,f,nsc,otl,li,lcm|s|Layer|Lmt
                lvlitem = WorldLevelTile();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
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
                                        //eu,el,ed,er=e[up,left,down,right]
                                        //        e=c1,c2,c3,c4
                                        //        c1,c2,c3=level exit type
                                        //        c4=condidtion expression[***urlencode!***][syntax]
                                        //        exit = (c1 || c2 || c3) && c4
                                        MakeCSVSubReader(dataReader, '\\'),
                                        //wx=go to world map position x
                                        &lvlitem.gotox,
                                        //wx=go to world map position y
                                        &lvlitem.gotoy,
                                        //wlz=nunber of doors to warp
                                        &lvlitem.entertowarp,
                                        MakeCSVSubReader(dataReader, ',',
                                                         //bg=big background
                                                         MakeCSVOptional(&lvlitem.bigpathbg, false),
                                                         //pb=path background
                                                         MakeCSVOptional(&lvlitem.pathbg, false),
                                                         //av=always visible
                                                         MakeCSVOptional(&lvlitem.alwaysVisible, false),
                                                         //ls=is game start point
                                                         MakeCSVOptional(&lvlitem.gamestart, false),
                                                         //TODO: Implement missing fields and handle them!
                                                         //f=forced
                                                         CSVDiscard(),
                                                         //nsc=no star coin count
                                                         CSVDiscard(),
                                                         //otl=destory after clear
                                                         CSVDiscard(),
                                                         //li=level ID
                                                         &lvlitem.id,
                                                         //lcm=Affected by Music Box
                                                         CSVDiscard()
                                                         ),
                                        //s=entrance syntax
                                        //        s=ds1/ds2...dsn
                                        //        ds=ds1,ds2[***urlencode!***][syntax]
                                        //        ds1=condidtion expression
                                        //        ds2=index
                                        CSVDiscard(),
                                        //layer=layer name["" == "Default"][***urlencode!***]
                                        MakeCSVOptional(&lvlitem.layer, "Default", nullptr, PGELayerOrDefault)
                                        //Lmt=Level Movement Command
                                        //    lmt=NodeInfo\PathInfo
                                        //        NodeInfo=Node1:Node2:...:NodeN
                                        //            Node=x,y,chance
                                        //        PathInfo=Path1:Path2:...:PathN
                                        //            Path=NodeID1,NodeID2
                                        );
                lvlitem.meta.array_id = FileData.level_array_id++;
                FileData.levels.push_back(lvlitem);
            }
            else if(identifier == "WL")
            {
                layer = WorldLayer();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&layer.name, PGELayerOrDefault),
                                        &layer.hidden
                                        );
                layer.meta.array_id = FileData.layers_array_id++;
                FileData.layers.push_back(layer);
            }
            else if(identifier == "WE")
            {
            //next line: events
            //    WE|name|layer|layerm|world|other
            //    name=event name[***urlencode!***]
            //    layer=way/hidelist/showlist/togglelist
            //        list=name1,name2,name3...namen
            //            name[***urlencode!***]
            //        if (way % 10 == 1) nosmoke = true;
            //        if (way > 10) object_state = true; else layer_state = true;
            //    layerm=movementcommand1\movementcommand2\...\movementcommandn
            //        movementcommand=way,layer,hp,vp,ap
            //            way:0=speed,1=coordinate,2=moveto,4=spin
            //            layer=layer name[***urlencode!***]
            //            hp=Horizontal Parameter[***urlencode!***]
            //            vp=Vertical Parameter[***urlencode!***]
            //            ap=Additional Parameter[***urlencode!***]
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
                dataReader.ReadDataLine();//TODO! Implement world map events handling
            }
            //TODO: Implement custom configs storage and hanlde the custom configs too!
            //custom object data:
            //    WCT|id|data	:custom tile
            //    WCS|id|data	:custom scene
            //    WCL|id|data	:custom level
            //    id=object id
            //    data=[HEX]value|[HEX]value...
            //    [HEX]=0001	:gfxwidth
            //    [HEX]=0002	:gfxheight
            //    [HEX]=0003	:frames
            else
                dataReader.ReadDataLine();
        }//while is not EOF
    }
    catch(const std::exception &err)
    {
        // First we try to extract the line number out of the nested exception.
        const std::exception *curErr = &err;
        const std::nested_exception *possibleNestedException = dynamic_cast<const std::nested_exception *>(curErr);

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
            {
                // Do Nothing
            }
        }

        // Now fill in the error data.
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(newest_file_format) + " format\n"
                                   "Caused by: \n" + PGESTRING(exception_to_pretty_string(err).c_str());

        // If we were unable to find error line number from the exception, then get the line number from the file reader.
        if(FileData.meta.ERROR_linenum == 0)
            FileData.meta.ERROR_linenum = in.getCurrentLineNumber();

        FileData.meta.ERROR_linedata = "";
        return false;
    }

    FileData.CurSection = 0;
    FileData.playmusic = 0;
    FileData.meta.ReadFileValid = true;
    return true;
    #else
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Unsupported on MSVC2013";
    return false;
    #endif
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
    (void)out;
    errorString = "Not implemented yet. Comming soon!";
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linedata = "";
    FileData.meta.ERROR_linenum = -1;
    FileData.meta.ReadFileValid = false;
    return false;
}

