// Created on: 1998-08-06
// Created by: Administrateur Atelier MDL
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.




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
LONG APIENTRY WndProc(HWND hWndFrame, UINT wMsg, WPARAM wParam, LONG lParam )
{
  HWND hWndClient;	
  switch(wMsg)
  {
    case WM_CREATE :
      {
	CreateProc(hWndFrame);					
	hWndClient = (HWND)GetWindowLong(hWndFrame, CLIENTWND);
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
      hWndClient = (HWND)GetWindowLong(hWndFrame, CLIENTWND);
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
  HWND hWnd;

  // Save hWnd in the main window in extra memory in 0
  if (hWnd = CreateMDIClientWindow(hWndFrame))
    SetWindowLong(hWndFrame, CLIENTWND, (LONG)hWnd);
  return(TRUE);
}


/*--------------------------------------------------------------------------*\
|  COMMAND PROCEDURE
|  		Handler for message WM_COMMAND   
|
\*--------------------------------------------------------------------------*/
BOOL CommandProc(HWND hWndFrame, WPARAM wParam, LPARAM lParam)
{
  HWND hWndClient; // Handle on window MDI
  HWND hWndActive;

	hWndClient = (HWND)GetWindowLong(hWndFrame, CLIENTWND);
  switch (LOWORD(wParam))
	{
	  case IDM_WINDOW_NEXT :
					if(hWndClient = (HWND)GetWindowLong(hWndFrame, CLIENTWND))
					  hWndActive = (HWND)SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0l);
						SendMessage(hWndClient, WM_MDINEXT, (WPARAM)hWndActive, 0l);  
					break;

		case IDM_WINDOW_CASCADE :
					if(hWndClient = (HWND)GetWindowLong(hWndFrame, CLIENTWND))
						SendMessage(hWndClient, WM_MDICASCADE, 0, 0l);
					break;
					 
		case IDM_WINDOW_TILEHOR :
					if(hWndClient = (HWND)GetWindowLong(hWndFrame, CLIENTWND))
						SendMessage(hWndClient, WM_MDITILE, MDITILE_HORIZONTAL, 0l);
					break;

		case IDM_WINDOW_TILEVERT :
					if(hWndClient = (HWND)GetWindowLong(hWndFrame, CLIENTWND))
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
#ifndef _WIN64
  HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
#else
  HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
#endif
  Destroy_Appli(hInst);
  PostQuitMessage(0);
}
#endif

