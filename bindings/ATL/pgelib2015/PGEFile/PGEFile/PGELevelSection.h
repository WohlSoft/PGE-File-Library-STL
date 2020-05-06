// PGELevelSection.h: Deklaration von CPGELevelSection

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelSection

class ATL_NO_VTABLE CPGELevelSection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelSection, &CLSID_PGELevelSection>,
	public IDispatchImpl<IPGELevelSection, &IID_IPGELevelSection, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
private:
    LONG m_id;
    LONG m_size_top;
    LONG m_size_bottom;
    LONG m_size_left;
    LONG m_size_right;
    LONG m_music_id;
    LONG m_bgcolor;
    VARIANT_BOOL m_iswarp;
    VARIANT_BOOL m_is_offscreen_exit;
    LONG m_bg_id;
    VARIANT_BOOL m_noback;
    VARIANT_BOOL m_isunderwater;
    CComBSTR m_custom_music_file;

public:
	CPGELevelSection() :
        m_id(0),
        m_size_top(0),
        m_size_bottom(0),
        m_size_left(0),
        m_size_right(0),
        m_music_id(0),
        m_bgcolor(16291944),
        m_iswarp(VARIANT_FALSE),
        m_is_offscreen_exit(VARIANT_FALSE),
        m_bg_id(0),
        m_noback(VARIANT_FALSE),
        m_isunderwater(VARIANT_FALSE),
        m_custom_music_file("")
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELSECTION)

DECLARE_NOT_AGGREGATABLE(CPGELevelSection)

BEGIN_COM_MAP(CPGELevelSection)
	COM_INTERFACE_ENTRY(IPGELevelSection)
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

    STDMETHOD(get_ID)(LONG* pVal);
    STDMETHOD(put_ID)(LONG newVal);
    STDMETHOD(get_Top)(LONG* pVal);
    STDMETHOD(put_Top)(LONG newVal);
    STDMETHOD(get_Bottom)(LONG* pVal);
    STDMETHOD(put_Bottom)(LONG newVal);
    STDMETHOD(get_Left)(LONG* pVal);
    STDMETHOD(put_Left)(LONG newVal);
    STDMETHOD(get_Right)(LONG* pVal);
    STDMETHOD(put_Right)(LONG newVal);
    STDMETHOD(get_MusicID)(LONG* pVal);
    STDMETHOD(put_MusicID)(LONG newVal);
    STDMETHOD(get_BGColor)(LONG* pVal);
    STDMETHOD(put_BGColor)(LONG newVal);
    STDMETHOD(get_IsWarp)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsWarp)(VARIANT_BOOL newVal);
    STDMETHOD(get_IsOffscreenExit)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsOffscreenExit)(VARIANT_BOOL newVal);
    STDMETHOD(get_BackgroundID)(LONG* pVal);
    STDMETHOD(put_BackgroundID)(LONG newVal);
    STDMETHOD(get_NoTurnBack)(VARIANT_BOOL* pVal);
    STDMETHOD(put_NoTurnBack)(VARIANT_BOOL newVal);
    STDMETHOD(get_IsUnderwater)(VARIANT_BOOL* pVal);
    STDMETHOD(put_IsUnderwater)(VARIANT_BOOL newVal);
    STDMETHOD(get_CustomMusicFile)(BSTR* pVal);
    STDMETHOD(put_CustomMusicFile)(BSTR newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(PGELevelSection), CPGELevelSection)
