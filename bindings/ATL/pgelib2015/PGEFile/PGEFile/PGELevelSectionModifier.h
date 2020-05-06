// PGELevelSectionModifier.h: Deklaration von CPGELevelSectionModifier

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelSectionModifier

class ATL_NO_VTABLE CPGELevelSectionModifier :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelSectionModifier, &CLSID_PGELevelSectionModifier>,
	public IDispatchImpl<IPGELevelSectionModifier, &IID_IPGELevelSectionModifier, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    LONG m_music_id;
    LONG m_background_id;
    LONG m_left;
    LONG m_top;
    LONG m_bottom;
    LONG m_right;
public:
	CPGELevelSectionModifier() :
        m_music_id(0),
        m_background_id(0),
        m_left(0),
        m_top(0),
        m_bottom(0),
        m_right(0)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELSECTIONMODIFIER)

DECLARE_NOT_AGGREGATABLE(CPGELevelSectionModifier)

BEGIN_COM_MAP(CPGELevelSectionModifier)
	COM_INTERFACE_ENTRY(IPGELevelSectionModifier)
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



    STDMETHOD(get_MusicID)(LONG* pVal);
    STDMETHOD(put_MusicID)(LONG newVal);
    STDMETHOD(get_BackgroundID)(LONG* pVal);
    STDMETHOD(put_BackgroundID)(LONG newVal);
    STDMETHOD(get_Left)(LONG* pVal);
    STDMETHOD(put_Left)(LONG newVal);
    STDMETHOD(get_Top)(LONG* pVal);
    STDMETHOD(put_Top)(LONG newVal);
    STDMETHOD(get_Bottom)(LONG* pVal);
    STDMETHOD(put_Bottom)(LONG newVal);
    STDMETHOD(get_Right)(LONG* pVal);
    STDMETHOD(put_Right)(LONG newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(PGELevelSectionModifier), CPGELevelSectionModifier)
