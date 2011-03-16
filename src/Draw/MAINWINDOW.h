/****************************************************\
*  MainWindow.h
\****************************************************/
#ifdef WNT

#define CLIENTWND 0

/*
** PROCEDURES DE MAIN WINDOW
*/
#include <Windows.h>

LONG APIENTRY WndProc(HWND, UINT, WPARAM, LONG);
BOOL CreateProc(HWND);
VOID DestroyProc(HWND);
BOOL CommandProc(HWND, WPARAM, LPARAM);


#endif
