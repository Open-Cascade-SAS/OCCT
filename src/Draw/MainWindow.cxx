// Created on: 1998-08-06
// Created by: Administrateur Atelier MDL
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifdef WNT

#include <windows.h>
#include <DrawRessource.h>
#include <init.h>
#include <MainWindow.h>
#include <Draw_Window.hxx>
#include <CommandWindow.h>

Standard_Boolean Draw_Interprete(const char* command); // Implemented in Draw.cxx
extern Standard_Boolean Draw_IsConsoleSubsystem;

//extern "C" int  compat_unlink(const char *fname); // Implemente dans TCL

/*--------------------------------------------------------*\
|  CLIENT WINDOW PROCEDURE
|
|
\*--------------------------------------------------------*/
LRESULT APIENTRY WndProc(HWND hWndFrame, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
  HWND hWndClient;	
  switch(wMsg)
  {
    case WM_CREATE :
      {
	CreateProc(hWndFrame);					
	hWndClient = (HWND)GetWindowLongPtr(hWndFrame, CLIENTWND);
	DrawWindow::hWndClientMDI = hWndClient;
	if (!Draw_IsConsoleSubsystem)
	  CreateCommandWindow(hWndFrame,0);					
      }
      break;

    case WM_COMMAND :
      CommandProc(hWndFrame, wParam, lParam);
      break;
      
    case WM_DESTROY :
      Draw_Interprete("exit");
      DestroyProc(hWndFrame);								
      break;

    default :
      hWndClient = (HWND)GetWindowLongPtr(hWndFrame, CLIENTWND);
      return(DefFrameProc(hWndFrame, hWndClient, wMsg, wParam, lParam));
  }
  return(0l);
}


/*--------------------------------------------------------------------------*\
|  CLIENT CREATE PROCEDURE
|     Handler for message WM_CREATE. Creation of control window MDI
|
\*--------------------------------------------------------------------------*/
BOOL CreateProc(HWND hWndFrame)
{
  HWND hWnd = CreateMDIClientWindow (hWndFrame);
  if (hWnd != NULL)
  {
    // Save hWnd in the main window in extra memory in 0
    SetWindowLongPtr(hWndFrame, CLIENTWND, (LONG_PTR)hWnd);
  }
  return(TRUE);
}


/*--------------------------------------------------------------------------*\
|  COMMAND PROCEDURE
|  		Handler for message WM_COMMAND   
|
\*--------------------------------------------------------------------------*/
BOOL CommandProc(HWND hWndFrame, WPARAM wParam, LPARAM /*lParam*/)
{
  // Handle on window MDI
  HWND hWndClient = (HWND)GetWindowLongPtr (hWndFrame, CLIENTWND);
  switch (LOWORD(wParam))
	{
	  case IDM_WINDOW_NEXT :
					if(hWndClient)
					{
					  HWND hWndActive = (HWND)SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0l);
					  SendMessage(hWndClient, WM_MDINEXT, (WPARAM)hWndActive, 0l);
					}
					break;

		case IDM_WINDOW_CASCADE :
					if(hWndClient)
						SendMessage(hWndClient, WM_MDICASCADE, 0, 0l);
					break;
					 
		case IDM_WINDOW_TILEHOR :
					if(hWndClient)
						SendMessage(hWndClient, WM_MDITILE, MDITILE_HORIZONTAL, 0l);
					break;

		case IDM_WINDOW_TILEVERT :
					if(hWndClient)
						SendMessage(hWndClient, WM_MDITILE, MDITILE_VERTICAL, 0l);
					break;
		
		case IDM_FILE_EXIT :
					Draw_Interprete("exit");
					//compat_unlink(NULL);

					DestroyProc(hWndFrame);
					break;
	}
  return(TRUE);
}


/*--------------------------------------------------------------------------*\
|  CLIENT DESTROY PROCEDURE
|     Handler for message WM_DESTROY.
|
\*--------------------------------------------------------------------------*/
VOID DestroyProc(HWND hWnd)
{
  HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

  Destroy_Appli(hInst);
  PostQuitMessage(0);
}
#endif

