#include <windows.h>

#include <iostream>
#include <tchar.h> // using _tmain macro
#include <conio.h> // using _getch

// Used for x86 and x64 compatibility
#ifdef UNICODE
#define tcout std::wcout
#else
#define tcout std::cout
#endif

// CLI help message
void displayHelp() {
    tcout
        << TEXT("Usage: ./DLLInjector.exe <dll_path>\n")
        << TEXT("\nArguments:\n")
        << TEXT("  <dll_path>:      Path to the .dll file with the procedures.\n")
        << std::endl;
}

HHOOK hookKeyboard;
HHOOK hookMouse;

// All global hook functions must be in libraries.
void WINAPI installGlobalHooks(HMODULE lib)
{
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
        tcout << "Can't find Keyboard function in dll!" << std::endl;
        return;
    }

    if (procedureMouse == NULL) {
        tcout << "Can't find Mouse function in dll!" << std::endl;
        return;
    }

    // install hook keyboard
    hookKeyboard = SetWindowsHookEx(WH_KEYBOARD, procedureKeyboard, lib, 0);
    if (hookKeyboard == NULL) {
        tcout << "Keyboard hook failed to install!" << std::endl;
        return;
    }

    // install hook mouse
    hookMouse = SetWindowsHookEx(WH_MOUSE, procedureMouse, lib, 0);
    if (hookMouse == NULL) {
        tcout << "Mouse hook failed to install!" << std::endl;
        return;
    }

    tcout << "Hooks installed properly!" << std::endl;
}

int _tmain(int argc, TCHAR* argv[])
{
    if (argc != 2) {
        displayHelp();
        return 1;
    } else if (argv[1] == TEXT("--help")){
        displayHelp();
        return 0;
    }

    // Find the DLL with the procedures
    HMODULE lib = LoadLibrary(argv[1]);

    if (lib == NULL) {
        tcout << "Can't find dll!" << std::endl;
        return 1;
    }
    

    // Install the procedures
    installGlobalHooks(lib);

    // If a 32-bit application installs a global hook on 64-bit Windows, the 32-bit hook is injected into each 32-bit process.
    // In a 64-bit process, the threads are still marked as "hooked." 
    // However, because a 32-bit application must run the hook code, the system executes the hook in the hooking app's context; specifically, on the thread that called SetWindowsHookEx.
    // This means that the hooking application must continue to pump messages or it might block the normal functioning of the 64-bit processes.
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa#remarks

    MSG message;
    // GetMessage retrieves a message from the calling thread's message queue. 
    // The function dispatches incoming sent messages until a posted message is available for retrieval.
    while (GetMessage(&message, NULL, 0, 0))
    {

        // TranslateMessage translates virtual-key messages into character messages. 
        // The character messages are posted to the calling thread's message queue, to be read the next time the thread calls the GetMessage
        TranslateMessage(&message);

        // DispatchMessage dispatches a message to a window procedure.
        // It is typically used to dispatch a message retrieved by the GetMessage function.
        DispatchMessage(&message);
    }

    // Stop the keylogger
    
    // TODO: how do we break the message loop?
    tcout << "Closing" << std::endl;
    FreeLibrary(lib);
    UnhookWindowsHookEx(hookKeyboard);
    UnhookWindowsHookEx(hookMouse);
}