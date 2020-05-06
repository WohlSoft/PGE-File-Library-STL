// PGELevelPhysicalEnvironment.h: Deklaration von CPGELevelPhysicalEnvironment

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelPhysicalEnvironment

class ATL_NO_VTABLE CPGELevelPhysicalEnvironment :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelPhysicalEnvironment, &CLSID_PGELevelPhysicalEnvironment>,
	public IDispatchImpl<IPGELevelPhysicalEnvironment, &IID_IPGELevelPhysicalEnvironment, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    LONG m_x;
    LONG m_y;
    LONG m_width;
    LONG m_height;
    PGEPhysicalEnvironmentType m_type;
    CComBSTR m_layer_name;
public:
	CPGELevelPhysicalEnvironment() :
        m_x(0),
        m_y(0),
        m_width(0),
        m_height(0),
        m_type(ENVIRONMENT_WATER),
        m_layer_name("Default")
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELPHYSICALENVIRONMENT)

DECLARE_NOT_AGGREGATABLE(CPGELevelPhysicalEnvironment)

BEGIN_COM_MAP(CPGELevelPhysicalEnvironment)
	COM_INTERFACE_ENTRY(IPGELevelPhysicalEnvironment)
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
    STDMETHOD(get_Width)(LONG* pVal);
    STDMETHOD(put_Width)(LONG newVal);
    STDMETHOD(get_Height)(LONG* pVal);
    STDMETHOD(put_Height)(LONG newVal);
    STDMETHOD(get_Type)(PGEPhysicalEnvironmentType* pVal);
    STDMETHOD(put_Type)(PGEPhysicalEnvironmentType newVal);
    STDMETHOD(get_LayerName)(BSTR* pVal);
    STDMETHOD(put_LayerName)(BSTR newVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelPhysicalEnvironment), CPGELevelPhysicalEnvironment)
