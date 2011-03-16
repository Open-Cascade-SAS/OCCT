#ifdef WNT

#define APPCLASS "TDRAW"
#define APPTITLE "Windows NT TDRAW"
#include <windows.h>

BOOL RegisterAppClass(HINSTANCE);
VOID UnregisterAppClass(HINSTANCE);
HWND CreateAppWindow(HINSTANCE);
HWND CreateMDIClientWindow(HWND);


#endif
