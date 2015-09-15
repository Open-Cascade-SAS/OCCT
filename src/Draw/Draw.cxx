// Created on: 1993-08-13
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Failure.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Draw_Window.hxx>
#include <gp_Pnt2d.hxx>
#include <OSD.hxx>
#include <OSD_Environment.hxx>
#include <OSD_SharedLibrary.hxx>
#include <OSD_Timer.hxx>
#include <Plugin_MapOfFunctions.hxx>
#include <Resource_Manager.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Version.hxx>
#include <TCollection_AsciiString.hxx>

#include <tcl.h>
// on MSVC, use #pragma to define name of the Tcl library to link with,
// depending on Tcl version number
#ifdef _MSC_VER
// two helper macros are needed to convert version number macro to string literal
#define STRINGIZE1(a) #a
#define STRINGIZE2(a) STRINGIZE1(a)
#pragma comment (lib, "tcl" STRINGIZE2(TCL_MAJOR_VERSION) STRINGIZE2(TCL_MINOR_VERSION) ".lib")
#pragma comment (lib, "tk"  STRINGIZE2(TCL_MAJOR_VERSION) STRINGIZE2(TCL_MINOR_VERSION) ".lib")
#undef STRINGIZE2
#undef STRINGIZE1
#endif

extern Standard_Boolean Draw_ParseFailed;

Standard_EXPORT Draw_Viewer dout;
Standard_EXPORT Draw_Interpretor theCommands;
Standard_EXPORT Standard_Boolean Draw_Batch;
Standard_EXPORT Standard_Boolean Draw_Spying = Standard_False;
Standard_EXPORT Standard_Boolean Draw_Chrono = Standard_False;
Standard_EXPORT Standard_Boolean Draw_VirtualWindows = Standard_False;
Standard_EXPORT Standard_Boolean ErrorMessages = Standard_True;

static const char* ColorNames[MAXCOLOR] = {
  "White","Red","Green","Blue","Cyan","Gold","Magenta",
  "Maroon","Orange","Pink","Salmon","Violet","Yellow","Khaki","Coral"
  };

filebuf Draw_Spyfile;

static ostream spystream(&Draw_Spyfile);

static   Standard_Boolean XLoop;

static Handle(Draw_ProgressIndicator) PInd = NULL;

Standard_EXPORT Standard_Boolean Draw_Interprete(const char* command);
// true if complete command

// *******************************************************************
// read an init file
// *******************************************************************
#ifdef _WIN32
extern console_semaphore_value volatile console_semaphore;
extern char console_command[1000];
#endif

static void ReadInitFile (const TCollection_AsciiString& theFileName)
{
  TCollection_AsciiString aPath = theFileName;
#ifdef _WIN32
  if (!Draw_Batch) {
    try {
      aPath.ChangeAll ('\\', '/');

      Sprintf(console_command, "source \"%.980s\"", aPath.ToCString());
      console_semaphore = HAS_CONSOLE_COMMAND;
      while (console_semaphore == HAS_CONSOLE_COMMAND)
        Sleep(10);
    }
    catch(...) {
      cout << "Error while reading a script file." << endl;
      ExitProcess(0);
    }
  } else {
#endif
    char* com = new char [aPath.Length() + strlen ("source ") + 2];
    Sprintf (com, "source %s", aPath.ToCString());
    Draw_Interprete (com);
    delete [] com;
#ifdef _WIN32
  }
#endif
}

//=======================================================================
//function :
//purpose  : Set/Get Progress Indicator
//=======================================================================
void Draw::SetProgressBar(const Handle(Draw_ProgressIndicator)& thePI)
{
  PInd = thePI;
}

Handle(Draw_ProgressIndicator) Draw::GetProgressBar()
{
  return PInd;
}

#ifndef _WIN32
/*--------------------------------------------------------*\
|  exitProc: finalization handler for Tcl/Tk thread. Forces parent process to die
\*--------------------------------------------------------*/
void exitProc(ClientData /*dc*/)
{
  if (!Draw_Batch) {
    for (Standard_Integer id = 0; id < MAXVIEW; id++)
      dout.DeleteView(id);
  }
}
#endif

