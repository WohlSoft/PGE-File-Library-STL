// PGELevelLayer.h: Deklaration von CPGELevelLayer

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelLayer

class ATL_NO_VTABLE CPGELevelLayer :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelLayer, &CLSID_PGELevelLayer>,
	public IDispatchImpl<IPGELevelLayer, &IID_IPGELevelLayer, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    CComBSTR m_name;
    VARIANT_BOOL m_hidden;
    VARIANT_BOOL m_locked;
public:
	CPGELevelLayer() :
        m_name(""),
        m_hidden(VARIANT_FALSE),
        m_locked(VARIANT_FALSE)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELLAYER)

DECLARE_NOT_AGGREGATABLE(CPGELevelLayer)

BEGIN_COM_MAP(CPGELevelLayer)
	COM_INTERFACE_ENTRY(IPGELevelLayer)
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
    STDMETHOD(get_Hidden)(VARIANT_BOOL* pVal);
    STDMETHOD(put_Hidden)(VARIANT_BOOL newVal);
    STDMETHOD(get_Locked)(VARIANT_BOOL* pVal);
    STDMETHOD(put_Locked)(VARIANT_BOOL newVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelLayer), CPGELevelLayer)
