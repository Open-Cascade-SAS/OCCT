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

#define COMMANDCLASS "COMMANDWINDOW"
#define COMMANDTITLE "Command Window"

#include <CommandWindow.h>
#include <Draw_Window.hxx>
#include <MainWindow.h>
#include <Draw_Appli.hxx>



/****************************************************\
*  CommandWindow.cxx :
*
\****************************************************/



Standard_Boolean Draw_Interprete(char*); // Implement in draw.cxx

#define CLIENTWND 0

#define PROMPT "Command >> "
#define COMMANDSIZE 1000 // Max nb of characters for a command


// Definition of global variables
#ifdef STRICT
  WNDPROC OldEditProc;  // Save the standard procedure of the edition (sub-class)
#else
  FARPROC OldEditProc;
#endif

/*--------------------------------------------------------*\
|  CREATE COMMAND WINDOW PROCEDURE
\*--------------------------------------------------------*/
HWND CreateCommandWindow(HWND hWnd, int nitem)
{
  HINSTANCE       hInstance;
  
#ifndef _WIN64
  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
#else
  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWLP_HINSTANCE);
#endif
	HWND hWndCommand = (CreateWindow(COMMANDCLASS, COMMANDTITLE,
			    												WS_CLIPCHILDREN | WS_OVERLAPPED |
			    												WS_THICKFRAME | WS_CAPTION	,
			    												0, 0,
			    												400, 100,
			    												hWnd, NULL, hInstance, NULL));

	ShowWindow(hWndCommand, SW_SHOW);	
	return hWndCommand;
}


/*--------------------------------------------------------*\
|  COMMAND WINDOW PROCEDURE
\*--------------------------------------------------------*/
LONG APIENTRY CommandProc(HWND hWnd, UINT wMsg, WPARAM wParam, LONG lParam )
{
  HWND hWndEdit;
  int index; // Nb of characters in the buffer of hWndEdit
  MINMAXINFO* lpmmi;

  switch(wMsg)
  {
    case WM_CREATE :
					CommandCreateProc(hWnd);
					hWndEdit = (HWND)GetWindowLong(hWnd, CLIENTWND);
					SendMessage(hWndEdit,EM_REPLACESEL, 0,(LPARAM)PROMPT);
					break;

    case WM_GETMINMAXINFO :
          lpmmi = (LPMINMAXINFO)lParam;
          lpmmi->ptMinTrackSize.x = 200;
          lpmmi->ptMinTrackSize.y = 50;           
          break;

    case WM_SIZE :
    			hWndEdit = (HWND)GetWindowLong(hWnd, CLIENTWND);          
    			MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            // Place the cursor at the end of the buffer
          index =  SendMessage(hWnd, WM_GETTEXTLENGTH, 0l, 0l);
          SendMessage(hWnd, EM_SETSEL, index, index); 
    			break;

    case WM_SETFOCUS :
    			hWndEdit = (HWND)GetWindowLong(hWnd, CLIENTWND);
          SetFocus(hWndEdit);
          break;

    default :
					return(DefWindowProc(hWnd, wMsg, wParam, lParam));
  }
  return(0l);
}



LONG APIENTRY EditProc(HWND, UINT, WPARAM, LONG);
/*--------------------------------------------------------*\
|  COMMAND CREATE PROCEDURE
\*--------------------------------------------------------*/
BOOL CommandCreateProc(HWND hWnd)
{

#ifndef _WIN64
  HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
#else
  HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
#endif
  HWND hWndEdit = CreateWindow("EDIT",NULL,
  			  WS_CHILD | WS_VISIBLE | WS_VSCROLL |
  			  ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
  			  0, 0, 0, 0,
  			  hWnd, 0,
  			  hInstance, NULL);

    // Save hWndEdit in the extra memory in 0 of CommandWindow
  if (hWndEdit)
    SetWindowLong(hWnd, CLIENTWND, (LONG)hWndEdit);

    // Sub-Class of the window
    //-------
    // Save the pointer on the existing procedure
  #ifdef STRICT
   #ifndef _WIN64
    OldEditProc = (WNDPROC)GetWindowLong(hWndEdit, GWL_WNDPROC);
   #else
    OldEditProc = (WNDPROC)GetWindowLong(hWndEdit, GWLP_WNDPROC);
   #endif // _WIN64
  #else
    OldEditProc = (FARPROC)GetWindowLong(hWndEdit, GWL_WNDPROC);
  #endif
    // Implement the new function
#ifndef _WIN64
  SetWindowLong(hWndEdit, GWL_WNDPROC, (LONG) EditProc);
#else
  SetWindowLong(hWndEdit, GWLP_WNDPROC, (LONG) EditProc);
#endif
  return(TRUE);
}