// *******************************************************************
// main
// *******************************************************************
#ifdef _WIN32
//Standard_EXPORT void Draw_Appli(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lps
Standard_EXPORT void Draw_Appli(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszLine, int nShow,const FDraw_InitAppli Draw_InitAppli)
#else
void Draw_Appli(Standard_Integer argc, char** argv,const FDraw_InitAppli Draw_InitAppli)
#endif
{

// prepend extra DLL search path to override system libraries like opengl32.dll
#ifdef _WIN32
  OSD_Environment aUserDllEnv ("CSF_UserDllPath");
  TCollection_AsciiString aUserDllPath = aUserDllEnv.Value();
  if (!aUserDllPath.IsEmpty())
  {
    // This function available since Win XP SP1 #if (_WIN32_WINNT >= 0x0502).
    // We retrieve dynamically here (kernel32 should be always preloaded).
    typedef BOOL (WINAPI *SetDllDirectoryA_t)(const char* thePathName);
    HMODULE aKern32Module = GetModuleHandleA ("kernel32");
    SetDllDirectoryA_t aFunc = (aKern32Module != NULL)
                             ? (SetDllDirectoryA_t )GetProcAddress (aKern32Module, "SetDllDirectoryA") : NULL;
    if (aFunc != NULL)
    {
      aFunc (aUserDllPath.ToCString());
    }
    else
    {
      //std::cerr << "SetDllDirectoryA() is not available on this system!\n";
    }
    if (aKern32Module != NULL)
    {
      FreeLibrary (aKern32Module);
    }
  }
#endif

  // *****************************************************************
  // analyze arguments
  // *****************************************************************
  Draw_Batch = Standard_False;
  TCollection_AsciiString aRunFile, aCommand;
  Standard_Integer i;
  Standard_Boolean isInteractiveForced = Standard_False;

#ifdef _WIN32
  // On NT command line arguments are in the lpzline and not in argv
  int argc = 0;
  const int MAXARGS = 1024;
  const char* argv[MAXARGS];
  for (const char* p = strtok(lpszLine, " \t"); p != NULL; p = strtok(NULL, " \t")) {
    argv[argc++] = p;
  }
#endif

  // parse command line
  for (i = 1; i < argc; i++) {
    if (strcasecmp (argv[i], "-h") == 0 || strcasecmp (argv[i], "--help") == 0)
    {
      cout << "Open CASCADE " << OCC_VERSION_STRING_EXT << " DRAW Test Harness" << endl << endl;
      cout << "Options: " << endl;
      cout << "  -b: batch mode (no GUI, no viewers)" << endl;
      cout << "  -v: no GUI, use virtual (off-screen) windows for viewers" << endl;
      cout << "  -i: interactive mode" << endl;
      cout << "  -f file: execute script from file" << endl;
      cout << "  -c command args...: execute command (with optional arguments)" << endl << endl;
      cout << "Options -b, -v, and -i are mutually exclusive." << endl;
      cout << "If -c or -f are given, -v is default; otherwise default is -i." << endl;
      cout << "Options -c and -f are alternatives and should be at the end " << endl;
      cout << "of the command line. " << endl;
      cout << "Option -c can accept set of commands separated by ';'." << endl;
      return;
    }
    else if (strcasecmp (argv[i], "-b") == 0)
      Draw_Batch = Standard_True;
    else if (strcasecmp (argv[i], "-v") == 0) {
      // force virtual windows
      Draw_VirtualWindows = Standard_True;
    } else if (strcasecmp (argv[i], "-i") == 0) {
      // force interactive
      Draw_VirtualWindows = Standard_False;
      isInteractiveForced = Standard_True;
    } else if (strcasecmp (argv[i], "-f") == 0) { // -f option should be LAST!
      Draw_VirtualWindows = !isInteractiveForced;
      if (++i < argc) {
        aRunFile = TCollection_AsciiString (argv[i]);
      }
      break;
    } else if (strcasecmp (argv[i], "-c") == 0) { // -c option should be LAST!
      Draw_VirtualWindows = !isInteractiveForced;
      if (++i < argc) {
        aCommand = TCollection_AsciiString (argv[i]);
      }
      while (++i < argc) {
        aCommand.AssignCat (" ");
        aCommand.AssignCat (argv[i]);
      }
      break;
    } else {
      cout << "Error: unsupported option " << argv[i] << endl;
    }
  }

  // *****************************************************************
  // set signals
  // *****************************************************************
  OSD::SetSignal(Standard_False);

#ifdef _WIN32
  // in interactive mode, force Windows to report dll loading problems interactively
  if ( ! Draw_VirtualWindows && ! Draw_Batch )
    ::SetErrorMode (0);
#endif

  // *****************************************************************
  // init X window and create display
  // *****************************************************************
#ifdef _WIN32
  HWND hWnd = NULL;
#endif

  if (!Draw_Batch)
#ifdef _WIN32
    Draw_Batch=!Init_Appli(hInst, hPrevInst, nShow, hWnd);
#else
    Draw_Batch=!Init_Appli();
#endif
  else
    cout << "DRAW is running in batch mode" << endl;

  XLoop = !Draw_Batch;
  if (XLoop) {
    // Default colors
    for (i=0;i<MAXCOLOR;i++) {
      if (!dout.DefineColor(i,ColorNames[i]))
	cout <<"Could not allocate default color "<<ColorNames[i]<<endl;
    }
  }

  // *****************************************************************
  // set maximum precision for cout
  // *****************************************************************
  cout.precision(15);

  // *****************************************************************
  // standard commands
  // *****************************************************************
  Draw::BasicCommands(theCommands);
  Draw::VariableCommands(theCommands);
  Draw::UnitCommands(theCommands);
  if (!Draw_Batch) Draw::GraphicCommands(theCommands);

  // *****************************************************************
  // user commands
  // *****************************************************************
  Draw_InitAppli(theCommands);

#ifndef _WIN32
  Tcl_CreateExitHandler(exitProc, 0);
#endif

  // *****************************************************************
  // read init files
  // *****************************************************************
  // default

  if (getenv ("DRAWDEFAULT") == NULL)
  {
    if (getenv ("CASROOT") == NULL)
    {
#ifdef _WIN32
      ReadInitFile ("ddefault");
#else
      cout << " the CASROOT variable is mandatory to Run OpenCascade "<< endl;
      cout << "No default file" << endl;
#endif
    }
    else
    {
      TCollection_AsciiString aDefStr (getenv ("CASROOT"));
      aDefStr += "/src/DrawResources/DrawDefault";
      ReadInitFile (aDefStr);
    }
  }
  else
  {
    ReadInitFile (getenv ("DRAWDEFAULT"));
  }

  // read commands from file
  if (!aRunFile.IsEmpty()) {
    ReadInitFile (aRunFile);
    // provide a clean exit, this is useful for some analysis tools
    if ( ! isInteractiveForced )
#ifndef _WIN32
      return;
#else
      ExitProcess(0);
#endif
  }

  // execute command from command line
  if (!aCommand.IsEmpty()) {
    Draw_Interprete (aCommand.ToCString());
    // provide a clean exit, this is useful for some analysis tools
    if ( ! isInteractiveForced )
#ifndef _WIN32
      return;
#else
      ExitProcess(0);
#endif
  }

  // *****************************************************************
  // X loop
  // *****************************************************************
  if (XLoop) {
#ifdef _WIN32
    Run_Appli(hWnd);
#else
    Run_Appli(Draw_Interprete);
#endif
  }
  else
  {
    char cmd[255];
    do {
      cout << "Viewer>";
      i = -1;
      do {
        cin.get(cmd[++i]);
      } while ((cmd[i] != '\n') && (!cin.fail()));
      cmd[i] = '\0';
    } while (Draw_Interprete(cmd) != (unsigned int ) -2);
  }
#ifdef _WIN32
  // Destruction de l'application
  Destroy_Appli(hInst);
#endif
}
//#endif

