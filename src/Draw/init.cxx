// File:	init.cxx
// Created:	Thu Aug  6 10:02:25 1998
// Author:	Administrateur Atelier MDL
//		<mdl@efalakox.paris1.matra-dtv.fr>


#ifdef WNT

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
  WNDCLASS wndClass;

  // Parametres communs aux classes
  //-----
  wndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
  wndClass.cbClsExtra    = 0;  
  wndClass.hCursor       = LoadCursor(NULL,IDC_ARROW);
  wndClass.hInstance     = hInstance;

  // Enregistrement de la fenetre principale
  //-----
	wndClass.cbWndExtra    = sizeof(LONG);
  wndClass.lpfnWndProc   = (WNDPROC)WndProc;
  wndClass.hIcon         = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  wndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName  = MAKEINTRESOURCE(APPMENU);
  wndClass.lpszClassName = APPCLASS;

  if(!RegisterClass(&wndClass))
    return(FALSE);

  // Enregistrement de la fenetre DrawWindow
  //------
	wndClass.cbWndExtra    = sizeof(LONG); // Extra Memory
  wndClass.lpfnWndProc   = (WNDPROC)DrawWindow::DrawProc;
  wndClass.hIcon         = 0;
  wndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
  wndClass.lpszMenuName  = NULL;
  wndClass.lpszClassName = DRAWCLASS;

  if(!RegisterClass(&wndClass))
  {
    UnregisterClass(APPCLASS, hInstance);
    return(FALSE);
  }


  // Enregistrement de la fenetre CommandWindow
  //------
  wndClass.lpfnWndProc   = (WNDPROC)CommandProc((HWND)WndProc,(WPARAM)342,(LPARAM)443);
  //wndClass.lpfnWndProc   = (WNDPROC)CommandProc;
  wndClass.hIcon         = 0;
  wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName  = NULL;
  wndClass.lpszClassName = COMMANDCLASS;

  if(!RegisterClass(&wndClass))
  {
    UnregisterClass(APPCLASS, hInstance);
    UnregisterClass(DRAWCLASS, hInstance);
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
  UnregisterClass(APPCLASS, hInstance);
  UnregisterClass(DRAWCLASS, hInstance);
}


/*--------------------------------------------------------*\
|  CREATE APPLICATION WINDOW
|    Creation de la fenetre Top-Level
|
\*--------------------------------------------------------*/
HWND CreateAppWindow(HINSTANCE hInstance)
{
  return(CreateWindow(APPCLASS, APPTITLE,
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

#ifndef _WIN64
  hInstance = (HANDLE)GetWindowLong(hWndFrame, GWL_HINSTANCE);
#else
  hInstance = (HANDLE)GetWindowLong(hWndFrame, GWLP_HINSTANCE);
#endif

  hWndClient = CreateWindow("MDICLIENT",NULL,
			     									WS_CHILD | WS_CLIPSIBLINGS | 
			     									WS_VISIBLE | MDIS_ALLCHILDSTYLES, 
			     									0, 0, 1, 1,
			     									hWndFrame, NULL, 
			     									(HINSTANCE)hInstance, (LPVOID)&ccs);
  return(hWndClient);
}
#endif
