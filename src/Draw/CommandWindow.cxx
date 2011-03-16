// File:	CommandWindow.cxx
// Created:	Thu Aug  6 09:59:19 1998
// Author:	Administrateur Atelier MDL
//		<mdl@efalakox.paris1.matra-dtv.fr>



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



Standard_Boolean Draw_Interprete(char*); // Implemente dans draw.cxx

#define CLIENTWND 0

#define PROMPT "Command >> "
#define COMMANDSIZE 1000 // Nb caracteres maximun pour un commande


// Definition des varaibles globales
#ifdef STRICT
  WNDPROC OldEditProc;  // Sauvegarde la procedure standard de la fenetre d'edition (Sous Classement)
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
  int index; // Nombre de caractere dans le buffer de hWndEdit
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
            // Place le curseur a la fin du buffer
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

    // Enregistrement hWndEdit deans l'extra memory en 0 de CommandWindow
  if (hWndEdit)
    SetWindowLong(hWnd, CLIENTWND, (LONG)hWndEdit);

    // Sous Classement de la fenetre
    //-------
    // Sauvegarde du pointeur sur la procedure existante
  #ifdef STRICT
   #ifndef _WIN64
    OldEditProc = (WNDPROC)GetWindowLong(hWndEdit, GWL_WNDPROC);
   #else
    OldEditProc = (WNDPROC)GetWindowLong(hWndEdit, GWLP_WNDPROC);
   #endif // _WIN64
  #else
    OldEditProc = (FARPROC)GetWindowLong(hWndEdit, GWL_WNDPROC);
  #endif
    // Mise en place de la nouvelle fonction
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
      // Initialisation du 1er WORD de temp au nombre de caracteres a lire 
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
	static int nbline; // Taille du buffer de la fenetre d`edition 
  int index;
    
  switch(wMsg)
  {
  case WM_CHAR :
    if (console_semaphore != WAIT_CONSOLE_COMMAND)
      return 0l;
    			switch(LOWORD(wParam))
    			{
              // Surcharge du caractere \n
    	  		case 0x0d :    	      
									GetCommand(hWnd, buffer);									
    	      			  // Traitement standard
			    	      CallWindowProc(OldEditProc, hWnd, wMsg, wParam, lParam);
    				        // Affichage du PROMPT
									rep = GetCaretPos(&pos);
    	      			SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)PROMPT);							
                    // Affiche la commande dans la console
                  cout << buffer << endl; 
									/*if (Draw_Interprete(buffer+strlen(PROMPT))== -2)
									    DestroyProc(hWnd); */ 
									strcpy(console_command, buffer+strlen(PROMPT));
									console_semaphore = HAS_CONSOLE_COMMAND;
									  // Purge du buffer
                  nbline = SendMessage(hWnd, EM_GETLINECOUNT, 0l, 0l);
									if(nbline > 200)
									{
                      nbline = 0;
											GetCommand(hWnd, buffer);
                      index = SendMessage(hWnd, EM_LINEINDEX, 100, 0);
											SendMessage(hWnd, EM_SETSEL, 0, index);			
											SendMessage(hWnd, WM_CUT, 0, 0);
                        // Place le curseur en fin de text
                      index =  SendMessage(hWnd, WM_GETTEXTLENGTH, 0l, 0l);
                      SendMessage(hWnd, EM_SETSEL, index, index);                      
									}
    				      return(0l);
    	      			break;
                default :
                  if (IsAlphanumeric((Standard_Character)LOWORD(wParam)))
                  {
                      // Place le curseur en fin de texte avant affichage
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