/*--------------------------------------------------------*\
|  GET COMMAND
|    
\*--------------------------------------------------------*/
int GetCommand(HWND hWnd, char* buffer)
{
  int again = 1;
  char temp[COMMANDSIZE]="";

  int nbLine = SendMessage(hWnd, EM_GETLINECOUNT, 0l, 0l);
  
  int nbChar = 0;
  buffer[0]='\0';
  while ( again && nbLine > -1 && nbChar < COMMANDSIZE-1)
    {
      strcat(buffer, strrev(temp));
      // Initialization of the 1st WORD to the nb of characters to read 
      WORD* nbMaxChar = (WORD*)temp;
      *nbMaxChar = COMMANDSIZE-1;
      
      int nbCharRead = SendMessage(hWnd, EM_GETLINE, nbLine-1, (LPARAM)temp);
      nbChar += nbCharRead ;
      int cmp = strncmp(temp, PROMPT, 10);
      temp[nbCharRead]='\0';
      if( cmp == 0 )
	{
	  strcat(buffer, strrev(temp));
	  again = 0;
	}
      nbLine -= 1;
    }	
  strrev(buffer);
  return nbChar;
}

extern console_semaphore_value volatile console_semaphore;
extern char console_command[1000];

/*--------------------------------------------------------*\
|  EDIT WINDOW PROCEDURE
\*--------------------------------------------------------*/
LONG APIENTRY EditProc(HWND hWnd, UINT wMsg, WPARAM wParam, LONG lParam )
{
  char buffer[COMMANDSIZE];	
	POINT pos;
	BOOL rep;
	static int nbline; // Process the buffer of the edit window 
  int index;
    
  switch(wMsg)
  {
  case WM_CHAR :
    if (console_semaphore != WAIT_CONSOLE_COMMAND)
      return 0l;
    			switch(LOWORD(wParam))
    			{
              // Overload of character \n
    	  		case 0x0d :    	      
									GetCommand(hWnd, buffer);									
    	      			  // Standard processing
			    	      CallWindowProc(OldEditProc, hWnd, wMsg, wParam, lParam);
    				        // Display of PROMPT
									rep = GetCaretPos(&pos);
    	      			SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)PROMPT);							
                    // Display the command in the console
                  cout << buffer << endl; 
									/*if (Draw_Interprete(buffer+strlen(PROMPT))== -2)
									    DestroyProc(hWnd); */ 
									strcpy(console_command, buffer+strlen(PROMPT));
									console_semaphore = HAS_CONSOLE_COMMAND;
									  // Purge the buffer
                  nbline = SendMessage(hWnd, EM_GETLINECOUNT, 0l, 0l);
									if(nbline > 200)
									{
                      nbline = 0;
											GetCommand(hWnd, buffer);
                      index = SendMessage(hWnd, EM_LINEINDEX, 100, 0);
											SendMessage(hWnd, EM_SETSEL, 0, index);			
											SendMessage(hWnd, WM_CUT, 0, 0);
                        // Place the cursor at the end of text
                      index =  SendMessage(hWnd, WM_GETTEXTLENGTH, 0l, 0l);
                      SendMessage(hWnd, EM_SETSEL, index, index);                      
									}
    				      return(0l);
    	      			break;
                default :
                  if (IsAlphanumeric((Standard_Character)LOWORD(wParam)))
                  {
                      // Place the cursor at the end of text before display
                    index =  SendMessage(hWnd, WM_GETTEXTLENGTH, 0l, 0l);
                    SendMessage(hWnd, EM_SETSEL, index, index);
                    CallWindowProc(OldEditProc, hWnd, wMsg, wParam, lParam);                    
                    return 0l;
                  }                  
                  break;
    			}       
    			break;
  case WM_KEYDOWN:
    if (console_semaphore != WAIT_CONSOLE_COMMAND) 
      return 0l;									
  }
  return CallWindowProc(OldEditProc, hWnd, wMsg, wParam, lParam);
}
#endif


