// PGELevelData.h: Deklaration von CPGELevelData

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"

#include "PGELevelBlocks.h"
#include "PGELevelBGOs.h"
#include "PGELevelSections.h"
#include "PGELevelPlayerPoints.h"
#include "PGELevelNPCs.h"
#include "PGELevelWarps.h"
#include "PGELevelEvents.h"
#include "PGELevelPhysicalEnvironments.h"
#include "PGELevelLayers.h"

#include "Utils/ComUtils.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelData

class ATL_NO_VTABLE CPGELevelData :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CPGELevelData, &CLSID_PGELevelData>,
    public IDispatchImpl<IPGELevelData, &IID_IPGELevelData, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
private:
    LONG m_stars;
    CComBSTR m_levelName;
    CComPtr<IPGELevelBlocks> m_blocks;
    CComPtr<IPGELevelBGOs> m_bgos;
    CComPtr<IPGELevelSections> m_sections;
    CComPtr<IPGELevelPlayerPoints> m_player_points;
    CComPtr<IPGELevelNPCs> m_npcs;
    CComPtr<IPGELevelWarps> m_warps;
    CComPtr<IPGELevelEvents> m_events;
    CComPtr<IPGELevelPhysicalEnvironments> m_physical_environments;
    CComPtr<IPGELevelLayers> m_layers;
public:
    CPGELevelData() : 
        m_stars(0),
        m_levelName("")
    {
        PGEComUtils::CreateObjectInternal<CPGELevelBlocks>(&m_blocks);
        PGEComUtils::CreateObjectInternal<CPGELevelBGOs>(&m_bgos);
        PGEComUtils::CreateObjectInternal<CPGELevelSections>(&m_sections);
        PGEComUtils::CreateObjectInternal<CPGELevelPlayerPoints>(&m_player_points);
        PGEComUtils::CreateObjectInternal<CPGELevelNPCs>(&m_npcs);
        PGEComUtils::CreateObjectInternal<CPGELevelWarps>(&m_warps);
        PGEComUtils::CreateObjectInternal<CPGELevelEvents>(&m_events);
        PGEComUtils::CreateObjectInternal<CPGELevelPhysicalEnvironments>(&m_physical_environments);
        PGEComUtils::CreateObjectInternal<CPGELevelLayers>(&m_layers);
    }

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELDATA)

DECLARE_NOT_AGGREGATABLE(CPGELevelData)

BEGIN_COM_MAP(CPGELevelData)
    COM_INTERFACE_ENTRY(IPGELevelData)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
#ifdef _DEBUG
        std::cout << "LevelData released!" << std::endl;
#endif
    }
   

public:



    STDMETHOD(get_Stars)(LONG* pVal);
    STDMETHOD(put_Stars)(LONG newVal);
    STDMETHOD(get_LevelName)(BSTR* pVal);
    STDMETHOD(put_LevelName)(BSTR newVal);
    STDMETHOD(get_Blocks)(IPGELevelBlocks** pVal);
    STDMETHOD(get_BGOs)(IPGELevelBGOs** pVal);
    STDMETHOD(get_Sections)(IPGELevelSections** pVal);
    STDMETHOD(get_PlayerPoints)(IPGELevelPlayerPoints** pVal);
    STDMETHOD(get_NPCs)(IPGELevelNPCs** pVal);
    STDMETHOD(get_Warps)(IPGELevelWarps** pVal);
    STDMETHOD(get_Events)(IPGELevelEvents** pVal);
    STDMETHOD(get_PhysicalEnvironments)(IPGELevelPhysicalEnvironments** pVal);
    STDMETHOD(get_Layers)(IPGELevelLayers** pVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelData), CPGELevelData)