// User functions called before and after each command
void (*Draw_BeforeCommand)() = NULL;
void (*Draw_AfterCommand)(Standard_Integer) = NULL;

Standard_Boolean Draw_Interprete(const char* com)
{

  static Standard_Boolean first = Standard_True;
  static Tcl_DString command;

  if (first) {
    first = Standard_False;
    Tcl_DStringInit(&command);
  }

#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 1)))
  // OCC63: Since Tcl 8.1 it uses UTF-8 encoding for internal representation of strings
  Tcl_ExternalToUtfDString ( NULL, com, -1, &command );
#else
  Tcl_DStringAppend(&command,com,-1);
#endif

  if (!theCommands.Complete(Tcl_DStringValue(&command)))
    return Standard_False;

  // *******************************************************************
  // Command interpreter
  // *******************************************************************

//  Standard_Integer i = 0;
//  Standard_Integer j = 0;

  Standard_Boolean wasspying = Draw_Spying;

  OSD_Timer tictac;
  Standard_Boolean hadchrono = Draw_Chrono;
  if (hadchrono) tictac.Start();

  if (Draw_BeforeCommand) (*Draw_BeforeCommand) ();

  Standard_Integer c;

  c = theCommands.RecordAndEval(Tcl_DStringValue(&command));

  if (Draw_AfterCommand) (*Draw_AfterCommand)(c);

  if (wasspying && Draw_Spying) {
    if (c > 0) spystream << "# ";
    spystream << Tcl_DStringValue(&command) << "\n";
  }

  dout.Flush();

  if (*theCommands.Result())
    cout << theCommands.Result() << endl;

  if (Draw_Chrono && hadchrono) {
    tictac.Stop();
    tictac.Show();
  }

  Tcl_DStringFree(&command);

  return Standard_True;
}

