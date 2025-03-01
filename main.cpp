#include <windows.h>
#include <detours.h>
extern "C" VOID __declspec(dllexport) giga(){}
typedef LPVOID(WINAPI* pVirtualAlloc)(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD flAllocationType,
    DWORD flProtect
);
typedef HFONT(WINAPI* pCreateFontIndirectA)(
    const LOGFONTA* lplf
    );
typedef HFONT (WINAPI* pCreateFontA)(int cHeight,int cWidth,int cEscapement,int cOrientation,int cWeight,DWORD bItalic,
DWORD bUnderline,DWORD bStrikeOut,DWORD iCharSet,DWORD iOutPrecision,DWORD iClipPrecision,DWORD iQuality,DWORD iPitchAndFamily,LPCSTR pszFaceName);
typedef HANDLE(WINAPI* pCreateFileW)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
);
pVirtualAlloc orgVirtualAlloc;
pCreateFontIndirectA orgCreateFontIndirectA;
pCreateFontA orgCreateFontA;
pCreateFileW orgCreateFileW;
LPVOID WINAPI newVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    if (dwSize == 0x00583000)
    {
        DWORD* entrypoint = (DWORD*)0x00AEF34A;
        if (*entrypoint == 0x5004EC83)
        {
            *entrypoint = 0x90000CC2;
            DetourTransactionBegin();
            DetourDetach(&(PVOID&)orgVirtualAlloc, newVirtualAlloc);
            DetourTransactionCommit();
        }
    }
    return orgVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}
HFONT WINAPI newCreateFontIndirectA(LOGFONTA* lplf)
{
    lplf->lfCharSet = 0x86;
    strcpy_s(lplf->lfFaceName, "Simsun");
    return orgCreateFontIndirectA(lplf);
}
HFONT  WINAPI newCreateFontA(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic,
    DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName)
{
    iCharSet = GB2312_CHARSET;
    return orgCreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
}
HANDLE WINAPI newCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    static LPCWSTR tarFileName = L".\\Update.pac";
    static LPCWSTR repFileName = L".\\Soikano_opus.pac";
    if (!wcscmp(tarFileName, repFileName))
    {
        lpFileName = repFileName;
    }
    return orgCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
VOID WINAPI WriteBorden()
{
    while (true)
    {
        DWORD* checkAddress1 = (DWORD*)0x00404D6C;
        DWORD* checkAddress2 = (DWORD*)0x004056BA;
        DWORD* checkAddress3 = (DWORD*)0x004E3732;
        DWORD* checkAddress4 = (DWORD*)0x004E4E08;
        DWORD* checkAddress5 = (DWORD*)0x004E4EAE;
        DWORD* checkAddress6 = (DWORD*)0x004E4FBF;
        DWORD* checkAddress7 = (DWORD*)0x004E5109;
        DWORD* checkAddress8 = (DWORD*)0x004E51EE;
        DWORD* checkAddress9 = (DWORD*)0x004E5593;
        DWORD* checkAddress10 = (DWORD*)0x004E5922;
        if (*checkAddress1 == 0x769FFA80
          &&*checkAddress2 == 0x08769F3C
          &&*checkAddress3 == 0x06769F3C)
        {
            *checkAddress1 = 0x76FEFA80;
            *checkAddress2 = 0x0876FE3C;
            *checkAddress3 = 0x0676FE3C;
            *checkAddress4 = 0x0A76FE3C;
            *checkAddress5 = 0x76FEF980;
            *checkAddress6 = 0x76FEF980;
            *checkAddress7 = 0x0676FE3C;
            *checkAddress8 = 0x76FEF980;
            *checkAddress9 = 0x0C76FE3C;
            *checkAddress10 = 0x0676FE3C;
            break;
        }
        Sleep(1000);
    }
}
VOID StartHook()
{
    orgVirtualAlloc = VirtualAlloc;
    orgCreateFontIndirectA = CreateFontIndirectA;
    orgCreateFontA = CreateFontA;
    orgCreateFileW = CreateFileW;

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)orgVirtualAlloc, newVirtualAlloc);
    DetourAttach(&(PVOID&)orgCreateFontIndirectA, newCreateFontIndirectA);
    DetourAttach(&(PVOID&)orgCreateFontA, newCreateFontA);
    DetourAttach(&(PVOID&)orgCreateFileW, newCreateFileW);
    DetourTransactionCommit();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)WriteBorden, NULL, NULL, NULL);
        StartHook();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}