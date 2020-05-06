// PGEStringCollection.h: Deklaration von CPGEStringCollection

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"
#include "Utils/ComUtils.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGEStringCollection

class ATL_NO_VTABLE CPGEStringCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGEStringCollection, &CLSID_PGEStringCollection>,
    public IDispatchImpl<PGEComUtils::CommonBSTRCollectionImpl<IPGEStringCollection>, &IID_IPGEStringCollection, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CPGEStringCollection()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGESTRINGCOLLECTION)

DECLARE_NOT_AGGREGATABLE(CPGEStringCollection)

BEGIN_COM_MAP(CPGEStringCollection)
	COM_INTERFACE_ENTRY(IPGEStringCollection)
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



};

OBJECT_ENTRY_AUTO(__uuidof(PGEStringCollection), CPGEStringCollection)
