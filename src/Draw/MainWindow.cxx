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

#ifdef _WIN32

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
  switch (wMsg)
  {
    case WM_CREATE:
    {
      CreateProc (hWndFrame);
      HWND hWndClient = (HWND )GetWindowLongPtrW (hWndFrame, CLIENTWND);
      DrawWindow::hWndClientMDI = hWndClient;
      if (!Draw_IsConsoleSubsystem)
      {
        CreateCommandWindow (hWndFrame, 0);
      }
      return 0;
    }
    case WM_COMMAND:
    {
      CmdProc (hWndFrame, LOWORD(wParam), wParam, lParam);
      return 0;
    }
    case WM_DESTROY:
    {
      Draw_Interprete ("exit");
      DestroyProc (hWndFrame);
      return 0;
    }
  }
  HWND hWndClient = (HWND)GetWindowLongPtrW(hWndFrame, CLIENTWND);
  return DefFrameProcW(hWndFrame, hWndClient, wMsg, wParam, lParam);
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
    SetWindowLongPtrW (hWndFrame, CLIENTWND, (LONG_PTR)hWnd);
  }
  return(TRUE);
}


/*--------------------------------------------------------------------------*\
|  COMMAND PROCEDURE
|  		Handler for message WM_COMMAND   
|     It is used when Draw_IsConsoleSubsystem = Standard_False
|     i.e. in non-console mode (see _main_() in Draw_Main.cxx).
\*--------------------------------------------------------------------------*/
LRESULT APIENTRY CmdProc(HWND hWndFrame, UINT wMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  // Handle on window MDI
  HWND hWndClient = (HWND )GetWindowLongPtrW (hWndFrame, CLIENTWND);
  switch (wMsg)
  {
    case IDM_WINDOW_NEXT:
    {
      if (hWndClient != NULL)
      {
        HWND hWndActive = (HWND )SendMessageW (hWndClient, WM_MDIGETACTIVE, 0, 0l);
        SendMessageW (hWndClient, WM_MDINEXT, (WPARAM )hWndActive, 0l);
      }
      break;
    }
    case IDM_WINDOW_CASCADE:
    {
      if (hWndClient != NULL)
      {
        SendMessageW (hWndClient, WM_MDICASCADE, 0, 0l);
      }
      break;
    }
    case IDM_WINDOW_TILEHOR:
    {
      if (hWndClient != NULL)
      {
        SendMessageW (hWndClient, WM_MDITILE, MDITILE_HORIZONTAL, 0l);
      }
      break;
    }
    case IDM_WINDOW_TILEVERT:
    {
      if (hWndClient != NULL)
      {
        SendMessageW (hWndClient, WM_MDITILE, MDITILE_VERTICAL, 0l);
      }
      break;
    }
    case IDM_FILE_EXIT:
    {
      Draw_Interprete ("exit");
      DestroyProc (hWndFrame);
      break;
    }
  }
  return 0;
}


/*--------------------------------------------------------------------------*\
|  CLIENT DESTROY PROCEDURE
|     Handler for message WM_DESTROY.
|
\*--------------------------------------------------------------------------*/
VOID DestroyProc(HWND hWnd)
{
  HINSTANCE hInst = (HINSTANCE )GetWindowLongPtrW (hWnd, GWLP_HINSTANCE);

  Destroy_Appli(hInst);
  PostQuitMessage(0);
}
#endif

