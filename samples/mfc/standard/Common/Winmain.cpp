// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Failure.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>

#ifdef AFX_CORE1_SEG
#pragma code_seg(AFX_CORE1_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// Standard WinMain implementation
//  Can be replaced as long as 'AfxWinInit' is called first

// for cout redefinition : 
#include <io.h> // for _open_osfhandle
#include <fcntl.h> // for _O_TEXT

#ifdef _DEBUG
#define DISPLAYCONSOLE 1 
#endif

int AFXAPI AfxWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
#ifdef DISPLAYCONSOLE 

  // Redirection of standard output to console
  int hCrt;  BOOL rep;  FILE *hf;
  _SYSTEM_INFO lps;
  GetSystemInfo(&lps);
  rep = AllocConsole();
  hCrt = _open_osfhandle((intptr_t) GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT);
  hf = _fdopen( hCrt, "w" );
  *stdout = *hf;
  // stop the buffer on stdout
//  int i = setvbuf( stdout, NULL, _IONBF, 0 );
//  filebuf ff(hCrt);
//  cout = &ff;
  cout<<"This Debug Window is defined in WinMain.cpp and will disappear in release mode"<<endl;

#endif //  DISPLAYCONSOLE  // By Matra

  // create log file for all OCC messages
//  Message::DefaultMessenger()->AddPrinter (new Message_PrinterOStream ("OCCSampleRun.log", Standard_False));

  ASSERT(hPrevInstance == NULL);

  int nReturnCode = -1;
  CWinApp* pApp = AfxGetApp();

// new in 2.0 CAS.CADE uses the standard C++ exception mechanism
/*#ifdef _DEBUG  // By Matra
  // _Function declaratiob here because you can jump to InitFailure
  Standard_ErrorHandler _Function;  
#endif //  _DEBUG  // By Matra
*/
	// AFX internal initialization
	if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		goto InitFailure;

	// App global initializations (rare)
	ASSERT_VALID(pApp);
	if (!pApp->InitApplication())
		goto InitFailure;
	ASSERT_VALID(pApp);

	// Perform specific initializations
	if (!pApp->InitInstance())
	{
		if (pApp->m_pMainWnd != NULL)
		{
			TRACE0("Warning: Destroying non-NULL m_pMainWnd\n");
			pApp->m_pMainWnd->DestroyWindow();
		}
		nReturnCode = pApp->ExitInstance();
		goto InitFailure;
	}
	ASSERT_VALID(pApp);


#ifdef _DEBUG  // By Matra
Application:

// new in 2.0 CAS.CADE uses the standard C++ exception mechanism

 // if(DoesNotAbort(_Function))
	try
    {
	  nReturnCode = pApp->Run();
    }
//  if(_Function.Catches(STANDARD_TYPE(Standard_Failure)))
	catch(Standard_Failure const& anException)
    {
      Standard_SStream ostr;
      ostr<<anException<<"\n\0";
      CString aMsg = ostr.str().c_str();
      MessageBoxW (NULL, aMsg, L"CasCade Error", MB_ICONERROR);
      goto Application; // restart application loop
    }
#else // _DEBUG  // By Matra
	nReturnCode = pApp->Run();
#endif // _DEBUG  // By Matra


	ASSERT_VALID(pApp);

InitFailure:
#ifdef _DEBUG
	// Check for missing AfxLockTempMap calls
	if (AfxGetModuleThreadState()->m_nTempMapLock != 0)
	{
		TRACE1("Warning: Temp map lock count non-zero (%ld).\n",
			AfxGetModuleThreadState()->m_nTempMapLock);
	}

	AfxLockTempMaps();
	AfxUnlockTempMaps();
#endif

	AfxWinTerm();

#ifdef DISPLAYCONSOLE  // By Matra
  // ferme la console pour le cout 
  fclose( stdout );
  //hCrt = _fcloseall();  :-)
  rep = FreeConsole();  
#endif // DISPLAYCONSOLE  // By Matra


	return nReturnCode;
}

/////////////////////////////////////////////////////////////////////////////
