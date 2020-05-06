// dllmain.h : Deklaration der Modulklasse.

class CPGEFileModule : public ATL::CAtlDllModuleT< CPGEFileModule >
{
public :
	DECLARE_LIBID(LIBID_PGEFileLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PGEFILE, "{8CE1E7AC-6EFE-4308-A2F0-6F6B7B936C5A}")
};

extern class CPGEFileModule _AtlModule;
