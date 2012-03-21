// Created on: 1993-08-13
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Draw.ixx>

#if defined(HAVE_TIME_H) || defined(WNT)
# include <time.h>
#endif

#include <Draw_Appli.hxx>
#include <OSD.hxx>
#include <OSD_Timer.hxx>

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif


#include <Draw_Window.hxx>
#include <gp_Pnt2d.hxx>

#include <Standard_Stream.hxx>

#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <tcl.h>

#include <Draw_MapOfFunctions.hxx>
#include <OSD_SharedLibrary.hxx>
#include <Resource_Manager.hxx>
#include <Draw_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_ErrorHandler.hxx>
extern Standard_Boolean Draw_ParseFailed;
#ifndef WNT
extern Standard_Boolean Draw_LowWindows;
#endif

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

Standard_EXPORT Standard_Boolean Draw_Interprete(char* command);
// true if complete command

//#ifndef WNT

// *******************************************************************
// read an init file
// *******************************************************************
#ifdef WNT
extern console_semaphore_value volatile console_semaphore;
extern char console_command[1000];
#endif

static void ReadInitFile (const TCollection_AsciiString& theFileName)
{
  TCollection_AsciiString aPath = theFileName;
#ifdef WNT
  if (!Draw_Batch) {
    try {
      OCC_CATCH_SIGNALS
      aPath.ChangeAll ('\\', '/');

      sprintf(console_command, "source \"%s\"", aPath.ToCString());
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
    sprintf (com, "source %s", aPath.ToCString());
    Draw_Interprete (com);
    delete [] com;
#ifdef WNT
  }
#endif
}
//#endif

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

#ifndef WNT
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
#ifdef WNT
//Standard_EXPORT void Draw_Appli(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lps
Standard_EXPORT void Draw_Appli(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszLine, int nShow,const FDraw_InitAppli Draw_InitAppli)
#else
void Draw_Appli(Standard_Integer argc, char** argv,const FDraw_InitAppli Draw_InitAppli)
#endif
{
  // *****************************************************************
  // analyze arguments
  // *****************************************************************
  Draw_Batch = Standard_False;
  TCollection_AsciiString aRunFile;
  Standard_Integer i;
  Standard_Boolean isInteractiveForced = Standard_False;
#ifndef WNT
  for (i = 0; i < argc; i++) {
    if (strcasecmp(argv[i],"-b") == 0)
      Draw_Batch = Standard_True;
# ifndef __sgi
    else if (strcasecmp(argv[i],"-l") == 0) {
      Draw_LowWindows = Standard_True;
    }
# endif
    else if (strcasecmp(argv[i],"-v") == 0) {
      // force virtual windows
      Draw_VirtualWindows = Standard_True;
    } else if (strcasecmp(argv[i],"-i") == 0) {
      // force interactive
      Draw_VirtualWindows = Standard_False;
      isInteractiveForced = Standard_True;
    } else if (strcasecmp(argv[i],"-f") == 0) { // -f option should be LAST!
      Draw_VirtualWindows = !isInteractiveForced;
      if (++i < argc) {
        aRunFile = TCollection_AsciiString (argv[i]);
      }
      break;
    }
  }
#else
  // On NT command line arguments are in the lpzline and not in argv
  for (char* p = strtok(lpszLine, " \t"); p != NULL; p = strtok(NULL, " \t")) {
    if (strcasecmp(p, "-v") == 0) {
      Draw_VirtualWindows = Standard_True;
    } else if (strcasecmp(p, "-i") == 0) {
      // force interactive
      Draw_VirtualWindows = Standard_False;
      isInteractiveForced = Standard_True;
    } else if (strcasecmp(p, "-f") == 0) { // -f option should be LAST!
      Draw_VirtualWindows = !isInteractiveForced;
      p = strtok(NULL," \t");
      if (p != NULL) {
        aRunFile = TCollection_AsciiString (p);
      }
      break;
    }
  }
#endif
  // *****************************************************************
  // set signals
  // *****************************************************************
  OSD::SetSignal();

  // *****************************************************************
  // init X window and create display
  // *****************************************************************
#ifdef WNT
  HWND hWnd;
#endif

  if (!Draw_Batch)
#ifdef WNT
	Draw_Batch=!Init_Appli(hInst, hPrevInst, nShow, hWnd);
#else
    Draw_Batch=!Init_Appli();
#endif
  else
    cout << "batch mode" << endl;

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

#ifndef WNT
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
#ifdef WNT
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

  // pure batch
  if (!aRunFile.IsEmpty()) {
    // do not map raise the windows, so test programs are discrete
#ifndef WNT
    Draw_LowWindows = Standard_True;
#endif
  // comme on ne peut pas photographier les fenetres trop discretes sur sgi
  // on se met en vedette !! (pmn 20/02/97)
#ifdef __sgi
    Draw_LowWindows = Standard_False;
#endif

    ReadInitFile (aRunFile);
    // provide a clean exit, this is usefull for some analysis tools
#ifndef WNT
    return;
#else
    ExitProcess(0);
#endif
  }

  // *****************************************************************
  // X loop
  // *****************************************************************
  if (XLoop) {
#ifdef WNT
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
#ifdef WNT
  // Destruction de l'application
  Destroy_Appli(hInst);
#endif
}
//#endif

// User functions called before and after each command
void (*Draw_BeforeCommand)() = NULL;
void (*Draw_AfterCommand)(Standard_Integer) = NULL;

Standard_Boolean Draw_Interprete(char* com)
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
		 const TCollection_AsciiString& theResourceFileName) {

  static Draw_MapOfFunctions theMapOfFunctions;
  OSD_Function f;

  if(!theMapOfFunctions.IsBound(theKey)) {

    Handle(Resource_Manager) aPluginResource = new Resource_Manager(theResourceFileName.ToCString());
    if(!aPluginResource->Find(theKey.ToCString())) {
      Standard_SStream aMsg; aMsg << "Could not find the resource:";
      aMsg << theKey.ToCString()<< endl;
      cout << "could not find the resource:"<<theKey.ToCString()<< endl;
      Draw_Failure::Raise(aMsg);
    }

    TCollection_AsciiString aPluginLibrary("");
#ifndef WNT
    aPluginLibrary += "lib";
#endif
    aPluginLibrary +=  aPluginResource->Value(theKey.ToCString());
#ifdef WNT
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
#ifdef DEB
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


//=================================================================================
//
//=================================================================================
void Draw::Load(Draw_Interpretor& theDI, const TCollection_AsciiString& theKey,
		const TCollection_AsciiString& theResourceFileName,
		TCollection_AsciiString& theDefaultsDirectory,
		TCollection_AsciiString& theUserDefaultsDirectory,
		const Standard_Boolean Verbose ) {

  static Draw_MapOfFunctions theMapOfFunctions;
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
#ifndef WNT
    aPluginLibrary += "lib";
#endif
    aPluginLibrary +=  aPluginResource->Value(theKey.ToCString());
#ifdef WNT
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
#ifdef DEB
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
