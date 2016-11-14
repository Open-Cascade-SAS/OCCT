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

#include <CommandWindow.h>
#include <Draw_Window.hxx>
#include <MainWindow.h>
#include <Draw_Appli.hxx>
#include <TCollection_AsciiString.hxx>

#define CLIENTWND 0

#define THE_PROMPT L"Command >> "
#define COMMANDSIZE 1000 // Max nb of characters for a command

Standard_Boolean Draw_Interprete (const char* command);

namespace
{
  // Definition of global variables
  static WNDPROC OldEditProc;  // Save the standard procedure of the edition (sub-class)
}

/*--------------------------------------------------------*\
|  CREATE COMMAND WINDOW PROCEDURE
\*--------------------------------------------------------*/
HWND CreateCommandWindow(HWND hWnd, int /*nitem*/)
{
  HINSTANCE hInstance = (HINSTANCE )GetWindowLongPtrW (hWnd, GWLP_HINSTANCE);

	HWND hWndCommand = CreateWindowW (COMMANDCLASS, COMMANDTITLE,
                                    WS_CLIPCHILDREN | WS_OVERLAPPED | WS_THICKFRAME | WS_CAPTION,
                                    0, 0, 400, 100,
                                    hWnd, NULL, hInstance, NULL);

	ShowWindow(hWndCommand, SW_SHOW);	
	return hWndCommand;
}


/*--------------------------------------------------------*\
|  COMMAND WINDOW PROCEDURE
\*--------------------------------------------------------*/
LRESULT APIENTRY CommandProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
  switch (wMsg)
  {
    case WM_CREATE:
    {
      CommandCreateProc (hWnd);
      HWND hWndEdit = (HWND )GetWindowLongPtrW (hWnd, CLIENTWND);
      SendMessageW (hWndEdit, EM_REPLACESEL, 0, (LPARAM )THE_PROMPT);
      return 0;
    }
    case WM_GETMINMAXINFO:
    {
      MINMAXINFO* lpmmi = (MINMAXINFO* )lParam;
      lpmmi->ptMinTrackSize.x = 200;
      lpmmi->ptMinTrackSize.y = 50;
      return 0;
    }
    case WM_SIZE:
    {
      HWND hWndEdit = (HWND )GetWindowLongPtrW(hWnd, CLIENTWND);
      MoveWindow (hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
      // Place the cursor at the end of the buffer
      // Nb of characters in the buffer of hWndEdit
      LRESULT index = SendMessageW (hWnd, WM_GETTEXTLENGTH, 0l, 0l);
      SendMessageW (hWnd, EM_SETSEL, index, index);
      return 0;
    }
    case WM_SETFOCUS:
    {
      HWND hWndEdit = (HWND )GetWindowLongPtrW (hWnd, CLIENTWND);
      SetFocus (hWndEdit);
      return 0;
    }
  }
  return DefWindowProcW(hWnd, wMsg, wParam, lParam);
}

LRESULT APIENTRY EditProc(HWND, UINT, WPARAM, LPARAM);
/*--------------------------------------------------------*\
|  COMMAND CREATE PROCEDURE
\*--------------------------------------------------------*/
BOOL CommandCreateProc(HWND hWnd)
{
  HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtrW(hWnd, GWLP_HINSTANCE);
  HWND hWndEdit = CreateWindowW (L"EDIT", NULL,
                                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
                                 0, 0, 0, 0,
                                 hWnd, 0,
                                 hInstance, NULL);

  // Save hWndEdit in the extra memory in 0 of CommandWindow
  if (hWndEdit != NULL)
  {
    SetWindowLongPtrW  (hWnd, CLIENTWND, (LONG_PTR )hWndEdit);
  }

  // Sub-Class of the window
  //-------
  // Save the pointer on the existing procedure
  OldEditProc = (WNDPROC )GetWindowLongPtrW (hWndEdit, GWLP_WNDPROC);
  // Implement the new function
  SetWindowLongPtrW (hWndEdit, GWLP_WNDPROC, (LONG_PTR) EditProc);
  return TRUE;
}

