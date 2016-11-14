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
#include "DrawRessource.h"
#include "init.h"
#include "MainWindow.h"
#include "CommandWindow.h"


#define USEDEFAULT 200


/*--------------------------------------------------------*\
|  REGISTER APPLICATION CLASS
|  Enregistrement des classes de fenetres de l'application
|
d\*--------------------------------------------------------*/

BOOL RegisterAppClass(HINSTANCE hInstance)
{
  WNDCLASSW wndClass;

  // Parametres communs aux classes
  //-----
  wndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
  wndClass.cbClsExtra    = 0;  
  wndClass.hCursor       = LoadCursorW (NULL, IDC_ARROW);
  wndClass.hInstance     = hInstance;

  // Enregistrement de la fenetre principale
  //-----
  wndClass.cbWndExtra    = sizeof(void*);
  wndClass.lpfnWndProc   = (WNDPROC)WndProc;
  wndClass.hIcon         = (HICON )LoadIconW (hInstance, MAKEINTRESOURCE(IDI_ICON1));
  wndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName  = MAKEINTRESOURCEW(APPMENU);
  wndClass.lpszClassName = APPCLASS;

  if(!RegisterClassW(&wndClass))
    return(FALSE);

  // Enregistrement de la fenetre DrawWindow
  //------
  wndClass.cbWndExtra    = sizeof(void*); // Extra Memory
  wndClass.lpfnWndProc   = (WNDPROC)DrawWindow::DrawProc;
  wndClass.hIcon         = 0;
  wndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
  wndClass.lpszMenuName  = NULL;
  wndClass.lpszClassName = DRAWCLASS;

  if(!RegisterClassW(&wndClass))
  {
    UnregisterClassW(APPCLASS, hInstance);
    return(FALSE);
  }


  // Enregistrement de la fenetre CommandWindow
  //------
  wndClass.lpfnWndProc   = (WNDPROC)CommandProc;
  wndClass.hIcon         = 0;
  wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName  = NULL;
  wndClass.lpszClassName = COMMANDCLASS;

  if(!RegisterClassW(&wndClass))
  {
    UnregisterClassW(APPCLASS, hInstance);
    UnregisterClassW(DRAWCLASS, hInstance);
    return(FALSE);
  }

  return(TRUE);
}


/*--------------------------------------------------------*\
|  UNREGISTER APPLICATION CLASS
|    Suppression des classes de fenetres de l'application
|
\*--------------------------------------------------------*/
VOID UnregisterAppClass(HINSTANCE hInstance)
{
  UnregisterClassW(APPCLASS, hInstance);
  UnregisterClassW(DRAWCLASS, hInstance);
}


/*--------------------------------------------------------*\
|  CREATE APPLICATION WINDOW
|    Creation de la fenetre Top-Level
|
\*--------------------------------------------------------*/
HWND CreateAppWindow(HINSTANCE hInstance)
{
  return(CreateWindowW(APPCLASS, APPTITLE,
			    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			    400,0,
			    623,767,
			    NULL, NULL, hInstance, NULL));
}


/*--------------------------------------------------------*\
|  CREATE MDI CLIENT WINDOW
|    Creation de la fenetre qui contient des fenetres MDI
|
\*--------------------------------------------------------*/
HWND CreateMDIClientWindow(HWND hWndFrame)
{
  HWND               hWndClient;
  HANDLE             hInstance;
  CLIENTCREATESTRUCT ccs;

  // Initialisation de la structure client
  ccs.hWindowMenu = NULL;
  ccs.idFirstChild = 0;

  hInstance = (HANDLE)GetWindowLongPtrW(hWndFrame, GWLP_HINSTANCE);

  hWndClient = CreateWindowW(L"MDICLIENT",NULL,
			     									WS_CHILD | WS_CLIPSIBLINGS | 
			     									WS_VISIBLE | MDIS_ALLCHILDSTYLES, 
			     									0, 0, 1, 1,
			     									hWndFrame, NULL, 
			     									(HINSTANCE)hInstance, (LPVOID)&ccs);
  return(hWndClient);
}
#endif
