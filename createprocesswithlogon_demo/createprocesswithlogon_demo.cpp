#include <windows.h>
#include <stdio.h>
#include <userenv.h>
#include <iostream> 

void DisplayError(LPWSTR pszAPI)
{
    LPVOID lpvMessageBuffer;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&lpvMessageBuffer, 0, NULL);

    //
    //... now display this string
    //
    wprintf(L"ERROR: API        = %s.\n", pszAPI);
    wprintf(L"       error code = %d.\n", GetLastError());
    wprintf(L"       message    = %s.\n", (LPWSTR)lpvMessageBuffer);

    //
    // Free the buffer allocated by the system
    //
    LocalFree(lpvMessageBuffer);

    ExitProcess(GetLastError());
}

void wmain(int argc, WCHAR* argv[])
{
    DWORD     dwSize;
    HANDLE    hToken;
    LPVOID    lpvEnv;
    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFO         si = { 0 };
    WCHAR               szUserProfile[256] = L"";

    si.cb = sizeof(STARTUPINFO);

    if (argc != 4)
    {
        wprintf(L"Usage: %s [user@domain] [password] [cmd]", argv[0]);
        wprintf(L"\n\n");
        return;
    }

    //
    // TO DO: change NULL to '.' to use local account database
    //
    if (!LogonUser(argv[1], NULL, argv[2], LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT, &hToken))
        DisplayError((LPWSTR)L"LogonUser");

    if (!CreateEnvironmentBlock(&lpvEnv, hToken, TRUE))
        DisplayError((LPWSTR)L"CreateEnvironmentBlock");

    dwSize = sizeof(szUserProfile) / sizeof(WCHAR);

    if (!GetUserProfileDirectory(hToken, szUserProfile, &dwSize))
        DisplayError((LPWSTR)L"GetUserProfileDirectory");

    //
    // TO DO: change NULL to '.' to use local account database
    //
    if (!CreateProcessWithLogonW(argv[1], NULL, argv[2],
        LOGON_WITH_PROFILE, NULL, argv[3],
        CREATE_UNICODE_ENVIRONMENT, lpvEnv, szUserProfile,
        &si, &pi))
        DisplayError((LPWSTR)L"CreateProcessWithLogonW");

    if (!DestroyEnvironmentBlock(lpvEnv))
        DisplayError((LPWSTR)L"DestroyEnvironmentBlock");

    CloseHandle(hToken);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}