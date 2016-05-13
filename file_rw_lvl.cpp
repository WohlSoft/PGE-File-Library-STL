/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "smbx64.h"
#include "smbx64_macro.h"
#include "CSVUtils.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
struct LevelEvent_layers
{
    PGESTRING hide;
    PGESTRING show;
    PGESTRING toggle;
};

bool FileFormats::ReadSMBX64LvlFileHeader(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    CreateLevelHeader(FileData);

    FileData.RecentFormat = LevelData::SMBX64;
    FileData.RecentFormatVersion = 64;

    PGE_FileFormats_misc::TextFileInput inf;
    if(!inf.open(filePath, false))
    {
        FileData.ReadFileValid=false;
        return false;
    }
    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.filename = in_1.basename();
    FileData.path = in_1.dirpath();

    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    SMBX64_FileBegin();
    #define nextLineH() line = inf.readCVSLine();

    try
    {
        nextLineH();                                    //Read first line
        SMBX64::ReadUInt(&file_format, line);           //File format number
        FileData.RecentFormatVersion = file_format;

        if(file_format >= 17)
        {
            nextLineH();                                //Read second Line
            SMBX64::ReadUInt(&FileData.stars, line);    //Number of stars
        } else FileData.stars=0;

        if(file_format >= 60)
        {
            nextLineH();   //Read third line
            SMBX64::ReadStr(&FileData.LevelName, line); //LevelTitle
        } else FileData.LevelName = "";

        FileData.CurSection=0;
        FileData.playmusic=0;

        FileData.ReadFileValid=true;
        return true;
    }
    catch(const std::exception& err)
    {
        if( file_format > 0 )
            FileData.ERROR_info = "Detected file format: SMBX-" + fromNum( file_format ) + " is invalid\n";
        else
            FileData.ERROR_info = "It is not an SMBX level file\n";
        #ifdef PGE_FILES_QT
        FileData.ERROR_info += QString::fromStdString( exception_to_pretty_string(err) );
        #else
        FileData.ERROR_info += exception_to_pretty_string(err);
        #endif
        FileData.ERROR_linenum = inf.getCurrentLineNumber();
        FileData.ERROR_linedata = line;
        FileData.ReadFileValid = false;
        return false;
    }
}

bool FileFormats::ReadSMBX64LvlFileF(PGESTRING  filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath, false))
    {
        errorString="Failed to open file for read";
        FileData.ERROR_info = errorString;
        FileData.ERROR_linedata = "";
        FileData.ERROR_linenum = -1;
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadSMBX64LvlFile(file, FileData);
}

bool FileFormats::ReadSMBX64LvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;
    if(!file.open(&rawdata, filePath))
    {
        errorString = "Failed to open raw string for read";
        FileData.ERROR_info = errorString;
        FileData.ERROR_linedata = "";
        FileData.ERROR_linenum = -1;
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadSMBX64LvlFile(file, FileData);
}

