// PGELevelEvent.h: Deklaration von CPGELevelEvent

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"
#include "PGEStringCollection.h"
#include "Utils/ComUtils.h"
#include "PGELevelSectionModifiers.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelEvent

class ATL_NO_VTABLE CPGELevelEvent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelEvent, &CLSID_PGELevelEvent>,
	public IDispatchImpl<IPGELevelEvent, &IID_IPGELevelEvent, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    CComBSTR m_name;
    CComBSTR m_talk_message;
    LONG m_sound_id;
    PGEEventEndGame m_end_game;
    VARIANT_BOOL m_no_smoke;
    // Array of layers_hide
    // Array of layers_show
    // Array of layers_toggle
    // Array of LevelEvents_Sets
    CComPtr<IPGEStringCollection> m_layers_to_hide;
    CComPtr<IPGEStringCollection> m_layers_to_show;
    CComPtr<IPGEStringCollection> m_layers_to_toggle;
    CComPtr<IPGELevelSectionModifiers> m_section_modifiers;
    // ---
    CComBSTR m_trigger_event;
    LONG m_trigger_timer;
    VARIANT_BOOL m_key_up;
    VARIANT_BOOL m_key_down;
    VARIANT_BOOL m_key_left;
    VARIANT_BOOL m_key_right;
    VARIANT_BOOL m_key_jump;
    VARIANT_BOOL m_key_alt_jump;
    VARIANT_BOOL m_key_run;
    VARIANT_BOOL m_key_alt_run;
    VARIANT_BOOL m_key_start;
    VARIANT_BOOL m_key_drop;
    VARIANT_BOOL m_autostart;
    CComBSTR m_move_layer_name;
    FLOAT m_layer_speed_x;
    FLOAT m_layer_speed_y;
    FLOAT m_move_camera_x;
    FLOAT m_move_camera_y;
    long m_scroll_section;
public:
    CPGELevelEvent() :
        m_name(""),
        m_talk_message(""),
        m_sound_id(0),
        m_end_game(ENDGAME_BOWSER_DEFEAT),
        m_no_smoke(VARIANT_TRUE),
        m_layers_to_hide(NULL),
        m_layers_to_show(NULL),
        m_layers_to_toggle(NULL),
        m_section_modifiers(NULL),
        m_trigger_event(""),
        m_trigger_timer(0),
        m_key_up(VARIANT_FALSE),
        m_key_down(VARIANT_FALSE),
        m_key_left(VARIANT_FALSE),
        m_key_right(VARIANT_FALSE),
        m_key_jump(VARIANT_FALSE),
        m_key_alt_jump(VARIANT_FALSE),
        m_key_run(VARIANT_FALSE),
        m_key_alt_run(VARIANT_FALSE),
        m_key_start(VARIANT_FALSE),
        m_key_drop(VARIANT_FALSE),
        m_autostart(VARIANT_FALSE),
        m_move_layer_name(""),
        m_layer_speed_x(0.0f),
        m_layer_speed_y(0.0f),
        m_move_camera_x(0.0f),
        m_move_camera_y(0.0f),
        m_scroll_section(0)
    {
        PGEComUtils::CreateObjectInternal<CPGEStringCollection>(&m_layers_to_hide);
        PGEComUtils::CreateObjectInternal<CPGEStringCollection>(&m_layers_to_show);
        PGEComUtils::CreateObjectInternal<CPGEStringCollection>(&m_layers_to_toggle);
        PGEComUtils::CreateObjectInternal<CPGELevelSectionModifiers>(&m_section_modifiers);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELEVENT)

DECLARE_NOT_AGGREGATABLE(CPGELevelEvent)

BEGIN_COM_MAP(CPGELevelEvent)
	COM_INTERFACE_ENTRY(IPGELevelEvent)
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



    STDMETHOD(get_Name)(BSTR* pVal);
    STDMETHOD(put_Name)(BSTR newVal);
    STDMETHOD(get_TalkMessage)(BSTR* pVal);
    STDMETHOD(put_TalkMessage)(BSTR newVal);
    STDMETHOD(get_SoundID)(LONG* pVal);
    STDMETHOD(put_SoundID)(LONG newVal);
    STDMETHOD(get_EndGameType)(PGEEventEndGame* pVal);
    STDMETHOD(put_EndGameType)(PGEEventEndGame newVal);
    STDMETHOD(get_NoSmoke)(VARIANT_BOOL* pVal);
    STDMETHOD(put_NoSmoke)(VARIANT_BOOL newVal);
    STDMETHOD(get_TriggerEventName)(BSTR* pVal);
    STDMETHOD(put_TriggerEventName)(BSTR newVal);
    STDMETHOD(get_TriggerTime)(LONG* pVal);
    STDMETHOD(put_TriggerTime)(LONG newVal);
    STDMETHOD(get_KeyUp)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyUp)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyDown)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyDown)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyLeft)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyLeft)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyRight)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyRight)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyJump)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyJump)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyAltJump)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyAltJump)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyRun)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyRun)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyAltRun)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyAltRun)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyStart)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyStart)(VARIANT_BOOL newVal);
    STDMETHOD(get_KeyDrop)(VARIANT_BOOL* pVal);
    STDMETHOD(put_KeyDrop)(VARIANT_BOOL newVal);
    STDMETHOD(get_Autostart)(VARIANT_BOOL* pVal);
    STDMETHOD(put_Autostart)(VARIANT_BOOL newVal);
    STDMETHOD(get_MoveLayerName)(BSTR* pVal);
    STDMETHOD(put_MoveLayerName)(BSTR newVal);
    STDMETHOD(get_LayerSpeedX)(FLOAT* pVal);
    STDMETHOD(put_LayerSpeedX)(FLOAT newVal);
    STDMETHOD(get_LayerSpeedY)(FLOAT* pVal);
    STDMETHOD(put_LayerSpeedY)(FLOAT newVal);
    STDMETHOD(get_CameraSpeedX)(FLOAT* pVal);
    STDMETHOD(put_CameraSpeedX)(FLOAT newVal);
    STDMETHOD(get_CameraSpeedY)(FLOAT* pVal);
    STDMETHOD(put_CameraSpeedY)(FLOAT newVal);
    STDMETHOD(get_SectionToScroll)(LONG* pVal);
    STDMETHOD(put_SectionToScroll)(LONG newVal);
    STDMETHOD(get_LayersToHide)(IPGEStringCollection** pVal);
    STDMETHOD(get_LayersToShow)(IPGEStringCollection** pVal);
    STDMETHOD(get_LayersToToggle)(IPGEStringCollection** pVal);
    STDMETHOD(get_SectionModifiers)(IPGELevelSectionModifiers** pVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelEvent), CPGELevelEvent)
