========================================================================
    AKTIVE VORLAGENBIBLIOTHEK: PGEFile-Projektübersicht
========================================================================

Der Anwendungs-Assistent hat dieses PGEFile-Projekt als Ausgangspunkt zum Schreiben der DLL (Dynamic Link Library) erstellt.

Diese Datei bietet eine Übersicht über den Inhalt der einzelnen Dateien, aus denen Ihr Projekt besteht.

PGEFile.vcxproj
    Dies ist die Hauptprojektdatei für VC++-Projekte, die mit dem Anwendungs-Assistenten generiert werden. Sie enthält Informationen über die Version von Visual C++, mit der die Datei generiert wurde, sowie über die Plattformen, Konfigurationen und Projektfunktionen, die im Anwendungs-Assistenten ausgewählt wurden.

PGEFile.vcxproj.filters
    Dies ist die Filterdatei für VC++-Projekte, die mithilfe eines Anwendungs-Assistenten erstellt werden. Sie enthält Informationen über die Zuordnung zwischen den Dateien im Projekt und den Filtern. Diese Zuordnung wird in der IDE zur Darstellung der Gruppierung von Dateien mit ähnlichen Erweiterungen unter einem bestimmten Knoten verwendet (z. B. sind CPP-Dateien dem Filter "Quelldateien" zugeordnet).

PGEFile.idl
    Diese Datei enthält die IDL-Definitionen der Typbibliothek, Schnittstellen und Co-Klassen, die im Projekt definiert sind.
    Diese Datei wird vom MIDL-Compiler verarbeitet, um Folgendes zu generieren:
        C++-Schnittstellendefinitionen und GUID-Deklarationen (PGEFile.h)
        GUID-Definitionen                                (PGEFile_i.c)
        Eine Typbibliothek                                  (PGEFile.tlb)
        Marshallingcode                                 (PGEFile_p.c und dlldata.c)

PGEFile.h
    Diese Datei enthält die C++-Schnittstellendefinitionen und GUID-Deklarationen der in PGEFile.idl definierten Elemente. Sie wird von MIDL während der Kompilierung erneut generiert.

PGEFile.cpp
    Diese Datei enthält die Objekttabelle und die Implementierung der DLL-Exporte.

PGEFile.rc
    Dies ist eine Auflistung aller vom Programm verwendeten Microsoft Windows-Ressourcen.

PGEFile.def
    Diese Moduldefinitionsdatei stellt dem Linker die für die DLL erforderlichen Informationen über die Exporte bereit. Sie enthält Exporte für:
        DllGetClassObject
        DllCanUnloadNow
        DllRegisterServer
        DllUnregisterServer
        DllInstall

/////////////////////////////////////////////////////////////////////////////
Andere Standarddateien:

StdAfx.h, StdAfx.cpp
    Mit diesen Dateien werden eine vorkompilierte Headerdatei (PCH) mit dem Namen PGEFile.pch und eine vorkompilierte Typendatei mit dem Namen StdAfx.obj erstellt.

Resource.h
    Dies ist die Standardheaderdatei, die Ressourcen-IDs definiert.


/////////////////////////////////////////////////////////////////////////////
