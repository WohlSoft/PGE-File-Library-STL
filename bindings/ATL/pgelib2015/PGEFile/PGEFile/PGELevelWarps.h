// PGELevelWarps.h: Deklaration von CPGELevelWarps

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"
#include "Utils/ComUtils.h"
#include "PGELevelWarp.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelWarps

class ATL_NO_VTABLE CPGELevelWarps :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelWarps, &CLSID_PGELevelWarps>,
    public IDispatchImpl<PGEComUtils::CommonComCollectionImpl<IPGELevelWarps, IPGELevelWarp, CPGELevelWarp>, &IID_IPGELevelWarps, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CPGELevelWarps()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELWARPS)

DECLARE_NOT_AGGREGATABLE(CPGELevelWarps)

BEGIN_COM_MAP(CPGELevelWarps)
	COM_INTERFACE_ENTRY(IPGELevelWarps)
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

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelWarps), CPGELevelWarps)
