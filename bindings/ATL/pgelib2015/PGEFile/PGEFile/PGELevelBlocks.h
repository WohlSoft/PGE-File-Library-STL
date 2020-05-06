// PGEBlocks.h: Deklaration von CPGEBlocks

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"

#include "PGELevelBlock.h"

#include "Utils/GenericCopyPolicy.h"
#include "Utils/ComUtils.h"
#include <vector>

#ifdef _DEBUG
#include <iostream>
#endif

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGEBlocks

class ATL_NO_VTABLE CPGELevelBlocks :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CPGELevelBlocks, &CLSID_PGELevelBlocks>,
    public IDispatchImpl<PGEComUtils::CommonComCollectionImpl<IPGELevelBlocks, IPGELevelBlock, CPGELevelBlock>, &IID_IPGELevelBlocks, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CPGELevelBlocks()
    {
    }

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELBLOCKS)

DECLARE_NOT_AGGREGATABLE(CPGELevelBlocks)

BEGIN_COM_MAP(CPGELevelBlocks)
    COM_INTERFACE_ENTRY(IPGELevelBlocks)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
#ifdef _DEBUG
        std::cout << "Blocks released!" << std::endl;
#endif
    }

public:

};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelBlocks), CPGELevelBlocks)
