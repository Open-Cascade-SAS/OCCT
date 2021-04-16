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

// include windows.h first to have all definitions available
#include <windows.h>

#include "Draw_Window.hxx"
#include "Draw_WNTRessource.pxx"
#include "Draw_WNTInit.pxx"
#include "Draw_WNTMainWindow.pxx"
#include "Draw_WNTCommandWindow.pxx"

#define USEDEFAULT 200

/*--------------------------------------------------------*\
|  REGISTER APPLICATION CLASS
|  Enregistrement des classes de fenetres de l'application
|
\*--------------------------------------------------------*/
BOOL RegisterAppClass (HINSTANCE theInstance)
{
  WNDCLASSW wndClass;

  // Parametres communs aux classes
  //-----
  wndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
  wndClass.cbClsExtra    = 0;
  wndClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wndClass.hInstance     = theInstance;

  // Enregistrement de la fenetre principale
  //-----
  wndClass.cbWndExtra    = sizeof(void*);
  wndClass.lpfnWndProc   = (WNDPROC)WndProc;
  wndClass.hIcon         = LoadIconW (theInstance, MAKEINTRESOURCEW(IDI_ICON1));
  wndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName  = MAKEINTRESOURCEW(APPMENU);
  wndClass.lpszClassName = APPCLASS;
  if (!RegisterClassW (&wndClass))
  {
    return FALSE;
  }

  // Enregistrement de la fenetre DrawWindow
  //------
  wndClass.cbWndExtra    = sizeof(void*); // Extra Memory
  wndClass.lpfnWndProc   = (WNDPROC)Draw_Window::DrawProc;
  wndClass.hIcon         = 0;
  wndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
  wndClass.lpszMenuName  = NULL;
  wndClass.lpszClassName = DRAWCLASS;
  if (!RegisterClassW (&wndClass))
  {
    UnregisterClassW (APPCLASS, theInstance);
    return FALSE;
  }

  // Enregistrement de la fenetre CommandWindow
  //------
  wndClass.lpfnWndProc   = (WNDPROC)CommandProc;
  wndClass.hIcon         = 0;
  wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName  = NULL;
  wndClass.lpszClassName = COMMANDCLASS;
  if (!RegisterClassW (&wndClass))
  {
    UnregisterClassW (APPCLASS,  theInstance);
    UnregisterClassW (DRAWCLASS, theInstance);
    return FALSE;
  }

  return TRUE;
}

/*--------------------------------------------------------*\
|  UNREGISTER APPLICATION CLASS
|    Suppression des classes de fenetres de l'application
|
\*--------------------------------------------------------*/
VOID UnregisterAppClass (HINSTANCE theInstance)
{
  UnregisterClassW (APPCLASS,  theInstance);
  UnregisterClassW (DRAWCLASS, theInstance);
}

/*--------------------------------------------------------*\
|  CREATE APPLICATION WINDOW
|    Creation de la fenetre Top-Level
|
\*--------------------------------------------------------*/
HWND CreateAppWindow (HINSTANCE theInstance)
{
  return CreateWindowW (APPCLASS, APPTITLE,
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                        400, 0,
                        623,767,
                        NULL, NULL, theInstance, NULL);
}

/*--------------------------------------------------------*\
|  CREATE MDI CLIENT WINDOW
|    Creation de la fenetre qui contient des fenetres MDI
|
\*--------------------------------------------------------*/
HWND CreateMDIClientWindow (HWND theWndFrame)
{
  CLIENTCREATESTRUCT ccs;
  ccs.hWindowMenu = NULL;
  ccs.idFirstChild = 0;

  HINSTANCE hInstance = (HINSTANCE )GetWindowLongPtrW (theWndFrame, GWLP_HINSTANCE);
  HWND hWndClient = CreateWindowW (L"MDICLIENT", NULL,
                                   WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | MDIS_ALLCHILDSTYLES,
                                   0, 0, 1, 1,
                                   theWndFrame, NULL,
                                   hInstance, (LPVOID )&ccs);
  return hWndClient;
}
#endif
