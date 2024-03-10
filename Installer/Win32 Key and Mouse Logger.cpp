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