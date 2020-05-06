#pragma once

#include <atlbase.h>
#include <type_traits>

// class GenericCopy
template <class DestinationType, class SourceType = DestinationType>
class GenericCopy
{
public:
	typedef DestinationType	destination_type;
	typedef SourceType		source_type;

	static void init(destination_type* p)
	{
		_Copy<destination_type>::init(p);
	}
	static void destroy(destination_type* p)
	{
		_Copy<destination_type>::destroy(p);
	}
	static HRESULT copy(destination_type* pTo, const source_type* pFrom)
	{
		return _Copy<destination_type>::copy(pTo, const_cast<source_type*>(pFrom));
	}

};

template <class InterfaceType>
class InterfaceAdapterCopy
{
public:
	typedef InterfaceType destination_type;
	typedef ATL::CComPtr<InterfaceType> source_type;
	
	static void init(destination_type** p)
	{
		_CopyInterface<destination_type>::init(p);
	}
	static void destroy(destination_type** p)
	{
		_CopyInterface<destination_type>::destroy(p);
	}
	static HRESULT copy(destination_type** pTo, const source_type* pFrom)
	{
		source_type* pFromNonConst = const_cast<source_type*>(pFrom);
		return pFromNonConst->CopyTo(pTo);
	}
};

template <class InterfaceType>
class InterfaceAdapterCopyToVariant
{
public:
	typedef VARIANT destination_type;
	typedef ATL::CComPtr<InterfaceType> source_type;

	static void init(destination_type* p)
	{
		_Copy<VARIANT>::init(p);
	}
	static void destroy(destination_type* p)
	{
		_Copy<VARIANT>::destroy(p);
	}
	static HRESULT copy(destination_type* pTo, const source_type* pFrom)
	{
		pTo->vt = VT_DISPATCH;
		source_type* pFromNonConst = const_cast<source_type*>(pFrom);
		return pFromNonConst->QueryInterface<IDispatch>(&pTo->pdispVal);
	}
};

template<class destionationType>
class CComBSTRCopy
{
public:
    static void init(destionationType* p)
    {
        ATL::_Copy<destionationType>::init(p);
    }
    static void destroy(destionationType* p)
    {
        ATL::_Copy<destionationType>::destroy(p);
    }
    static HRESULT copy(destionationType* pTo, const ATL::CComBSTR* pFrom)
    {
        ATL::CComBSTR* pFromNonConst = const_cast<ATL::CComBSTR*>(pFrom);
        return pFromNonConst->CopyTo(pTo);
    }
};

