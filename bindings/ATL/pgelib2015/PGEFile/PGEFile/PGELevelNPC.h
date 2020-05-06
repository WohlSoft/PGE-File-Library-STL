// PGELevelNPC.h: Deklaration von CPGELevelNPC

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelNPC

class ATL_NO_VTABLE CPGELevelNPC :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelNPC, &CLSID_PGELevelNPC>,
	public IDispatchImpl<IPGELevelNPC, &IID_IPGELevelNPC, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    LONG m_x;
    LONG m_y;
    PGEDirection m_direction;
    LONG m_id;
    LONG m_special_data1;
    LONG m_special_data2;
    VARIANT_BOOL m_generator;
    PGEGeneratorDirection m_generator_direction;
    PGEGeneratorType m_generator_type;
    LONG m_generator_period;
    CComBSTR m_message;
    VARIANT_BOOL m_friendly;
    VARIANT_BOOL m_nomove;
    VARIANT_BOOL m_legacy_boss;
    CComBSTR m_layer_name;
    CComBSTR m_event_activate;
    CComBSTR m_event_die;
    CComBSTR m_event_talk;
    CComBSTR m_event_no_more_object_in_layer;
    CComBSTR m_layer_attached;
public:
    CPGELevelNPC() :
        m_x(0),
        m_y(0),
        m_direction(DIRECTION_LEFT),
        m_id(0),
        m_special_data1(0),
        m_special_data2(0),
        m_generator(false),
        m_generator_direction(GENERATOR_UP),
        m_generator_type(GENERATOR_WARP),
        m_generator_period(20),
        m_message(""),
        m_friendly(VARIANT_FALSE),
        m_nomove(VARIANT_FALSE),
        m_legacy_boss(VARIANT_FALSE),
        m_layer_name("Default"),
        m_event_activate(""),
        m_event_die(""),
        m_event_talk(""),
        m_event_no_more_object_in_layer(""),
        m_layer_attached("")
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELNPC)

DECLARE_NOT_AGGREGATABLE(CPGELevelNPC)

BEGIN_COM_MAP(CPGELevelNPC)
	COM_INTERFACE_ENTRY(IPGELevelNPC)
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



    STDMETHOD(get_X)(LONG* pVal);
    STDMETHOD(put_X)(LONG newVal);
    STDMETHOD(get_Y)(LONG* pVal);
    STDMETHOD(put_Y)(LONG newVal);
    STDMETHOD(get_Direction)(PGEDirection* pVal);
    STDMETHOD(put_Direction)(PGEDirection newVal);
    STDMETHOD(get_ID)(LONG* pVal);
    STDMETHOD(put_ID)(LONG newVal);
    STDMETHOD(get_SpecialData1)(LONG* pVal);
    STDMETHOD(put_SpecialData1)(LONG newVal);
    STDMETHOD(get_SpecialData2)(LONG* pVal);
    STDMETHOD(put_SpecialData2)(LONG newVal);
    STDMETHOD(get_IsGenerator)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsGenerator)(VARIANT_BOOL newVal);
    STDMETHOD(get_GeneratorDirection)(PGEGeneratorDirection* pVal);
    STDMETHOD(put_GeneratorDirection)(PGEGeneratorDirection newVal);
    STDMETHOD(get_GeneratorType)(PGEGeneratorType* pVal);
    STDMETHOD(put_GeneratorType)(PGEGeneratorType newVal);
    STDMETHOD(get_GeneratorPeriod)(LONG* pVal);
    STDMETHOD(put_GeneratorPeriod)(LONG newVal);
    STDMETHOD(get_Message)(BSTR* pVal);
    STDMETHOD(put_Message)(BSTR newVal);
    STDMETHOD(get_IsFriendly)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsFriendly)(VARIANT_BOOL newVal);
    STDMETHOD(get_NoMove)(VARIANT_BOOL* pVal);
    STDMETHOD(put_NoMove)(VARIANT_BOOL newVal);
    STDMETHOD(get_IsLegacyBoss)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsLegacyBoss)(VARIANT_BOOL newVal);
    STDMETHOD(get_LayerName)(BSTR* pVal);
    STDMETHOD(put_LayerName)(BSTR newVal);
    STDMETHOD(get_EventActivateName)(BSTR* pVal);
    STDMETHOD(put_EventActivateName)(BSTR newVal);
    STDMETHOD(get_EventDieName)(BSTR* pVal);
    STDMETHOD(put_EventDieName)(BSTR newVal);
    STDMETHOD(get_EventTalkName)(BSTR* pVal);
    STDMETHOD(put_EventTalkName)(BSTR newVal);
    STDMETHOD(get_EventNoMoreObjectInLayerName)(BSTR* pVal);
    STDMETHOD(put_EventNoMoreObjectInLayerName)(BSTR newVal);
    STDMETHOD(get_AttachLayerName)(BSTR* pVal);
    STDMETHOD(put_AttachLayerName)(BSTR newVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelNPC), CPGELevelNPC)
