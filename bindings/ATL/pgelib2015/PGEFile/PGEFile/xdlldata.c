// Wrapper für "dlldata.c"

#ifdef _MERGE_PROXYSTUB // Proxy-Stub-DLL zusammenführen

#define REGISTER_PROXY_DLL //DllRegisterServer usw.

#define _WIN32_WINNT 0x0500	//für WinNT 4.0 oder Win95 mit DCOM
#define USE_STUBLESS_PROXY	//nur mit dem MIDL-Schalter "/Oicf" definiert

#pragma comment(lib, "rpcns4.lib")
#pragma comment(lib, "rpcrt4.lib")

#define ENTRY_PREFIX	Prx

#include "dlldata.c"
#include "PGEFile_p.c"

#endif //_MERGE_PROXYSTUB