//
// for TCl
//

Standard_Integer Tcl_AppInit (Tcl_Interp *)
{
  return 0;
}

//
// for debug call
//



Standard_Integer  Draw_Call (char *c)
{
   Standard_Integer r = theCommands.Eval(c);
   cout << theCommands.Result() << endl;
   return r;
}

//=================================================================================
//
//=================================================================================
void Draw::Load(Draw_Interpretor& theDI, const TCollection_AsciiString& theKey,
		const TCollection_AsciiString& theResourceFileName,
		TCollection_AsciiString& theDefaultsDirectory,
		TCollection_AsciiString& theUserDefaultsDirectory,
		const Standard_Boolean Verbose ) {

  static Plugin_MapOfFunctions theMapOfFunctions;
  OSD_Function f;

  if(!theMapOfFunctions.IsBound(theKey)) {

    Handle(Resource_Manager) aPluginResource = new Resource_Manager(theResourceFileName.ToCString(), theDefaultsDirectory, theUserDefaultsDirectory, Verbose);

    if(!aPluginResource->Find(theKey.ToCString())) {
      Standard_SStream aMsg; aMsg << "Could not find the resource:";
      aMsg << theKey.ToCString()<< endl;
      cout << "could not find the resource:"<<theKey.ToCString()<< endl;
      Draw_Failure::Raise(aMsg);
    }

    TCollection_AsciiString aPluginLibrary("");
#ifndef _WIN32
    aPluginLibrary += "lib";
#endif
    aPluginLibrary +=  aPluginResource->Value(theKey.ToCString());
#ifdef _WIN32
    aPluginLibrary += ".dll";
#elif __APPLE__
    aPluginLibrary += ".dylib";
#elif defined (HPUX) || defined(_hpux)
    aPluginLibrary += ".sl";
#else
    aPluginLibrary += ".so";
#endif
    OSD_SharedLibrary aSharedLibrary(aPluginLibrary.ToCString());
    if(!aSharedLibrary.DlOpen(OSD_RTLD_LAZY)) {
      TCollection_AsciiString error(aSharedLibrary.DlError());
      Standard_SStream aMsg; aMsg << "Could not open: ";
      aMsg << aPluginResource->Value(theKey.ToCString());
      aMsg << "; reason: ";
      aMsg << error.ToCString();
#ifdef OCCT_DEBUG
      cout << "could not open: "  << aPluginResource->Value(theKey.ToCString())<< " ; reason: "<< error.ToCString() << endl;
#endif
      Draw_Failure::Raise(aMsg);
    }
    f = aSharedLibrary.DlSymb("PLUGINFACTORY");
    if( f == NULL ) {
      TCollection_AsciiString error(aSharedLibrary.DlError());
      Standard_SStream aMsg; aMsg << "Could not find the factory in: ";
      aMsg << aPluginResource->Value(theKey.ToCString());
      aMsg << error.ToCString();
      Draw_Failure::Raise(aMsg);
    }
    theMapOfFunctions.Bind(theKey, f);
  }
  else
    f = theMapOfFunctions(theKey);

//   void (*fp) (Draw_Interpretor&, const TCollection_AsciiString&) = NULL;
//   fp = (void (*)(Draw_Interpretor&, const TCollection_AsciiString&)) f;
//   (*fp) (theDI, theKey);

  void (*fp) (Draw_Interpretor&) = NULL;
  fp = (void (*)(Draw_Interpretor&)) f;
  (*fp) (theDI);

}