bool FileFormats::ReadSMBX64LvlFile(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    SMBX64_FileBegin();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    //SMBX64_File( RawData );

    int i;                  //counters
    CreateLevelData(FileData);
    FileData.RecentFormat=LevelData::SMBX64;
    FileData.RecentFormatVersion = 64;
    FileData.LevelName="";
    FileData.stars=0;
    FileData.CurSection=0;
    FileData.playmusic=0;

    //Enable strict mode for SMBX LVL file format
    FileData.smbx64strict = true;

    //Begin all ArrayID's here;
    FileData.blocks_array_id = 1;
    FileData.bgo_array_id = 1;
    FileData.npc_array_id = 1;
    FileData.doors_array_id = 1;
    FileData.physenv_array_id = 1;
    FileData.layers_array_id = 1;
    FileData.events_array_id = 1;

    FileData.layers.clear();
    FileData.events.clear();

    LevelSection section;
    int sct;
    PlayerPoint players;
    LevelBlock blocks;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoor doors;
    LevelPhysEnv waters;
    LevelLayer layers;
    LevelSMBX64Event events;
    LevelEvent_layers events_layers;
    LevelEvent_Sets events_sets;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.filename = in_1.basename();
        FileData.path = in_1.dirpath();
    }

    #ifdef PGE_EDITOR
    FileData.metaData.script.reset(); //set NULL to pointers in the Meta
    #endif

    try
    {
        ///////////////////////////////////////Begin file///////////////////////////////////////
        nextLine();   //Read first line
        SMBX64::ReadUInt(&file_format, line);//File format number

        FileData.RecentFormatVersion = file_format;

        if(ge(17))
        {
            nextLine(); SMBX64::ReadUInt(&FileData.stars, line); //Number of stars
        } else FileData.stars=0;

        if(ge(60)) { nextLine(); SMBX64::ReadStr(&FileData.LevelName, line);} //LevelTitle

        //total sections
        sct = (ge(8) ? 21 : 6);


        ////////////SECTION Data//////////
        for(i=0;i<sct;i++)
        {
            section = CreateLvlSection();

            nextLine(); SMBX64::ReadSIntFromFloat(&section.size_left, line);
            nextLine(); SMBX64::ReadSIntFromFloat(&section.size_top,  line);
            nextLine(); SMBX64::ReadSIntFromFloat(&section.size_bottom, line); //bottom
            nextLine(); SMBX64::ReadSIntFromFloat(&section.size_right, line);  //right

            nextLine(); SMBX64::ReadUInt(&section.music_id, line);    //Music ID
            nextLine(); SMBX64::ReadUInt(&section.bgcolor, line);     //BG Color
            nextLine(); SMBX64::ReadCSVBool(&section.wrap_h, line);     //Connect sides of section
            nextLine(); SMBX64::ReadCSVBool(&section.OffScreenEn, line);//Offscreen exit
            nextLine(); SMBX64::ReadUInt(&section.background, line);  //BackGround id
            if(ge(1)) {nextLine(); SMBX64::ReadCSVBool(&section.lock_left_scroll, line);} //Don't walk to left (no turn back)
            if(ge(30)){nextLine(); SMBX64::ReadCSVBool(&section.underwater, line);}//Underwater
            if(ge(2)) {nextLine(); SMBX64::ReadStr(&section.music_file, line);}//Custom Music

            //Very important data! I'ts a camera position in the editor!
            section.PositionX=section.size_left-10; //left
            section.PositionY=section.size_top-10; //top

            section.id = i;
            if(i < (signed)FileData.sections.size())
                FileData.sections[i]=section;//Replace if already exists
            else
                FileData.sections.push_back(section); //Add Section in main array
        }

        if(lt(8))
            for(;i<21;i++)
            {
                section = CreateLvlSection();
                section.id=i;
                if(i < (signed)FileData.sections.size())
                    FileData.sections[i]=section;//Replace if already exists
                else
                    FileData.sections.push_back(section); //Add Section in main array
            }

        //Player's point config
        for(i=0;i<2;i++)
        {
            players=CreateLvlPlayerPoint();

            nextLine(); SMBX64::ReadSIntFromFloat(&players.x, line);//Player x
            nextLine(); SMBX64::ReadSIntFromFloat(&players.y, line);//Player y
            nextLine(); SMBX64::ReadUInt(&players.w, line);//Player w
            nextLine(); SMBX64::ReadUInt(&players.h, line);//Player h

            players.id = i+1;

            if(players.x!=0 && players.y!=0 && players.w !=0 && players.h != 0) //Don't add into array non-exist point
                FileData.players.push_back(players);    //Add player in array
        }

        ////////////Block Data//////////
        nextLine();
        while(line!="next")
        {
            blocks = CreateLvlBlock();

                        SMBX64::ReadSIntFromFloat(&blocks.x, line);
            nextLine(); SMBX64::ReadSIntFromFloat(&blocks.y, line);
            nextLine(); SMBX64::ReadSIntFromFloat(&blocks.h, line);
            nextLine(); SMBX64::ReadSIntFromFloat(&blocks.w, line);
            nextLine(); SMBX64::ReadUInt(&blocks.id, line);

            long xnpcID;
            nextLine(); SMBX64::ReadUInt(&xnpcID, line); //Containing NPC id
            {
                //Convert NPC-ID value from SMBX1/2 to SMBX64
                switch(xnpcID)
                {
                    case 100: xnpcID = 1009; break;//Mushroom
                    case 101: xnpcID = 1001; break;//Goomba
                    case 102: xnpcID = 1014; break;//Fire flower
                    case 103: xnpcID = 1034; break;//Super leaf
                    case 104: xnpcID = 1035; break;//Shoe
                    default:break;
                }

                //Convert NPC-ID value from SMBX64 into PGE format
                if(xnpcID != 0)
                {
                    if(xnpcID > 1000)
                        xnpcID = xnpcID-1000;
                    else
                        xnpcID *= -1;
                }
                blocks.npc_id = xnpcID;
            }

            nextLine(); SMBX64::ReadCSVBool(&blocks.invisible, line);
            if(ge(61)) { nextLine(); SMBX64::ReadCSVBool(&blocks.slippery, line); }
            if(ge(10)) { nextLine(); SMBX64::ReadStr(&blocks.layer, line); }
            if(ge(14)) {
                nextLine(); SMBX64::ReadStr(&blocks.event_destroy, line);
                nextLine(); SMBX64::ReadStr(&blocks.event_hit, line);
                nextLine(); SMBX64::ReadStr(&blocks.event_emptylayer, line); }

            blocks.array_id = FileData.blocks_array_id++;
            blocks.index = FileData.blocks.size(); //Apply element index
        FileData.blocks.push_back(blocks); //AddBlock into array

        nextLine();
        }

        ////////////BGO Data//////////
        nextLine();
        while(line!="next")
        {
            bgodata = CreateLvlBgo();
                        SMBX64::ReadSIntFromFloat(&bgodata.x, line);
            nextLine(); SMBX64::ReadSIntFromFloat(&bgodata.y, line);
            nextLine(); SMBX64::ReadUInt(&bgodata.id, line);
            if(ge(10)) { nextLine(); SMBX64::ReadStr(&bgodata.layer, line);}
                bgodata.smbx64_sp = -1;

            if( (file_format < 10) && (bgodata.id==65) ) //set foreground for BGO-65 (SMBX 1.0)
            {
                bgodata.z_mode = LevelBGO::Foreground1;
                bgodata.smbx64_sp = 80;
            }

            bgodata.array_id = FileData.bgo_array_id++;
            bgodata.index = FileData.bgo.size(); //Apply element index

        FileData.bgo.push_back(bgodata); //Add Background object into array

        nextLine();
        }


        ////////////NPC Data//////////
        nextLine();
        while(line!="next")
        {
            npcdata = CreateLvlNpc();

                        SMBX64::ReadSIntFromFloat(&npcdata.x, line);
            nextLine(); SMBX64::ReadSIntFromFloat(&npcdata.y, line);
            nextLine(); SMBX64::ReadSInt(&npcdata.direct, line); //NPC direction
            nextLine(); SMBX64::ReadUInt(&npcdata.id, line); //NPC id

            npcdata.special_data = 0;
            npcdata.contents     = 0;
            switch(npcdata.id)
            {
            //SMBX64 Fixed special options for NPC
            /*parakoopas*/ case 76: case 121: case 122:case 123:case 124: case 161:case 176:case 177:
                /*Paragoomba*/ case 243: case 244:
                /*Cheep-Cheep*/ case 28: case 229: case 230: case 232: case 233: case 234: case 236:
                /*WarpSelection*/ case 288: case 289: /*firebar*/ case 260:
                if(
                        ((npcdata.id!=76)&&(npcdata.id!=28))
                        ||
                        (
                            ((ge(15))&&(npcdata.id==76))
                            ||((ge(31))&&(npcdata.id==28))
                            )
                        )
                {
                    nextLine(); SMBX64::ReadSInt(&npcdata.special_data, line); //NPC special option
                } break;

                /*Containers*/
            case 91: /*buried*/
            case 96: /*egg*/
            case 283:/*Bubble*/
            case 284:/*SMW Lakitu*/
                nextLine(); SMBX64::ReadSInt(&npcdata.contents, line);

                if(npcdata.id==91)
                    switch(npcdata.contents)
                    {
                    /*WarpSelection*/ case 288: /*case 289:*/ /*firebar*/ /*case 260:*/
                        nextLine(); SMBX64::ReadSInt(&npcdata.special_data, line); break;
                    default: break;
                    }

            default: break;
            }

            if(ge(3))
            {
                nextLine(); SMBX64::ReadCSVBool(&npcdata.generator, line); //Generator enabled
                npcdata.generator_direct = 1;
                npcdata.generator_type = 1;

                if(npcdata.generator)
                {
                    nextLine(); SMBX64::ReadSInt(&npcdata.generator_direct, line); //Generator direction (1, 2, 3, 4)
                    if(npcdata.generator_direct<0) npcdata.generator_direct = 1;//Fix of old accidental mistake causes -1 value
                    nextLine(); SMBX64::ReadUInt(&npcdata.generator_type, line);   //Generator type [1] Warp, [2] Projectile
                    nextLine(); SMBX64::ReadUInt(&npcdata.generator_period, line); //Generator period ( sec*10 ) [1-600]
                }
            }

            if(ge(5))
            {
                nextLine();
                //strVarMultiLine(npcdata.msg, line)//Message
                SMBX64::ReadStr(&npcdata.msg, line);//Message
            }

            if(ge(6))
            {
                nextLine(); SMBX64::ReadCSVBool(&npcdata.friendly, line);//Friendly NPC
                nextLine(); SMBX64::ReadCSVBool(&npcdata.nomove, line); //Don't move NPC
            }

            if(ge(9))
            {
                nextLine(); SMBX64::ReadCSVBool(&npcdata.is_boss, line); //Set as boss flag
            }
            else
            {
                switch(npcdata.id)
                {
                //set boss flag to TRUE for old file formats automatically
                case 15: case 39: case 86:
                    npcdata.is_boss=true;
                default: break;
                }
            }

            if(ge(10))
            {
                nextLine(); SMBX64::ReadStr(&npcdata.layer, line);
                nextLine(); SMBX64::ReadStr(&npcdata.event_activate, line);
                nextLine(); SMBX64::ReadStr(&npcdata.event_die, line);
                nextLine(); SMBX64::ReadStr(&npcdata.event_talk, line);
            }

            if(ge(14)) {nextLine(); SMBX64::ReadStr(&npcdata.event_emptylayer, line);} //No more objects in layer event
            if(ge(63)) {nextLine(); SMBX64::ReadStr(&npcdata.attach_layer, line);} //Layer name to attach

            npcdata.array_id = FileData.npc_array_id++;
            npcdata.index = FileData.npc.size(); //Apply element index

            FileData.npc.push_back(npcdata); //Add NPC into array
            nextLine();
        }


        ////////////Warp and Doors Data//////////
        nextLine();
        while( ((line!="next")&&(file_format>=10)) || ((file_format<10)&&(line!="")&&(!in.eof())))
        {
            doors = CreateLvlWarp();
            doors.isSetIn=true;
            doors.isSetOut=true;

                        SMBX64::ReadSIntFromFloat(&doors.ix, line); //Entrance x
            nextLine(); SMBX64::ReadSIntFromFloat(&doors.iy, line); //Entrance y
            nextLine(); SMBX64::ReadSIntFromFloat(&doors.ox, line); //Exit x
            nextLine(); SMBX64::ReadSIntFromFloat(&doors.oy, line); //Exit y

            nextLine(); SMBX64::ReadUInt(&doors.idirect, line); //Entrance direction: [3] down, [1] up, [2] left, [4] right
            nextLine(); SMBX64::ReadUInt(&doors.odirect, line); //Exit direction: [1] down [3] up [4] left [2] right
            nextLine(); SMBX64::ReadUInt(&doors.type, line);    //Door type: [1] pipe, [2] door, [0] instant

            if(ge(3)){ nextLine(); SMBX64::ReadStr(&doors.lname, line);   //Warp to level
                nextLine(); SMBX64::ReadUInt(&doors.warpto, line); //Normal entrance or Warp to other door
                nextLine(); SMBX64::ReadCSVBool(&doors.lvl_i, line); //Level Entrance (cannot enter)
                    doors.isSetIn = ((doors.lvl_i)?false:true);}

            if(ge(4)){ nextLine(); SMBX64::ReadCSVBool(&doors.lvl_o, line);
                      doors.isSetOut = (((doors.lvl_o)?false:true) || (doors.lvl_i));
                    nextLine(); SMBX64::ReadSInt(&doors.world_x, line); //WarpTo X
                    nextLine(); SMBX64::ReadSInt(&doors.world_y, line); //WarpTo y
            }

            if(ge(7)){ nextLine(); SMBX64::ReadUInt(&doors.stars, line);} //Need a stars
            if(ge(12)) { nextLine(); SMBX64::ReadStr(&doors.layer, line); //Layer
                nextLine(); SMBX64::ReadCSVBool(&doors.unknown, line); }    //<unused>, always FALSE

            if(ge(23)) { nextLine(); SMBX64::ReadCSVBool(&doors.novehicles, line); } //Deny vehicles
            if(ge(25)) { nextLine(); SMBX64::ReadCSVBool(&doors.allownpc, line); }   //Allow carried items
            if(ge(26)) { nextLine(); SMBX64::ReadCSVBool(&doors.locked, line); }     //Locked

            doors.array_id = FileData.doors_array_id++;
            doors.index = FileData.doors.size(); //Apply element index

        FileData.doors.push_back(doors); //Add NPC into array
        nextLine();
        }

        ////////////Water/QuickSand Data//////////
        if(file_format>=29)
        {
            nextLine();
            while(line!="next")
            {
                waters = CreateLvlPhysEnv();

                            SMBX64::ReadSIntFromFloat(&waters.x, line);
                nextLine(); SMBX64::ReadSIntFromFloat(&waters.y, line);
                nextLine(); SMBX64::ReadUInt(&waters.w, line);
                nextLine(); SMBX64::ReadUInt(&waters.h, line);
                nextLine(); SMBX64::ReadUInt(&waters.unknown, line); //Unused value
                if(ge(62)) { nextLine(); SMBX64::ReadCSVBool(&waters.env_type, line);}
                nextLine(); SMBX64::ReadStr(&waters.layer, line);

                waters.array_id = FileData.physenv_array_id++;
                waters.index = FileData.physez.size(); //Apply element index

            FileData.physez.push_back(waters); //Add Water area into array
            nextLine();
            }
        }

        if(ge(10)) {
            ////////////Layers Data//////////
            nextLine();
            while((line!="next")&&(!in.eof())&&(!IsEmpty(line)))
            {

                              SMBX64::ReadStr(&layers.name, line);     //Layer name
                nextLine(); SMBX64::ReadCSVBool(&layers.hidden, line); //hidden layer

                layers.locked = false;

                layers.array_id = FileData.layers_array_id++;

            FileData.layers.push_back(layers); //Add Water area into array
            nextLine();
            }

            ////////////Events Data//////////
            nextLine();
            while((line!="")&&(!in.eof())&&(!IsEmpty(line)))
            {
                events = CreateLvlEvent();

                SMBX64::ReadStr(&events.name, line);//Event name

                if(ge(11))
                {
                    nextLine();
                    SMBX64::ReadStr(&events.msg, line);//Event message
                }

                if(ge(14)){ nextLine(); SMBX64::ReadUInt(&events.sound_id, line);}
                if(ge(18)){ nextLine(); SMBX64::ReadUInt(&events.end_game, line);}

                PGELIST<LevelEvent_layers > events_layersArr;
                events_layersArr.clear();

                events.layers_hide.clear();
                events.layers_show.clear();
                events.layers_toggle.clear();

                for(i=0; i<sct; i++)
                {
                    nextLine(); SMBX64::ReadStr(&events_layers.hide, line); //Hide layer
                    nextLine(); SMBX64::ReadStr(&events_layers.show, line); //Show layer
                    if(ge(14)){ nextLine(); SMBX64::ReadStr(&events_layers.toggle, line);//Toggle layer
                    } else events_layers.toggle="";

                    if(events_layers.hide!="") events.layers_hide.push_back(events_layers.hide);
                    if(events_layers.show!="") events.layers_show.push_back(events_layers.show);
                    if(events_layers.toggle!="") events.layers_toggle.push_back(events_layers.toggle);
                    events_layersArr.push_back(events_layers);
                }

                if(ge(13))
                {
                    events.sets.clear();
                    for(i=0; i<21; i++)
                    {
                        events_sets.id = i;
                        nextLine(); SMBX64::ReadSInt(&events_sets.music_id, line);        //Set Music
                        nextLine(); SMBX64::ReadSInt(&events_sets.background_id, line);   //Set Background
                        nextLine(); SMBX64::ReadSInt(&events_sets.position_left, line);   //Set Position to: LEFT
                        nextLine(); SMBX64::ReadSInt(&events_sets.position_top, line);    //Set Position to: TOP
                        nextLine(); SMBX64::ReadSInt(&events_sets.position_bottom, line); //Set Position to: BOTTOM
                        nextLine(); SMBX64::ReadSInt(&events_sets.position_right, line);  //Set Position to: RIGHT
                        events.sets.push_back(events_sets);
                    }
                }

                if(ge(26)) { nextLine(); SMBX64::ReadStr(&events.trigger, line); //Trigger
                             nextLine(); SMBX64::ReadUInt(&events.trigger_timer, line);} //Start trigger event after x [1/10 sec]. Etc. 153,2 sec

                if(ge(27)) { nextLine(); SMBX64::ReadCSVBool(&events.nosmoke, line); }//Don't smoke tobacco, let's healthy! :D

                if(ge(28))
                {
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_altjump, line);//Hold ALT-JUMP player control
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_altrun, line); //ALT-RUN
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_down, line);   //DOWN
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_drop, line);   //DROP
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_jump, line);   //JUMP
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_left, line);   //LEFT
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_right, line);  //RIGHT
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_run, line);    //RUN
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_start, line);  //START
                    nextLine(); SMBX64::ReadCSVBool(&events.ctrl_up, line);  //UP
                    events.ctrls_enable = events.ctrlKeyPressed();
                    events.ctrl_lock_keyboard=events.ctrls_enable;
                }

                if(ge(32))
                {
                    nextLine(); SMBX64::ReadCSVBool(&events.autostart, line);  //Auto start
                    nextLine(); SMBX64::ReadStr(&events.movelayer, line);  //Layer for movement
                    nextLine(); SMBX64::ReadFloat(&events.layer_speed_x, line); //Layer moving speed – horizontal
                    nextLine(); SMBX64::ReadFloat(&events.layer_speed_y, line); //Layer moving speed – vertical
                    if( !IsEmpty(events.movelayer) )
                    {
                        LevelEvent_MoveLayer mvl;
                        mvl.name = events.movelayer;
                        mvl.speed_x = events.layer_speed_x;
                        mvl.speed_y = events.layer_speed_y;
                        events.moving_layers.push_back(mvl);
                    }
                }

                if(ge(33))
                {
                    nextLine(); SMBX64::ReadFloat(&events.move_camera_x, line); //Move screen horizontal speed
                    nextLine(); SMBX64::ReadFloat(&events.move_camera_y, line); //Move screen vertical speed
                    nextLine(); SMBX64::ReadSInt(&events.scroll_section, line); //Scroll section x, (in file value is x-1)
                    if( ((events.move_camera_x!=0.0f)||(events.move_camera_y!=0.0f)) && (events.scroll_section < (signed long)events.sets.size()) )
                    {
                        events.sets[events.scroll_section].autoscrol=true;
                        events.sets[events.scroll_section].autoscrol_x = events.move_camera_x;
                        events.sets[events.scroll_section].autoscrol_y = events.move_camera_y;
                        events.sets[events.scroll_section].expression_autoscrool_x = fromNum(events.move_camera_x);
                        events.sets[events.scroll_section].expression_autoscrool_y = fromNum(events.move_camera_y);
                    }
                }
                events.array_id = FileData.events_array_id++;

            FileData.events.push_back(events);
            nextLine();
            }
        }

        LevelAddInternalEvents(FileData);
        ///////////////////////////////////////EndFile///////////////////////////////////////
        FileData.ReadFileValid=true;
        return true;
    }
    catch(const std::exception& err)
    {
        if( file_format > 0 )
            FileData.ERROR_info = "Detected file format: SMBX-"+fromNum(file_format)+" is invalid\n";
        else
            FileData.ERROR_info = "It is not an SMBX level file\n";
        #ifdef PGE_FILES_QT
        FileData.ERROR_info += QString::fromStdString( exception_to_pretty_string(err) );
        #else
        FileData.ERROR_info += exception_to_pretty_string(err);
        #endif
        FileData.ERROR_linenum  = in.getCurrentLineNumber();
        FileData.ERROR_linedata = line;
        FileData.ReadFileValid = false;
        return false;
    }
}








