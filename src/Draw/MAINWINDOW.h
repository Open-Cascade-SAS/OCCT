/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

/****************************************************\
*/ 
/**  MainWindow.h
*/ 
/*\****************************************************/ 
#ifdef _WIN32

#define CLIENTWND 0

/*
** PROCEDURES DE MAIN WINDOW
*/
#include <Windows.h>

LRESULT APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CreateProc(HWND);
VOID DestroyProc(HWND);
LRESULT APIENTRY CmdProc(HWND, UINT, WPARAM, LPARAM);


#endif
