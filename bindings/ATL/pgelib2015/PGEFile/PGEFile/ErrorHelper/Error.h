#pragma once

#include <Windows.h>

class CError
{
public:
	
	// Throws an exception corresponding to the given
	// Win32 Error code. GUID is the CLSID of the component
	// throwing error.
	static HRESULT DispatchWin32Error(DWORD dwError,
					REFCLSID clsid, LPTSTR szSource,
					DWORD dwHelpContext,
					LPTSTR szHelpFileName);

	// Throws the given error code and the message
	// corresponding to the code. If the code is a
	// standard code and no message is provided, then
	// the message is extracted from the system.
	static HRESULT DispatchError(HRESULT hError,
					REFCLSID clsid, LPTSTR szSource,
					LPTSTR szDescription,
					DWORD dwHelpContext,
					LPTSTR szHelpFileName);
};