/*--------------------------------------------------------*\
|  GET COMMAND
|    
\*--------------------------------------------------------*/
int GetCommand (HWND hWnd, wchar_t* theBuffer)
{
  bool isAgain = true;
  wchar_t aTempBuff[COMMANDSIZE] = L"";

  int aNbLine = (int )SendMessageW (hWnd, EM_GETLINECOUNT, 0l, 0l);
  int aNbChar = 0;
  theBuffer[0] = L'\0';
  while (isAgain && aNbLine > -1 && aNbChar < COMMANDSIZE - 1)
  {
    wcscat (theBuffer, _wcsrev (aTempBuff));
    // Initialization of the 1st WORD to the nb of characters to read
    WORD* aNbMaxChar = (WORD* )aTempBuff;
    *aNbMaxChar = COMMANDSIZE - 1;
      
    const int aNbCharRead = (int )SendMessageW (hWnd, EM_GETLINE, aNbLine - 1, (LPARAM )aTempBuff);
    aNbChar += aNbCharRead;
    const bool isPromp = wcsncmp (aTempBuff, THE_PROMPT, 10) == 0;
    aTempBuff[aNbCharRead]='\0';
    if (isPromp)
    {
      wcscat (theBuffer, _wcsrev (aTempBuff));
      isAgain = false;
    }
    aNbLine -= 1;
  }
  _wcsrev (theBuffer);
  return aNbChar;
}

extern console_semaphore_value volatile console_semaphore;
extern wchar_t console_command[1000];

/*--------------------------------------------------------*\
|  EDIT WINDOW PROCEDURE
\*--------------------------------------------------------*/
LRESULT APIENTRY EditProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	static LRESULT nbline; // Process the buffer of the edit window 
  switch (wMsg)
  {
    case WM_CHAR:
    {
      if (console_semaphore != WAIT_CONSOLE_COMMAND)
      {
        return 0;
      }
      switch (LOWORD(wParam))
      {
        // Overload of character \n
        case 0x0d:
        {
          wchar_t aCmdBuffer[COMMANDSIZE];
          GetCommand (hWnd, aCmdBuffer);
          // Standard processing
          CallWindowProcW (OldEditProc, hWnd, wMsg, wParam, lParam);
          // Display of PROMPT
          POINT pos;
          GetCaretPos (&pos);
          SendMessageW (hWnd, EM_REPLACESEL, 0, (LPARAM )THE_PROMPT);
          // Display the command in the console
          std::wcout << aCmdBuffer << std::endl;
          //TCollection_AsciiString aCmdUtf8 (aCmdBuffer + sizeof(THE_PROMPT) / sizeof(wchar_t) - 1);
          //Draw_Interprete (aCmdUtf8.ToCString());
          //if (toExit) { DestroyProc (hWnd); }
          wcscpy (console_command, aCmdBuffer + sizeof(THE_PROMPT) / sizeof(wchar_t) - 1);
          console_semaphore = HAS_CONSOLE_COMMAND;
          // Purge the buffer
          nbline = SendMessageW (hWnd, EM_GETLINECOUNT, 0l, 0l);
          if (nbline > 200)
          {
            nbline = 0;
            GetCommand (hWnd, aCmdBuffer);
            LRESULT index = SendMessageW (hWnd, EM_LINEINDEX, 100, 0);
            SendMessageW (hWnd, EM_SETSEL, 0, index);
            SendMessageW (hWnd, WM_CUT, 0, 0);
            // Place the cursor at the end of text
            index =  SendMessageW (hWnd, WM_GETTEXTLENGTH, 0l, 0l);
            SendMessageW (hWnd, EM_SETSEL, index, index);
          }
          return 0;
        }
        default:
        {
          if (IsAlphanumeric ((Standard_Character)LOWORD(wParam)))
          {
            // Place the cursor at the end of text before display
            LRESULT index =  SendMessageW (hWnd, WM_GETTEXTLENGTH, 0l, 0l);
            SendMessageW (hWnd, EM_SETSEL, index, index);
            CallWindowProcW (OldEditProc, hWnd, wMsg, wParam, lParam);
            return 0;
          }
          break;
        }
      }
      break;
    }
    case WM_KEYDOWN:
    {
      if (console_semaphore != WAIT_CONSOLE_COMMAND)
      {
        return 0;
      }
      break;
    }
  }
  return CallWindowProcW (OldEditProc, hWnd, wMsg, wParam, lParam);
}
#endif
