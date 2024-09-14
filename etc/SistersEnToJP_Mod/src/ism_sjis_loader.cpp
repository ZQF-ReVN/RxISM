#include <Windows.h>
#include <detours.h>


INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nShowCmd)
{
    STARTUPINFOW si{};
    si.cb = sizeof(STARTUPINFOW);
    PROCESS_INFORMATION pi{};
    LPCSTR dll_name_array[1]{ "ism_sjis_hook.dll" };
    if (!::DetourCreateProcessWithDllsW(L"ism.exe", nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi, 1, dll_name_array, nullptr))
    {
        ::MessageBoxW(nullptr, L"failed to inject dll", nullptr, MB_OK);
        ::ExitProcess(-1);
    }
    ::ResumeThread(pi.hThread);
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
    return 0;
}
