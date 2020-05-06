// PGELevelWarp.h: Deklaration von CPGELevelWarp

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelWarp

class ATL_NO_VTABLE CPGELevelWarp :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelWarp, &CLSID_PGELevelWarp>,
	public IDispatchImpl<IPGELevelWarp, &IID_IPGELevelWarp, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    LONG m_entrance_x;
    LONG m_entrance_y;
    VARIANT_BOOL m_is_entrance_active;
    LONG m_exit_x;
    LONG m_exit_y;
    VARIANT_BOOL m_is_exit_active;
    PGEWarpEntrance m_entrance_direction;
    PGEWarpExit m_exit_direction;
    PGEWarpType m_type;
    CComBSTR m_level_target_name;
    LONG m_target_warp;
    VARIANT_BOOL m_is_level_entrance;
    VARIANT_BOOL m_is_level_exit;
    LONG m_world_x;
    LONG m_world_y;
    LONG m_required_stars;
    CComBSTR m_layer_name;
    VARIANT_BOOL m_no_vehicle;
    VARIANT_BOOL m_allow_npc;
    VARIANT_BOOL m_is_locked;

    LONG m_warp_number;
public:
	CPGELevelWarp() :
        m_entrance_x(0),
        m_entrance_y(0),
        m_is_entrance_active(VARIANT_FALSE),
        m_exit_x(0),
        m_exit_y(0),
        m_is_exit_active(VARIANT_FALSE),
        m_entrance_direction(ENTRANCE_UP),
        m_exit_direction(EXIT_DOWN),
        m_type(WARP_INSTANT),
        m_level_target_name(""),
        m_target_warp(0),
        m_is_level_entrance(VARIANT_FALSE),
        m_is_level_exit(VARIANT_FALSE),
        m_world_x(-1),
        m_world_y(-1),
        m_required_stars(0),
        m_layer_name("Default"),
        m_no_vehicle(VARIANT_FALSE),
        m_allow_npc(VARIANT_FALSE),
        m_is_locked(VARIANT_FALSE),
        m_warp_number(0)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELWARP)

DECLARE_NOT_AGGREGATABLE(CPGELevelWarp)

BEGIN_COM_MAP(CPGELevelWarp)
	COM_INTERFACE_ENTRY(IPGELevelWarp)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



    STDMETHOD(get_EntranceX)(LONG* pVal);
    STDMETHOD(put_EntranceX)(LONG newVal);
    STDMETHOD(get_EntranceY)(LONG* pVal);
    STDMETHOD(put_EntranceY)(LONG newVal);
    STDMETHOD(get_IsEntranceActive)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsEntranceActive)(VARIANT_BOOL newVal);
    STDMETHOD(get_ExitX)(LONG* pVal);
    STDMETHOD(put_ExitX)(LONG newVal);
    STDMETHOD(get_ExitY)(LONG* pVal);
    STDMETHOD(put_ExitY)(LONG newVal);
    STDMETHOD(get_IsExitActive)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsExitActive)(VARIANT_BOOL newVal);
    STDMETHOD(get_EntranceDirection)(PGEWarpEntrance* pVal);
    STDMETHOD(put_EntranceDirection)(PGEWarpEntrance newVal);
    STDMETHOD(get_ExitDirection)(PGEWarpExit* pVal);
    STDMETHOD(put_ExitDirection)(PGEWarpExit newVal);
    STDMETHOD(get_Type)(PGEWarpType* pVal);
    STDMETHOD(put_Type)(PGEWarpType newVal);
    STDMETHOD(get_LevelName)(BSTR* pVal);
    STDMETHOD(put_LevelName)(BSTR newVal);
    STDMETHOD(get_LevelWarpID)(LONG* pVal);
    STDMETHOD(put_LevelWarpID)(LONG newVal);
    STDMETHOD(get_IsLevelEntrance)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsLevelEntrance)(VARIANT_BOOL newVal);
    STDMETHOD(get_IsLevelExit)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsLevelExit)(VARIANT_BOOL newVal);
    STDMETHOD(get_WorldX)(LONG* pVal);
    STDMETHOD(put_WorldX)(LONG newVal);
    STDMETHOD(get_WorldY)(LONG* pVal);
    STDMETHOD(put_WorldY)(LONG newVal);
    STDMETHOD(get_RequiredStars)(LONG* pVal);
    STDMETHOD(put_RequiredStars)(LONG newVal);
    STDMETHOD(get_LayerName)(BSTR* pVal);
    STDMETHOD(put_LayerName)(BSTR newVal);
    STDMETHOD(get_NoVehicle)(VARIANT_BOOL* pVal);
    STDMETHOD(put_NoVehicle)(VARIANT_BOOL newVal);
    STDMETHOD(get_AllowNPC)(VARIANT_BOOL* pVal);
    STDMETHOD(put_AllowNPC)(VARIANT_BOOL newVal);
    STDMETHOD(get_IsLocked)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsLocked)(VARIANT_BOOL newVal);
    STDMETHOD(get_WarpNumber)(LONG* pVal);
    STDMETHOD(put_WarpNumber)(LONG newVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelWarp), CPGELevelWarp)
