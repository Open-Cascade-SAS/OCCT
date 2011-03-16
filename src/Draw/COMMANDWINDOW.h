#ifdef WNT


#define COMMANDCLASS "COMMANDWINDOW"
#define COMMANDTITLE "Command Window"


HWND CreateCommandWindow(HWND, int);
LONG APIENTRY CommandProc(HWND, UINT, WPARAM, LONG);
BOOL CommandCreateProc(HWND);
VOID CommandDestroyProc(HWND);
BOOL CommandHandler(HWND, WPARAM, LPARAM);


#include <sys/stat.h>
#include <tcl.h>
#include <Standard_Stream.hxx>
#include <stdio.h>
//#include <io.h>
#include <fcntl.h>



#endif