//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteSMBX64LvlFileF(PGESTRING filePath, LevelData &FileData, int file_format)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Fail to open file for write";
        return false;
    }
    return WriteSMBX64LvlFile(file, FileData, file_format);
}

bool FileFormats::WriteSMBX64LvlFileRaw(LevelData &FileData, PGESTRING &rawdata, int file_format)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Failed to open raw string for write";
        return false;
    }
    return WriteSMBX64LvlFile(file, FileData, file_format);
}

bool FileFormats::WriteSMBX64LvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData &FileData, int file_format)
{
    int i, j;

    //Count placed stars on this level
    FileData.stars=0;
    for(i=0;i<(signed)FileData.npc.size();i++)
    {
        if(FileData.npc[i].is_star)
            FileData.stars++;
    }

    //Sort blocks array by X->Y
    smbx64LevelSortBlocks(FileData);
    //Sort BGO's array by SortPriority->X->Y
    smbx64LevelSortBGOs(FileData);

    //Prevent out of range: 0....64
    if(file_format<0) file_format = 0;
    else
    if(file_format>64) file_format = 64;

    FileData.RecentFormat = LevelData::SMBX64;
    FileData.RecentFormatVersion = file_format;

    out << SMBX64::WriteSInt(file_format);                     //Format version
    if(file_format>=17)
    out << SMBX64::WriteSInt(FileData.stars);         //Number of stars
    if(file_format>=60)
    out << SMBX64::WriteStr(FileData.LevelName);  //Level name

    int s_limit=21;

    if(file_format < 8)
        s_limit=6;
    //qDebug() << "sections "<<s_limit << "format "<<file_format ;

    //Sections settings
    for(i=0; (i<s_limit)&&(i<(signed)FileData.sections.size()) ; i++)
    {
        out << SMBX64::WriteSInt(FileData.sections[i].size_left);
        out << SMBX64::WriteSInt(FileData.sections[i].size_top);
        out << SMBX64::WriteSInt(FileData.sections[i].size_bottom);
        out << SMBX64::WriteSInt(FileData.sections[i].size_right);
        out << SMBX64::WriteSInt(FileData.sections[i].music_id);
        out << SMBX64::WriteSInt(FileData.sections[i].bgcolor);
        out << SMBX64::WriteCSVBool(FileData.sections[i].wrap_h);
        out << SMBX64::WriteCSVBool(FileData.sections[i].OffScreenEn);
        out << SMBX64::WriteSInt(FileData.sections[i].background);
        if(file_format>=1)
        out << SMBX64::WriteCSVBool(FileData.sections[i].lock_left_scroll);
        if(file_format>=30)
        out << SMBX64::WriteCSVBool(FileData.sections[i].underwater);
        if(file_format>=2)
        out << SMBX64::WriteStr(FileData.sections[i].music_file);
    }
    for( ; i<s_limit ; i++) //Protector
    {
        if(file_format<1)
            out << "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n";
        else if(file_format<2)
            out << "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n#FALSE#\n";
        else if(file_format<30)
            out << "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n#FALSE#\n\"\"\n";
        else
            out << "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n#FALSE#\n#FALSE#\n\"\"\n";
    }
        //append dummy section data, if array size is less than 21

    //qDebug() << "i="<< i;

    //Players start point
    int playerpoints=0;
    for(j=1;j<=2 && playerpoints<2;j++)
    {
        bool found=false;
        for(i=0; i<(signed)FileData.players.size(); i++ )
        {
            if(FileData.players[i].id!=(unsigned int)j) continue;
            out << SMBX64::WriteSInt(FileData.players[i].x);
            out << SMBX64::WriteSInt(FileData.players[i].y);
            out << SMBX64::WriteSInt(FileData.players[i].w);
            out << SMBX64::WriteSInt(FileData.players[i].h);
            playerpoints++;found=true;
        }
        if(!found)
        {
            out << "0\n0\n0\n0\n";
            playerpoints++;
        }

    }
    for( ;playerpoints<2; playerpoints++ ) //Protector
        out << "0\n0\n0\n0\n";


    //Blocks
    for(int blkID=0;blkID<(signed)FileData.blocks.size();blkID++)
    {
        LevelBlock &block=FileData.blocks[blkID];
        out << SMBX64::WriteSInt(block.x);
        out << SMBX64::WriteSInt(block.y);
        out << SMBX64::WriteSInt(block.h);
        out << SMBX64::WriteSInt(block.w);
        out << SMBX64::WriteSInt(block.id);
        int npcID = block.npc_id;
        if(npcID < 0)
        {
            npcID *= -1; if(npcID>99) npcID = 99;
        }
        else
        if(npcID!=0)
        {
            npcID+=1000;
            if(file_format < 18)
            {
                long xnpcID = npcID;
                //Convert NPC-ID value from SMBX64 to SMBX1/2
                switch(xnpcID)
                {
                    case 1009://Mushroom
                        xnpcID = 100; break;
                    case 1001://Goomba
                        xnpcID = 101; break;
                    case 1014://Fire flower
                        xnpcID = 102; break;
                    case 1034://Super leaf
                        xnpcID = 103; break;
                    case 1035://Shoe
                        xnpcID = 104; break;
                    default:
                        break;
                }
                npcID = xnpcID;
            }
        }

        out << SMBX64::WriteSInt(npcID);
        out << SMBX64::WriteCSVBool(block.invisible);
        if(file_format>=61)
        out << SMBX64::WriteCSVBool(block.slippery);
        if(file_format>=10)
        out << SMBX64::WriteStr(block.layer);
        if(file_format>=14)
        {
            out << SMBX64::WriteStr(block.event_destroy);
            out << SMBX64::WriteStr(block.event_hit);
            out << SMBX64::WriteStr(block.event_emptylayer);
        }
    }
    out << "\"next\"\n";//Separator


    //BGOs
    for(int bgoID=0;bgoID<(signed)FileData.bgo.size();bgoID++)
    {
        LevelBGO &bgo1=FileData.bgo[bgoID];
        out << SMBX64::WriteSInt( bgo1.x);
        out << SMBX64::WriteSInt( bgo1.y);
        out << SMBX64::WriteSInt( bgo1.id);
        if(file_format>=10)
        out << SMBX64::WriteStr( bgo1.layer);
    }
    out << "\"next\"\n";//Separator

    //NPCs
    for(i=0; i<(signed)FileData.npc.size(); i++)
    {
        //Section size
        out << SMBX64::WriteSInt(FileData.npc[i].x);
        out << SMBX64::WriteSInt(FileData.npc[i].y);
        out << SMBX64::WriteSInt(FileData.npc[i].direct);
        out << SMBX64::WriteSInt(FileData.npc[i].id);

        if(file_format>=10)
        {
            switch(FileData.npc[i].id)
            {
                //SMBX64 Fixed special options for NPC
                /*Items*/
                /*parakoopa*/ case 76: case 121: case 122:case 123:case 124: case 161:case 176:case 177:
                /*paragoomba*/ case 243: case 244:
                /*Cheep-Cheep*/ case 28: case 229: case 230: case 232: case 233: case 234: case 236:
                /*WarpSelection*/ case 288: case 289:
                /*firebar*/ case 260:
                if(
                      ( (FileData.npc[i].id != 76) && (FileData.npc[i].id!=28) ) ||
                      (
                          ((file_format >= 15) && (FileData.npc[i].id==76)) ||
                          ((file_format >= 31) && (FileData.npc[i].id==28))
                      )
                  )
                    out << SMBX64::WriteSInt(FileData.npc[i].special_data);
                break;
                /*Containers*/
                case 283:/*Bubble*/
                case 91: /*buried*/
                case 284:/*SMW Lakitu*/
                case 96: /*egg*/

                out << SMBX64::WriteSInt(FileData.npc[i].contents);

                //Warp Section value for included into herb magic potion
                if( (FileData.npc[i].id==91) && (FileData.npc[i].contents==288) )
                    out << SMBX64::WriteSInt(FileData.npc[i].special_data);

                break;
                default:
                    break;
            }
        }

        if(file_format>=3)
        {
            out << SMBX64::WriteCSVBool(FileData.npc[i].generator);
            if(FileData.npc[i].generator)
            {
                out << SMBX64::WriteSInt(FileData.npc[i].generator_direct);
                out << SMBX64::WriteSInt(FileData.npc[i].generator_type);
                out << SMBX64::WriteSInt(FileData.npc[i].generator_period);
            }
        }

        if(file_format>=5)
        out << SMBX64::WriteStr_multiline(FileData.npc[i].msg);

        if(file_format>=6)
        {
            out << SMBX64::WriteCSVBool(FileData.npc[i].friendly);
            out << SMBX64::WriteCSVBool(FileData.npc[i].nomove);
        }
        if(file_format>=9)
        out << SMBX64::WriteCSVBool(FileData.npc[i].is_boss);

        if(file_format>=10)
        {
            out << SMBX64::WriteStr(FileData.npc[i].layer);
            out << SMBX64::WriteStr(FileData.npc[i].event_activate);
            out << SMBX64::WriteStr(FileData.npc[i].event_die);
            out << SMBX64::WriteStr(FileData.npc[i].event_talk);
        }
        if(file_format>=14)
        out << SMBX64::WriteStr(FileData.npc[i].event_emptylayer);
        if(file_format>=63)
        out << SMBX64::WriteStr(FileData.npc[i].attach_layer);
    }
    out << "\"next\"\n";//Separator

    //Doors
    for(i=0; i<(signed)FileData.doors.size(); i++)
    {
        if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) )
            if(!FileData.doors[i].isSetIn) continue; // Skip broken door
        if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_i)) )
            if(!FileData.doors[i].isSetOut) continue; // Skip broken door

        out << SMBX64::WriteSInt(FileData.doors[i].ix);
        out << SMBX64::WriteSInt(FileData.doors[i].iy);
        out << SMBX64::WriteSInt(FileData.doors[i].ox);
        out << SMBX64::WriteSInt(FileData.doors[i].oy);
        out << SMBX64::WriteSInt(FileData.doors[i].idirect);
        out << SMBX64::WriteSInt(FileData.doors[i].odirect);
        out << SMBX64::WriteSInt(FileData.doors[i].type);
        if(file_format>=3)
        {
            out << SMBX64::WriteStr(FileData.doors[i].lname);
            out << SMBX64::WriteSInt(FileData.doors[i].warpto);
            out << SMBX64::WriteCSVBool(FileData.doors[i].lvl_i);
        }
        if(file_format>=4)
        {
            out << SMBX64::WriteCSVBool(FileData.doors[i].lvl_o);
            out << SMBX64::WriteSInt(FileData.doors[i].world_x);
            out << SMBX64::WriteSInt(FileData.doors[i].world_y);
        }
        if(file_format>=7)
        out << SMBX64::WriteSInt(FileData.doors[i].stars);
        if(file_format>=12)
        {
            out << SMBX64::WriteStr(FileData.doors[i].layer);
            out << SMBX64::WriteCSVBool(FileData.doors[i].unknown);
        }
        if(file_format>=23)
        out << SMBX64::WriteCSVBool(FileData.doors[i].novehicles);
        if(file_format>=25)
        out << SMBX64::WriteCSVBool(FileData.doors[i].allownpc);
        if(file_format>=26)
        out << SMBX64::WriteCSVBool(FileData.doors[i].locked);
    }

    //Water
    if(file_format>=29)
    {
        out << "\"next\"\n";//Separator
        for(i=0; i<(signed)FileData.physez.size(); i++)
        {
            out << SMBX64::WriteSInt(FileData.physez[i].x);
            out << SMBX64::WriteSInt(FileData.physez[i].y);
            out << SMBX64::WriteSInt(FileData.physez[i].w);
            out << SMBX64::WriteSInt(FileData.physez[i].h);
            out << SMBX64::WriteSInt(FileData.physez[i].unknown);
            if(file_format>=62)
            out << SMBX64::WriteCSVBool( FileData.physez[i].env_type != LevelPhysEnv::ENV_WATER );
            out << SMBX64::WriteStr( FileData.physez[i].layer );
        }
    }

    if(file_format>=10)
    {

        out << "\"next\"\n";//Separator

        //Layers
        for(i=0; i<(signed)FileData.layers.size(); i++)
        {
            out << SMBX64::WriteStr(FileData.layers[i].name);
            out << SMBX64::WriteCSVBool(FileData.layers[i].hidden);
        }
        out << "\"next\"\n";//Separator

        LevelEvent_layers layerSet;
        for(i=0; i<(signed)FileData.events.size(); i++)
        {
            out << SMBX64::WriteStr(FileData.events[i].name);
            if(file_format>=11)
            out << SMBX64::WriteStr_multiline(FileData.events[i].msg);
            if(file_format>=14)
            out << SMBX64::WriteSInt(FileData.events[i].sound_id);
            if(file_format>=18)
            out << SMBX64::WriteSInt(FileData.events[i].end_game);

            PGELIST<LevelEvent_layers > events_layersArr;
            //FileData.events[i].layers.clear();
            for(j=0; j<20; j++)
            {
                layerSet.hide =
                        ((j<(signed)FileData.events[i].layers_hide.size())?
                          FileData.events[i].layers_hide[j] : "");
                layerSet.show =
                        ((j<(signed)FileData.events[i].layers_show.size())?
                          FileData.events[i].layers_show[j] : "");;
                layerSet.toggle =
                        ((j<(signed)FileData.events[i].layers_toggle.size())?
                          FileData.events[i].layers_toggle[j] : "");

                events_layersArr.push_back(layerSet);
            }

            for(j=0; j< (signed)events_layersArr.size()  && j<s_limit-1; j++)
            {
                out << SMBX64::WriteStr(events_layersArr[j].hide);
                out << SMBX64::WriteStr(events_layersArr[j].show);
                if(file_format>=14)
                out << SMBX64::WriteStr(events_layersArr[j].toggle);
            }
            for( ; j<s_limit; j++)
            {
                if(file_format>=14)
                out << "\"\"\n\"\"\n\"\"\n"; //(21st element is SMBX 1.3 bug protector)
                else
                out << "\"\"\n\"\"\n";
            }

            if(file_format>=13)
            {
                for(j=0; j< (signed)FileData.events[i].sets.size()  && j<s_limit; j++)
                {
                    out << SMBX64::WriteSInt(FileData.events[i].sets[j].music_id);
                    out << SMBX64::WriteSInt(FileData.events[i].sets[j].background_id);
                    out << SMBX64::WriteSInt(FileData.events[i].sets[j].position_left);
                    out << SMBX64::WriteSInt(FileData.events[i].sets[j].position_top);
                    out << SMBX64::WriteSInt(FileData.events[i].sets[j].position_bottom);
                    out << SMBX64::WriteSInt(FileData.events[i].sets[j].position_right);
                }
                for( ; j<s_limit; j++) // Protector
                    out << "0\n0\n0\n-1\n-1\n-1\n";
            }

            if(file_format>=26){
            out << SMBX64::WriteStr(FileData.events[i].trigger);
            out << SMBX64::WriteSInt(FileData.events[i].trigger_timer);
            }

            if(file_format>=27)
            out << SMBX64::WriteCSVBool(FileData.events[i].nosmoke);

            if(file_format>=28)
            {
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_altjump);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_altrun);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_down);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_drop);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_jump);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_left);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_right);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_run);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_start);
            out << SMBX64::WriteCSVBool(FileData.events[i].ctrl_up);
            }

            if(file_format>=32)
            {
            out << SMBX64::WriteCSVBool( FileData.events[i].autostart>0 );

            out << SMBX64::WriteStr(FileData.events[i].movelayer);
            out << SMBX64::WriteFloat(FileData.events[i].layer_speed_x);
            out << SMBX64::WriteFloat(FileData.events[i].layer_speed_y);
            }
            if(file_format>=33)
            {
            out << SMBX64::WriteFloat(FileData.events[i].move_camera_x);
            out << SMBX64::WriteFloat(FileData.events[i].move_camera_y);
            out << SMBX64::WriteSInt(FileData.events[i].scroll_section);
            }
        }
    }

    return true;
}
