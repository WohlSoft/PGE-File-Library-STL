// PGELevelSectionModifiers.h: Deklaration von CPGELevelSectionModifiers

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"
#include "PGELevelSectionModifier.h"
#include "Utils/ComUtils.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGELevelSectionModifiers

class ATL_NO_VTABLE CPGELevelSectionModifiers :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGELevelSectionModifiers, &CLSID_PGELevelSectionModifiers>,
    public IDispatchImpl<PGEComUtils::CommonComCollection<IPGELevelSectionModifiers, IPGELevelSectionModifier>::CollectionType, &IID_IPGELevelSectionModifiers, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CPGELevelSectionModifiers()
	{
        m_coll.reserve(21);
        for (int i = 0; i < 21; ++i) {
            CComPtr<IPGELevelSectionModifier> newModifier = NULL;
            PGEComUtils::CreateObjectInternal<CPGELevelSectionModifier>(&newModifier);
            m_coll.push_back(newModifier);
        }
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELSECTIONMODIFIERS)

DECLARE_NOT_AGGREGATABLE(CPGELevelSectionModifiers)

BEGIN_COM_MAP(CPGELevelSectionModifiers)
	COM_INTERFACE_ENTRY(IPGELevelSectionModifiers)
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

OBJECT_ENTRY_AUTO(__uuidof(PGELevelSectionModifiers), CPGELevelSectionModifiers)
