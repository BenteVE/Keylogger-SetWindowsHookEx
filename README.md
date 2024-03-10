# Keylogger-SetWindowsHookEx

A prototype for a key- and mouse logger that uses KeyboardProc and MouseProc to get the keystrokes and SetWindowsHookEx to install the procedures.

## Installer

## Procedures

 This program installs a KeyboardProc and a MouseProc hook from a DLL with SetWindowsHookEx.
 When this hook is installed globally with SetWindowsHookEx, the hook logs all keyboard presses and mouse clicks and movements to a file.

- KeyboardProc callback function: <https://learn.microsoft.com/en-us/windows/win32/winmsg/keyboardproc>
- MouseProc callback function: <https://learn.microsoft.com/en-us/windows/win32/winmsg/mouseproc>

## Architectures (x86/x64)

Because hooks run in the context of an application, they must match the "bitness" of the application.
If a 32-bit application installs a global hook on 64-bit Windows, the 32-bit hook is injected into each 32-bit process (the usual security boundaries apply).
In a 64-bit process, the threads are still marked as "hooked."
However, because a 32-bit application must run the hook code, the system executes the hook in the hooking app's context;
specifically, on the thread that called SetWindowsHookEx.
This means that the hooking application must continue to pump messages or it might block the normal functioning of the 64-bit processes.
More info: <https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw#remarks>