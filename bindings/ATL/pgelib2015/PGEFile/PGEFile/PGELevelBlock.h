// PGEBlock.h: Deklaration von CPGEBlock

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


// CPGEBlock

class ATL_NO_VTABLE CPGELevelBlock :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CPGELevelBlock, &CLSID_PGELevelBlock>,
    public IDispatchImpl<IPGELevelBlock, &IID_IPGELevelBlock, &LIBID_PGEFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
private:
    LONG m_x;
    LONG m_y;
    LONG m_width;
    LONG m_height;
    LONG m_id;
    LONG m_npc_id;
    VARIANT_BOOL m_invisible;
    VARIANT_BOOL m_slippery;
    CComBSTR m_layer_name;
    CComBSTR m_event_destroyed_name;
    CComBSTR m_event_hit_name;
    CComBSTR m_event_no_more_object_in_layer_name;
public:
    CPGELevelBlock() :
        m_x(0),
        m_y(0),
        m_width(0),
        m_height(0),
        m_id(0),
        m_npc_id(0),
        m_invisible(VARIANT_FALSE),
        m_slippery(VARIANT_FALSE),
        m_layer_name("Default"),
        m_event_destroyed_name(""),
        m_event_hit_name(""),
        m_event_no_more_object_in_layer_name("")
    {}

DECLARE_REGISTRY_RESOURCEID(IDR_PGELEVELBLOCK)

DECLARE_NOT_AGGREGATABLE(CPGELevelBlock)

BEGIN_COM_MAP(CPGELevelBlock)
    COM_INTERFACE_ENTRY(IPGELevelBlock)
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
        std::cout << "Released block!" << std::endl;
#endif
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
    STDMETHOD(get_ID)(LONG* pVal);
    STDMETHOD(put_ID)(LONG newVal);
    STDMETHOD(get_NPCID)(LONG* pVal);
    STDMETHOD(put_NPCID)(LONG newVal);
    STDMETHOD(get_Invisible)(VARIANT_BOOL* pVal);
    STDMETHOD(put_Invisible)(VARIANT_BOOL newVal);
    STDMETHOD(get_Slippery)(VARIANT_BOOL* pVal);
    STDMETHOD(put_Slippery)(VARIANT_BOOL newVal);
    STDMETHOD(get_LayerName)(BSTR* pVal);
    STDMETHOD(put_LayerName)(BSTR newVal);
    STDMETHOD(get_EventDestroyedName)(BSTR* pVal);
    STDMETHOD(put_EventDestroyedName)(BSTR newVal);
    STDMETHOD(get_EventHitName)(BSTR* pVal);
    STDMETHOD(put_EventHitName)(BSTR newVal);
    STDMETHOD(get_EventNoMoreObjectInLayerName)(BSTR* pVal);
    STDMETHOD(put_EventNoMoreObjectInLayerName)(BSTR newVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(PGELevelBlock), CPGELevelBlock)
