#include "../stdafx.h"
#include "Error.h"
#include <crtdbg.h>
#include <atlbase.h>

// Throws an exception corresponding to the given Win32 Error code.
// GUID is the CLSID of the component throwing error.
HRESULT CError::DispatchWin32Error(DWORD dwError,
								   REFCLSID clsid,
								   LPTSTR szSource,
								   DWORD dwHelpContext,
								   LPTSTR szHelpFileName)
{
	// Dispatch the requested error message
	return DispatchError(HRESULT_FROM_WIN32(dwError),
			clsid, szSource, NULL, dwHelpContext,
			szHelpFileName);
}

// Throws the given error code and the message corresponding
// to the code. If the code is a standard code and no message
// is provided, then the message is extracted from the system.
HRESULT CError::DispatchError(HRESULT hError,
							  REFCLSID clsid,
							  LPTSTR szSource,
							  LPTSTR szDescription,
							  DWORD dwHelpContext,
							  LPTSTR szHelpFileName)
{
	// This function uses ATL conversion macros
	// (Hence we must use this MACRO provided by ATL)
	USES_CONVERSION;

	// Convert the description to OLE string
	LPOLESTR wszError = NULL;
	if(szDescription != NULL)
	{
		// Convert to wide char
		wszError = T2OLE(szDescription);
	}
	else
	{
		// If the code is a Win32 error code
		if(HRESULT_FACILITY(hError) == FACILITY_WIN32)
		{
			// Get the error from the system
			LPTSTR szError = NULL;
			if(!::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL,
				HRESULT_CODE(hError),
				MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT),
				(LPTSTR)&szError, 0, NULL))
				return HRESULT_FROM_WIN32(GetLastError());

			// Convert the Error multibyte string to OLE string
			if(szError != NULL)
			{
				// Convert to wide char
				wszError = T2OLE(szError);
				// Free the multibyte string
				LocalFree(szError);
			}
		}
	}

	// Convert the source string to OLE string
	LPOLESTR wszSource = NULL;
	if(szSource != NULL)
		wszSource = T2OLE(szSource);

	// Convert the help filename to OLE string
	LPOLESTR wszHelpFile = NULL;
	if(szHelpFileName != NULL)
		wszHelpFile = T2OLE(szHelpFileName);

	// Get the ICreateErrorInfo Interface
	ICreateErrorInfo *pCreateErrorInfo = NULL;
    HRESULT hSuccess = CreateErrorInfo(&pCreateErrorInfo);
	ATLASSERT(SUCCEEDED(hSuccess));

	// Fill the error information into it
	pCreateErrorInfo->SetGUID(clsid);
	if(wszError != NULL)
		pCreateErrorInfo->SetDescription(wszError);
	if(wszSource != NULL)
		pCreateErrorInfo->SetSource(wszSource);
	if(wszHelpFile != NULL)
		pCreateErrorInfo->SetHelpFile(wszHelpFile);
	pCreateErrorInfo->SetHelpContext(dwHelpContext);

	// Get the IErrorInfo interface
	IErrorInfo *pErrorInfo = NULL;
	hSuccess = pCreateErrorInfo->QueryInterface(IID_IErrorInfo,
				(LPVOID *)&pErrorInfo);
	if(FAILED(hSuccess))
	{
		pCreateErrorInfo->Release();
		return hSuccess;
	}

	// Set this error information in the current thread
	hSuccess = SetErrorInfo(0, pErrorInfo);
	if(FAILED(hSuccess))
	{
		pCreateErrorInfo->Release();
		pErrorInfo->Release();
		return hSuccess;
	}

	// Finally release the interfaces
	pCreateErrorInfo->Release();
	pErrorInfo->Release();
	
	// And, Return the error code that was asked
	// to be dispatched
	return hError;
}
