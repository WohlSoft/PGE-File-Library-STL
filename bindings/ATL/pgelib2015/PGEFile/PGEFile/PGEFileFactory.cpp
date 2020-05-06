// PGEFileFactory.cpp: Implementierung von CPGEFileFactory

#include "stdafx.h"
#include "PGEFileFactory.h"
#include "PGELevelData.h"

#include "PGE-File-Library-STL/file_formats.h"
#include "Utils/ComUtils.h"
#include "Utils/PGEStringUtils.h"
#include "ErrorHelper/Error.h"
#include <winerror.h>


// CPGEFileFactory

HRESULT DetectFileFormatLevel(const std::string& fName, PGELevelFormats& formatOut) {
    if (PGEStringUtils::strEndsWith(fName, ".lvl")) {
        formatOut = LVLFORMAT_LVL;
    }
    else if (PGEStringUtils::strEndsWith(fName, ".lvlx")) {
        formatOut = LVLFORMAT_LVLX;
    }
    else {
        CComBSTR errMsg(_T("The filetyp (.lvl or .lvlx) could not be received! The Path is: "));
        errMsg += fName.c_str();
        CError::DispatchError(COR_E_IO, __uuidof(IPGEFileFactory), _T("PGE.FileLib.1"), errMsg, 0, NULL);
        return COR_E_IO;
    }
    return S_OK;
}



STDMETHODIMP CPGEFileFactory::InterfaceSupportsErrorInfo(REFIID riid)
{
    static const IID* const arr[] = 
    {
        &IID_IPGEFileFactory
    };

    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
        if (InlineIsEqualGUID(*arr[i],riid))
            return S_OK;
    }
    return S_FALSE;
}



STDMETHODIMP CPGEFileFactory::CreateNewLevelData(IPGELevelData** pVal)
{
    COMUTILS_RETURN_IF_FAILED_START;
    CComObject<CPGELevelData>* levelDataClass = NULL;
    COMUTILS_RETURN_IF_FAILED(CComObject<CPGELevelData>::CreateInstance(&levelDataClass));
    
    levelDataClass->QueryInterface(pVal);

    return S_OK;
}


