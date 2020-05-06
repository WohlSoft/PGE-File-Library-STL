// PGELevelEvents.h: Deklaration von CPGELevelEvents

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"
#include "Utils/ComUtils.h"
#include "PGELevelEvent.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelEvents

class ATL_NO_VTABLE CPGELevelEvents :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelEvents, &CLSID_PGELevelEvents>,
    public IDispatchImpl<PGEComUtils::CommonComCollectionImpl<IPGELevelEvents, IPGELevelEvent, CPGELevelEvent>, &IID_IPGELevelEvents, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CPGELevelEvents()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELEVENTS)

DECLARE_NOT_AGGREGATABLE(CPGELevelEvents)

BEGIN_COM_MAP(CPGELevelEvents)
	COM_INTERFACE_ENTRY(IPGELevelEvents)
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

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelEvents), CPGELevelEvents)
