// PGELevelData.cpp: Implementierung von CPGELevelData

#include "stdafx.h"
#include "PGELevelData.h"


// CPGELevelData



STDMETHODIMP CPGELevelData::get_Stars(LONG* pVal)
{
    *pVal = m_stars;
    return S_OK;
}


STDMETHODIMP CPGELevelData::put_Stars(LONG newVal)
{
    m_stars = newVal;
    return S_OK;
}


STDMETHODIMP CPGELevelData::get_LevelName(BSTR* pVal)
{
    return m_levelName.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::put_LevelName(BSTR newVal)
{
    return m_levelName.AssignBSTR(newVal);
}


STDMETHODIMP CPGELevelData::get_Blocks(IPGELevelBlocks** pVal)
{
    return m_blocks.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_BGOs(IPGELevelBGOs** pVal)
{
    return m_bgos.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_Sections(IPGELevelSections** pVal)
{
    return m_sections.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_PlayerPoints(IPGELevelPlayerPoints** pVal)
{
    return m_player_points.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_NPCs(IPGELevelNPCs** pVal)
{
    return m_npcs.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_Warps(IPGELevelWarps** pVal)
{
    return m_warps.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_Events(IPGELevelEvents** pVal)
{
    return m_events.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_PhysicalEnvironments(IPGELevelPhysicalEnvironments** pVal)
{
    return m_physical_environments.CopyTo(pVal);
}


STDMETHODIMP CPGELevelData::get_Layers(IPGELevelLayers** pVal)
{
    return m_layers.CopyTo(pVal);
}