STDMETHODIMP CPGEFileFactory::LoadLevelDataByFilename(BSTR filename, PGELevelFormats formatType, IPGELevelData** pVal)
{
    COMUTILS_RETURN_IF_FAILED_START;
    std::string fName = PGEComUtils::ConvertBSTRToMBS(filename);
    PGELevelFormats usedFormat = formatType;
    if (usedFormat == LVLFORMAT_DETECT) {
        COMUTILS_RETURN_IF_FAILED(DetectFileFormatLevel(fName, usedFormat));
    }

    LevelData data;

    
    if(!FileFormats::OpenLevelFile(fName, data))
        return COR_E_IO;

    CComPtr<IPGELevelData> newDataObj = NULL;
    COMUTILS_RETURN_IF_FAILED(PGEComUtils::CreateObjectInternal<CPGELevelData>(&newDataObj));

    CComPtr<IPGELevelBlocks> blocksArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_Blocks(&blocksArray));
    for (const LevelBlock& nextBlock : data.blocks)
    {
        CComPtr<IPGELevelBlock> newBlock = NULL;
        COMUTILS_RETURN_IF_FAILED(blocksArray->Add(&newBlock));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_EventDestroyedName(CComBSTR(nextBlock.event_destroy.c_str())));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_EventHitName(CComBSTR(nextBlock.event_hit.c_str())));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_EventNoMoreObjectInLayerName(CComBSTR(nextBlock.event_emptylayer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_Height(nextBlock.h));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_ID(nextBlock.id));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_Invisible(COMBOOL(nextBlock.invisible)));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_LayerName(CComBSTR(nextBlock.layer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_NPCID(nextBlock.npc_id));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_Slippery(COMBOOL(nextBlock.slippery)));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_Width(nextBlock.w));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_X(nextBlock.x));
        COMUTILS_RETURN_IF_FAILED(newBlock->put_Y(nextBlock.y));
    }

    CComPtr<IPGELevelBGOs> bgoArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_BGOs(&bgoArray));
    for (const LevelBGO& nextBGO : data.bgo)
    {
        CComPtr<IPGELevelBGO> newBGO = NULL;
        COMUTILS_RETURN_IF_FAILED(bgoArray->Add(&newBGO));
        COMUTILS_RETURN_IF_FAILED(newBGO->put_ID(nextBGO.id));
        COMUTILS_RETURN_IF_FAILED(newBGO->put_LayerName(CComBSTR(nextBGO.layer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newBGO->put_X(nextBGO.x));
        COMUTILS_RETURN_IF_FAILED(newBGO->put_Y(nextBGO.y));
    }

    CComPtr<IPGELevelSections> sectionArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_Sections(&sectionArray));
    for (const LevelSection& nextSection : data.sections)
    {
        CComPtr<IPGELevelSection> newSection = NULL;
        COMUTILS_RETURN_IF_FAILED(sectionArray->Add(&newSection));
        COMUTILS_RETURN_IF_FAILED(newSection->put_BackgroundID(nextSection.background));
        COMUTILS_RETURN_IF_FAILED(newSection->put_BGColor(nextSection.bgcolor));
        COMUTILS_RETURN_IF_FAILED(newSection->put_Bottom(nextSection.size_bottom));
        COMUTILS_RETURN_IF_FAILED(newSection->put_CustomMusicFile(CComBSTR(nextSection.music_file.c_str())));
        COMUTILS_RETURN_IF_FAILED(newSection->put_ID(nextSection.id));
        COMUTILS_RETURN_IF_FAILED(newSection->put_IsOffscreenExit(COMBOOL(nextSection.OffScreenEn)));
        COMUTILS_RETURN_IF_FAILED(newSection->put_IsUnderwater(COMBOOL(nextSection.underwater)));
        COMUTILS_RETURN_IF_FAILED(newSection->put_IsWarp(COMBOOL(nextSection.wrap_h)));
        COMUTILS_RETURN_IF_FAILED(newSection->put_Left(nextSection.size_left));
        COMUTILS_RETURN_IF_FAILED(newSection->put_MusicID(nextSection.music_id));
        COMUTILS_RETURN_IF_FAILED(newSection->put_NoTurnBack(COMBOOL(nextSection.lock_left_scroll)));
        COMUTILS_RETURN_IF_FAILED(newSection->put_Right(nextSection.size_right));
        COMUTILS_RETURN_IF_FAILED(newSection->put_Top(nextSection.size_top));
    }
        
    CComPtr<IPGELevelPlayerPoints> playerPointsArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_PlayerPoints(&playerPointsArray));
    for (const PlayerPoint& nextPlayerPoint : data.players)
    {
        CComPtr<IPGELevelPlayerPoint> newPlayerPoint = NULL;
        COMUTILS_RETURN_IF_FAILED(playerPointsArray->Add(&newPlayerPoint));
        COMUTILS_RETURN_IF_FAILED(newPlayerPoint->put_Height(nextPlayerPoint.h));
        COMUTILS_RETURN_IF_FAILED(newPlayerPoint->put_ID(nextPlayerPoint.id));
        COMUTILS_RETURN_IF_FAILED(newPlayerPoint->put_Width(nextPlayerPoint.w));
        COMUTILS_RETURN_IF_FAILED(newPlayerPoint->put_X(nextPlayerPoint.x));
        COMUTILS_RETURN_IF_FAILED(newPlayerPoint->put_Y(nextPlayerPoint.y));
    }

    CComPtr<IPGELevelNPCs> npcArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_NPCs(&npcArray));
    for (const LevelNPC& nextNPC : data.npc)
    {
        CComPtr<IPGELevelNPC> newNPC = NULL;
        COMUTILS_RETURN_IF_FAILED(npcArray->Add(&newNPC));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_AttachLayerName(CComBSTR(nextNPC.attach_layer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_Direction(static_cast<PGEDirection>(nextNPC.direct)));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_EventActivateName(CComBSTR(nextNPC.event_activate.c_str())));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_EventDieName(CComBSTR(nextNPC.event_die.c_str())));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_EventNoMoreObjectInLayerName(CComBSTR(nextNPC.event_emptylayer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_EventTalkName(CComBSTR(nextNPC.event_talk.c_str())));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_GeneratorDirection(static_cast<PGEGeneratorDirection>(nextNPC.generator_direct)));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_GeneratorPeriod(nextNPC.generator_period));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_GeneratorType(static_cast<PGEGeneratorType>(nextNPC.generator_type)));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_ID(nextNPC.id));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_IsFriendly(COMBOOL(nextNPC.friendly)));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_IsGenerator(COMBOOL(nextNPC.generator)));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_IsLegacyBoss(COMBOOL(nextNPC.is_boss)));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_LayerName(CComBSTR(nextNPC.layer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_Message(CComBSTR(nextNPC.msg.c_str())));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_NoMove(COMBOOL(nextNPC.nomove)));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_SpecialData1(nextNPC.special_data));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_SpecialData2(nextNPC.special_data2));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_X(nextNPC.x));
        COMUTILS_RETURN_IF_FAILED(newNPC->put_Y(nextNPC.y));
    }

    CComPtr<IPGELevelWarps> warpArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_Warps(&warpArray));
    for (const LevelDoor& nextDoor : data.doors)
    {
        CComPtr<IPGELevelWarp> newWarp = NULL;
        COMUTILS_RETURN_IF_FAILED(warpArray->Add(&newWarp));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_AllowNPC(COMBOOL(nextDoor.allownpc)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_EntranceDirection(static_cast<PGEWarpEntrance>(nextDoor.idirect)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_EntranceX(nextDoor.ix));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_EntranceY(nextDoor.iy));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_ExitDirection(static_cast<PGEWarpExit>(nextDoor.odirect)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_ExitX(nextDoor.ox));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_ExitY(nextDoor.oy));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_IsEntranceActive(nextDoor.isSetIn));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_IsExitActive(nextDoor.isSetOut));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_IsLevelEntrance(COMBOOL(nextDoor.lvl_i)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_IsLevelExit(COMBOOL(nextDoor.lvl_o)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_IsLocked(COMBOOL(nextDoor.locked)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_LayerName(CComBSTR(nextDoor.layer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_LevelName(CComBSTR(nextDoor.lname.c_str())));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_LevelWarpID(nextDoor.warpto));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_NoVehicle(COMBOOL(nextDoor.novehicles)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_RequiredStars(nextDoor.stars));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_Type(static_cast<PGEWarpType>(nextDoor.type)));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_WarpNumber(nextDoor.meta.index));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_WorldX(nextDoor.world_x));
        COMUTILS_RETURN_IF_FAILED(newWarp->put_WorldY(nextDoor.world_y));
    }

    CComPtr<IPGELevelPhysicalEnvironments> physArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_PhysicalEnvironments(&physArray));
    for (const LevelPhysEnv nextPhys : data.physez) 
    {
        CComPtr<IPGELevelPhysicalEnvironment> newPhys = NULL;
        COMUTILS_RETURN_IF_FAILED(physArray->Add(&newPhys));
        COMUTILS_RETURN_IF_FAILED(newPhys->put_Height(nextPhys.h));
        COMUTILS_RETURN_IF_FAILED(newPhys->put_LayerName(CComBSTR(nextPhys.layer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newPhys->put_Type(static_cast<PGEPhysicalEnvironmentType>(nextPhys.env_type)));
        COMUTILS_RETURN_IF_FAILED(newPhys->put_Width(nextPhys.w));
        COMUTILS_RETURN_IF_FAILED(newPhys->put_X(nextPhys.x));
        COMUTILS_RETURN_IF_FAILED(newPhys->put_Y(nextPhys.y));
    }

    CComPtr<IPGELevelLayers> layerArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_Layers(&layerArray));
    for (const LevelLayer nextLayer : data.layers)
    {
        CComPtr<IPGELevelLayer> newLayer = NULL;
        COMUTILS_RETURN_IF_FAILED(layerArray->Add(&newLayer));
        COMUTILS_RETURN_IF_FAILED(newLayer->put_Hidden(COMBOOL(nextLayer.hidden)));
        COMUTILS_RETURN_IF_FAILED(newLayer->put_Locked(COMBOOL(nextLayer.locked)));
        COMUTILS_RETURN_IF_FAILED(newLayer->put_Name(CComBSTR(nextLayer.name.c_str())));
    }

    CComPtr<IPGELevelEvents> eventArray = NULL;
    COMUTILS_RETURN_IF_FAILED(newDataObj->get_Events(&eventArray));
    for (const LevelSMBX64Event nextEvent : data.events)
    {
        CComPtr<IPGELevelEvent> newEvent = NULL;
        COMUTILS_RETURN_IF_FAILED(eventArray->Add(&newEvent));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_Autostart(COMBOOL(nextEvent.autostart)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_CameraSpeedX(nextEvent.move_camera_x));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_CameraSpeedY(nextEvent.move_camera_y));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_EndGameType(static_cast<PGEEventEndGame>(nextEvent.end_game)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyAltJump(COMBOOL(nextEvent.ctrl_altjump)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyAltRun(COMBOOL(nextEvent.ctrl_altrun)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyDown(COMBOOL(nextEvent.ctrl_down)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyDrop(COMBOOL(nextEvent.ctrl_drop)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyJump(COMBOOL(nextEvent.ctrl_jump)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyLeft(COMBOOL(nextEvent.ctrl_left)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyRight(COMBOOL(nextEvent.ctrl_right)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyRun(COMBOOL(nextEvent.ctrl_run)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyStart(COMBOOL(nextEvent.ctrl_start)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_KeyUp(COMBOOL(nextEvent.ctrl_up)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_LayerSpeedX(nextEvent.layer_speed_x));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_LayerSpeedY(nextEvent.layer_speed_y));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_MoveLayerName(CComBSTR(nextEvent.movelayer.c_str())));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_Name(CComBSTR(nextEvent.name.c_str())));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_NoSmoke(COMBOOL(nextEvent.nosmoke)));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_SectionToScroll(nextEvent.scroll_section));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_SoundID(nextEvent.sound_id));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_TalkMessage(CComBSTR(nextEvent.msg.c_str())));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_TriggerEventName(CComBSTR(nextEvent.trigger.c_str())));
        COMUTILS_RETURN_IF_FAILED(newEvent->put_TriggerTime(nextEvent.trigger_timer));

        CComPtr<IPGELevelSectionModifiers> eventSectionModifiers = NULL;
        COMUTILS_RETURN_IF_FAILED(newEvent->get_SectionModifiers(&eventSectionModifiers));
        for (int i = 1; i <= 21; ++i) 
        {
            if ((signed)nextEvent.sets.size() <= i - 1) break;
            LevelEvent_Sets nextModifier = nextEvent.sets[i - 1];
                
            CComPtr<IPGELevelSectionModifier> newModifier = NULL;
            COMUTILS_RETURN_IF_FAILED(eventSectionModifiers->get_Item(i, &newModifier));
            COMUTILS_RETURN_IF_FAILED(newModifier->put_BackgroundID(nextModifier.background_id));
            COMUTILS_RETURN_IF_FAILED(newModifier->put_Bottom(nextModifier.position_bottom));
            COMUTILS_RETURN_IF_FAILED(newModifier->put_Left(nextModifier.position_left));
            COMUTILS_RETURN_IF_FAILED(newModifier->put_MusicID(nextModifier.music_id));
            COMUTILS_RETURN_IF_FAILED(newModifier->put_Right(nextModifier.position_right));
            COMUTILS_RETURN_IF_FAILED(newModifier->put_Top(nextModifier.position_top));
        }

        CComPtr<IPGEStringCollection> eventLayerToHide = NULL;
        COMUTILS_RETURN_IF_FAILED(newEvent->get_LayersToHide(&eventLayerToHide));
        for (const std::string& nextLayerToHide : nextEvent.layers_hide)
        {
            COMUTILS_RETURN_IF_FAILED(eventLayerToHide->Add(CComBSTR(nextLayerToHide.c_str())));
        }

        CComPtr<IPGEStringCollection> eventLayerToShow = NULL;
        COMUTILS_RETURN_IF_FAILED(newEvent->get_LayersToShow(&eventLayerToShow));
        for (const std::string& nextLayerToShow : nextEvent.layers_show)
        {
            COMUTILS_RETURN_IF_FAILED(eventLayerToShow->Add(CComBSTR(nextLayerToShow.c_str())));
        }

        CComPtr<IPGEStringCollection> eventLayerToToggle = NULL;
        COMUTILS_RETURN_IF_FAILED(newEvent->get_LayersToToggle(&eventLayerToToggle));
        for (const std::string& nextLayerToToggle : nextEvent.layers_toggle)
        {
            COMUTILS_RETURN_IF_FAILED(eventLayerToToggle->Add(CComBSTR(nextLayerToToggle.c_str())));
        }

    }

    COMUTILS_RETURN_IF_FAILED(newDataObj.CopyTo(pVal));
    return S_OK;
    
    // TODO: Unreachable code, alter later
    return E_INVALIDARG;
}


STDMETHODIMP CPGEFileFactory::SaveLevelDataByFilename(BSTR filename, IPGELevelData* input, PGELevelFormats formatType)
{
    COMUTILS_RETURN_IF_FAILED_START;
    std::string fName = PGEComUtils::ConvertBSTRToMBS(filename);
    PGELevelFormats usedFormat = formatType;
    if (usedFormat == LVLFORMAT_DETECT) {
        COMUTILS_RETURN_IF_FAILED(DetectFileFormatLevel(fName, usedFormat));
    }

    LevelData newData;
    CComPtr<IPGELevelSections> nextSections = NULL;
    input->get_Sections(&nextSections);
    
    long countOfSections;
    nextSections->get_Count(&countOfSections);
    for (long i = 1; i <= countOfSections; ++i)
    {
        CComPtr<IPGELevelSection> nextSection = NULL;
        nextSections->get_Item(i, &nextSection);
        
        LevelSection newSection = FileFormats::CreateLvlSection();

    }

    


    return S_OK;
}
