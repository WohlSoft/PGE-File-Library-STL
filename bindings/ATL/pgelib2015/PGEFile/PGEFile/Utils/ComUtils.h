#pragma once

#include <atlbase.h>
#include <vector>
#include "GenericCopyPolicy.h"


#ifndef COR_E_ENDOFSTREAM
#define COR_E_ENDOFSTREAM (HRESULT)0x80070026
#endif

#ifndef COR_E_FILELOAD
#define COR_E_FILELOAD (HRESULT)0x80131621
#endif

#ifndef COR_E_FILENOTFOUND
#define COR_E_FILENOTFOUND (HRESULT)0x80070002
#endif

#ifndef COR_E_DIRECTORYNOTFOUND
#define COR_E_DIRECTORYNOTFOUND (HRESULT)0x80070003
#endif

#ifndef COR_E_PATHTOOLONG
#define COR_E_PATHTOOLONG (HRESULT)0x800700CE
#endif

#ifndef COR_E_IO
#define COR_E_IO (HRESULT)0x80131620
#endif


#define COMBOOL(var) (var ? VARIANT_TRUE : VARIANT_FALSE)

#define COMUTILS_SIMPLE_PROPIMPL_GET(classType, varType, internalPropVal, outerPropVal) \
    STDMETHODIMP classType ## ::get_ ## outerPropVal ## ( varType * pVal ) { \
        *pVal = internalPropVal ; \
        return S_OK; \
    }

#define COMUTILS_SIMPLE_PROPIMPL_PUT(classType, varType, internalPropVal, outerPropVal) \
    STDMETHODIMP classType ## ::put_ ## outerPropVal ## ( varType newVal ) { \
        internalPropVal = newVal; \
        return S_OK; \
    }

#define COMUTILS_SIMPLE_PROPIMPL_FULL(classType, varType, internalPropVal, outerPropVal) \
    COMUTILS_SIMPLE_PROPIMPL_GET(classType, varType, internalPropVal, outerPropVal) \
    COMUTILS_SIMPLE_PROPIMPL_PUT(classType, varType, internalPropVal, outerPropVal)


#define COMUTILS_SIMPLE_PROPIMPL_GET(classType, varType, internalPropVal, outerPropVal) \
    STDMETHODIMP classType ## ::get_ ## outerPropVal ## ( varType * pVal ) { \
        *pVal = internalPropVal ; \
        return S_OK; \
    }

#define COMUTILS_SIMPLE_PROPIMPL_PUT(classType, varType, internalPropVal, outerPropVal) \
    STDMETHODIMP classType ## ::put_ ## outerPropVal ## ( varType newVal ) { \
        internalPropVal = newVal; \
        return S_OK; \
    }

#define COMUTILS_SIMPLE_PROPIMPL_FULL(classType, varType, internalPropVal, outerPropVal) \
    COMUTILS_SIMPLE_PROPIMPL_GET(classType, varType, internalPropVal, outerPropVal) \
    COMUTILS_SIMPLE_PROPIMPL_PUT(classType, varType, internalPropVal, outerPropVal)


#define COMUTILS_BSTR_PROPIMPL_GET(classType, internalPropVal, outerPropVal) \
    STDMETHODIMP classType ## ::get_ ## outerPropVal ## ( BSTR* pVal ) { \
        internalPropVal ## .CopyTo(pVal); \
        return S_OK; \
    }    

#define COMUTILS_BSTR_PROPIMPL_PUT(classType, internalPropVal, outerPropVal) \
    STDMETHODIMP classType ## ::put_ ## outerPropVal ## ( BSTR newVal ) { \
        internalPropVal ## .AssignBSTR(newVal); \
        return S_OK; \
    }

#define COMUTILS_BSTR_PROPIMPL_FULL(classType, internalPropVal, outerPropVal) \
    COMUTILS_BSTR_PROPIMPL_GET(classType, internalPropVal, outerPropVal) \
    COMUTILS_BSTR_PROPIMPL_PUT(classType, internalPropVal, outerPropVal)

#define COMUTILS_RETURN_IF_FAILED_START HRESULT ___errCodeUse_
#define COMUTILS_RETURN_IF_FAILED(code) \
    ___errCodeUse_ = code; \
    if(FAILED(___errCodeUse_)) \
        return ___errCodeUse_;


namespace PGEComUtils
{
    template<class CoClassType, class InterfaceType>
    HRESULT CreateObjectInternal(InterfaceType** intf)
    {
        COMUTILS_RETURN_IF_FAILED_START;
        typedef ATL::CComObject<CoClassType> InterfaceTypeObject;
        InterfaceTypeObject* internalObject = NULL;
        COMUTILS_RETURN_IF_FAILED(InterfaceTypeObject::CreateInstance(&internalObject));
        return internalObject->QueryInterface(intf);
    }

