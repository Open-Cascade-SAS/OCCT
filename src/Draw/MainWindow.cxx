// File:	MainWindow.cxx
// Created:	Thu Aug  6 10:00:35 1998
// Author:	Administrateur Atelier MDL
//		<mdl@efalakox.paris1.matra-dtv.fr>



#ifdef WNT


#include <windows.h>
#include <DrawRessource.h>
#include <init.h>
#include <MainWindow.h>
#include <Draw_Window.hxx>
#include <CommandWindow.h>

Standard_Boolean Draw_Interprete(char* command); // Implemente dans Draw.cxx
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
|     Handler pour le message WM_CREATE. Creation de la fenetre de control MDI
|
\*--------------------------------------------------------------------------*/
BOOL CreateProc(HWND hWndFrame)
{
  HWND hWnd;

  // Enregistre le hWnd dans la fenetre principale dans extra memory en 0
  if (hWnd = CreateMDIClientWindow(hWndFrame))
    SetWindowLong(hWndFrame, CLIENTWND, (LONG)hWnd);
  return(TRUE);
}


/*--------------------------------------------------------------------------*\
|  COMMAND PROCEDURE
|  		Handler pour le message WM_COMMAND   
|
\*--------------------------------------------------------------------------*/
BOOL CommandProc(HWND hWndFrame, WPARAM wParam, LPARAM lParam)
{
  HWND hWndClient; // Handle sur la fenetre MDI
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
|     Handler pour le message WM_DESTROY.
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

