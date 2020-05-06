// PGEFileFactory.h: Deklaration von CPGEFileFactory

#pragma once
#include "resource.h"       // Hauptsymbole



#include "PGEFile_i.h"


#ifdef _DEBUG
#include <iostream>
#endif

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Singlethread-COM-Objekte werden auf der Windows CE-Plattform nicht vollständig unterstützt. Windows Mobile-Plattformen bieten beispielsweise keine vollständige DCOM-Unterstützung. Definieren Sie _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA, um ATL zu zwingen, die Erstellung von Singlethread-COM-Objekten zu unterstützen und die Verwendung eigener Singlethread-COM-Objektimplementierungen zu erlauben. Das Threadmodell in der RGS-Datei wurde auf 'Free' festgelegt, da dies das einzige Threadmodell ist, das auf Windows CE-Plattformen ohne DCOM unterstützt wird."
#endif

using namespace ATL;


// CPGEFileFactory

class ATL_NO_VTABLE CPGEFileFactory :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CPGEFileFactory, &CLSID_PGEFileFactory>,
    public ISupportErrorInfo,
    public IDispatchImpl<IPGEFileFactory, &IID_IPGEFileFactory, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CPGEFileFactory()
    {
        
    }

DECLARE_REGISTRY_RESOURCEID(IDR_PGEFILEFACTORY)


BEGIN_COM_MAP(CPGEFileFactory)
    COM_INTERFACE_ENTRY(IPGEFileFactory)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);


    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
#ifdef _DEBUG
        std::cout << "FileFactory released!" << std::endl;
#endif
    }

public:


    STDMETHOD(CreateNewLevelData)(IPGELevelData** pVal);
    STDMETHOD(LoadLevelDataByFilename)(BSTR filename, PGELevelFormats formatType, IPGELevelData** pVal);
    STDMETHOD(SaveLevelDataByFilename)(BSTR filename, IPGELevelData* input, PGELevelFormats formatType);
};

OBJECT_ENTRY_AUTO(__uuidof(PGEFileFactory), CPGEFileFactory)