	template<class CollectionInterfaceType, class ObjectInterfaceType>
	struct CommonComCollection
	{
		// 1. Collection Type
		typedef std::vector< ATL::CComPtr<ObjectInterfaceType> >						ContainerType;

		// 2. Interface Types
		typedef ObjectInterfaceType*													CollectionExposedType;
		typedef CollectionInterfaceType													CollectionInterface;

		// 3. Enum Types
		typedef VARIANT																	EnumeratorExposedType;
		typedef IEnumVARIANT															EnumeratorInterface;

		// 4. Copy Policy
		typedef InterfaceAdapterCopyToVariant<ObjectInterfaceType>                      EnumeratorCopyType;
		typedef InterfaceAdapterCopy<ObjectInterfaceType>                               CollectionCopyType;

		// 5. Full Enum & Collection Types
		typedef ATL::CComEnumOnSTL< EnumeratorInterface, &__uuidof(EnumeratorInterface), EnumeratorExposedType, EnumeratorCopyType, ContainerType > EnumeratorType;
		typedef ATL::ICollectionOnSTLImpl< CollectionInterface, ContainerType, CollectionExposedType, CollectionCopyType, EnumeratorType > CollectionType;
	};

	template <class CollectionInterfaceType, class ObjectInterfaceType, class ObjectCoClassType>
	class CommonComCollectionImpl :
		public CommonComCollection<CollectionInterfaceType, ObjectInterfaceType>::CollectionType
	{
	public:
		STDMETHOD(Clear)()
		{
			m_coll.clear();
			return S_OK;
		}
		STDMETHOD(Add)(ObjectInterfaceType** pVal)
		{
			CComPtr<ObjectInterfaceType> newObject = NULL;
			PGEComUtils::CreateObjectInternal<ObjectCoClassType>(&newObject);
            
			m_coll.push_back(newObject);
			newObject.CopyTo(pVal);
			return S_OK;
		}
		STDMETHOD(Remove)(long Index)
		{
			long realIndex = Index - 1;
			if (realIndex < (long)0 || realIndex >(long)m_coll.size() - 1)
				return E_INVALIDARG;

			m_coll.erase(m_coll.begin() + realIndex);
			return S_OK;
		}
		STDMETHOD(RemoveItem)(ObjectInterfaceType* ItemToRemove)
		{
			for (int i = 0; i < (int)m_coll.size(); ++i) {
				if (m_coll[i] == ItemToRemove)
					return Remove(i + 1);
			}
			return E_INVALIDARG;
		}
	};


    template<class CollectionInterfaceType>
    struct CommonBSTRCollection 
    {
        // 1. Collection Type
        typedef std::vector< ATL::CComBSTR >		                       				ContainerType;

        // 2. Interface Types
        typedef BSTR                													CollectionExposedType;
        typedef CollectionInterfaceType													CollectionInterface;

        // 3. Enum Types
        typedef VARIANT																	EnumeratorExposedType;
        typedef IEnumVARIANT															EnumeratorInterface;

        // 4. Copy Policy
        typedef CComBSTRCopy<VARIANT>                                                   EnumeratorCopyType;
        typedef CComBSTRCopy<BSTR>                                                      CollectionCopyType;

        // 5. Full Enum & Collection Types
        typedef ATL::CComEnumOnSTL< EnumeratorInterface, &__uuidof(EnumeratorInterface), EnumeratorExposedType, EnumeratorCopyType, ContainerType > EnumeratorType;
        typedef ATL::ICollectionOnSTLImpl< CollectionInterface, ContainerType, CollectionExposedType, CollectionCopyType, EnumeratorType > CollectionType;
    };

    template <class CollectionInterfaceType>
    class CommonBSTRCollectionImpl :
        public CommonBSTRCollection<CollectionInterfaceType>::CollectionType
    {
    public:
        STDMETHOD(Clear)()
        {
            m_coll.clear();
            return S_OK;
        }
        STDMETHOD(Add)(BSTR pVal)
        {
            m_coll.emplace_back(pVal);
            return S_OK;
        }
        STDMETHOD(Remove)(long Index)
        {
            long realIndex = Index - 1;
            if (realIndex < (long)0 || realIndex >(long)m_coll.size() - 1)
                return E_INVALIDARG;

            m_coll.erase(m_coll.begin() + realIndex);
            return S_OK;
        }
        STDMETHOD(RemoveItem)(BSTR ItemToRemove)
        {
            for (int i = 0; i < (int)m_coll.size(); ++i) {
                if (m_coll[i] == ItemToRemove)
                    return Remove(i + 1);
            }
            return E_INVALIDARG;
        }
    };
    
    std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
    std::string ConvertBSTRToMBS(BSTR bstr);
    BSTR ConvertMBSToBSTR(const std::string& str);

}
