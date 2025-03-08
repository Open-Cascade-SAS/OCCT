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
#include <Draw_Failure.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Draw_Window.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <OSD.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_Process.hxx>
#include <OSD_SharedLibrary.hxx>
#include <OSD_Timer.hxx>
#include <Plugin_MapOfFunctions.hxx>
#include <Resource_Manager.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Version.hxx>
#include <TCollection_AsciiString.hxx>

#include <tcl.h>

#include <Standard_WarningDisableFunctionCast.hxx>

extern Standard_Boolean Draw_ParseFailed;

Standard_EXPORT Draw_Viewer      dout;
Standard_EXPORT Draw_Interpretor theCommands;
Standard_EXPORT Standard_Boolean Draw_Batch          = Standard_False;
Standard_EXPORT Standard_Boolean Draw_Spying         = Standard_False;
Standard_EXPORT Standard_Boolean Draw_Chrono         = Standard_False;
Standard_EXPORT Standard_Boolean Draw_VirtualWindows = Standard_False;
Standard_EXPORT Standard_Boolean ErrorMessages       = Standard_True;

static const char* ColorNames[MAXCOLOR] = {"White",
                                           "Red",
                                           "Green",
                                           "Blue",
                                           "Cyan",
                                           "Gold",
                                           "Magenta",
                                           "Maroon",
                                           "Orange",
                                           "Pink",
                                           "Salmon",
                                           "Violet",
                                           "Yellow",
                                           "Khaki",
                                           "Coral"};

std::filebuf Draw_Spyfile;

static std::ostream spystream(&Draw_Spyfile);

static Handle(Draw_ProgressIndicator) global_Progress = NULL;

Standard_EXPORT Standard_Boolean Draw_Interprete(const char* command);

// true if complete command

// *******************************************************************
// read an init file
// *******************************************************************

static void interpreteTclCommand(const TCollection_AsciiString& theCmd)
{
#ifdef _WIN32
  if (!Draw_Batch)
  {
    try
    {
      while (console_semaphore == HAS_CONSOLE_COMMAND)
      {
        Sleep(10);
      }
      {
        TCollection_ExtendedString aCmdWide(theCmd);
        wcscpy_s(console_command, aCmdWide.ToWideString());
      }
      console_semaphore = HAS_CONSOLE_COMMAND;
      while (console_semaphore == HAS_CONSOLE_COMMAND)
      {
        Sleep(10);
      }
    }
    catch (...)
    {
      std::cout << "Error while reading a script file.\n";
      ExitProcess(0);
    }
  }
  else
#endif
  {
    Draw_Interprete(theCmd.ToCString());
  }
}

static void ReadInitFile(const TCollection_AsciiString& theFileName)
{
  TCollection_AsciiString aCmd = theFileName;
#ifdef _WIN32
  aCmd.ChangeAll('\\', '/');
#endif
  aCmd = TCollection_AsciiString("source -encoding utf-8 \"") + aCmd + "\"";
  interpreteTclCommand(aCmd);
}

//! Define environment variable available from Tcl and OCCT.
static void setOcctTclEnv(const TCollection_AsciiString& theName, TCollection_AsciiString& thePath)
{
  if (thePath.IsEmpty())
  {
    return;
  }

  thePath.ChangeAll('\\', '/');
  OSD_Environment aRedPathEnv(theName);
  aRedPathEnv.SetValue(thePath);
  aRedPathEnv.Build();

  const TCollection_AsciiString aPutEnv = theName + "=" + thePath;
  Tcl_PutEnv(aPutEnv.ToCString());
}

//! Look for resource within standard installation layouts relative to executable location.
//!
//! Bin (INSTALL_DIR_BIN):
//!  - Windows: <prefix>/win64/vc10/bin(d)
//!  - Unix:    <prefix>/bin
//! Resources (INSTALL_DIR_RESOURCE):
//!  - Windows: <prefix>/src
//!  - Unix:    <prefix>/share/opencascade-7.0.0/resources
//! Samples (INSTALL_DIR_SAMPLES):
//!  - Windows: <prefix>/samples
//!  - Unix:    <prefix>/share/opencascade-7.0.0/samples
//! Tests (INSTALL_DIR_TESTS):
//!  - Windows: <prefix>/tests
//!  - Unix:    <prefix>/share/opencascade-7.0.0/tests
//!
//! @param theCasRoot  [out] found CASROOT location (e.g. installation folder)
//! @param theResRoot  [out] found resources root location
//! @param theResName   [in] resource to find ("resources", "samples", etc.)
//! @param theProbeFile [in] file to probe within resources location (e.g.
//! "DrawResources/DrawDefault" within "resources")
static bool searchResources(TCollection_AsciiString&       theCasRoot,
                            TCollection_AsciiString&       theResRoot,
                            const TCollection_AsciiString& theResName,
                            const TCollection_AsciiString& theProbeFile)
{
  const TCollection_AsciiString aResLayouts[] = {
    TCollection_AsciiString("/share/opencascade-" OCC_VERSION_STRING_EXT "/") + theResName,
    TCollection_AsciiString("/share/opencascade-" OCC_VERSION_COMPLETE "/") + theResName,
    TCollection_AsciiString("/share/opencascade-" OCC_VERSION_STRING "/") + theResName,
    TCollection_AsciiString("/share/opencascade/") + theResName,
    TCollection_AsciiString("/share/occt-" OCC_VERSION_STRING_EXT "/") + theResName,
    TCollection_AsciiString("/share/occt-" OCC_VERSION_COMPLETE "/") + theResName,
    TCollection_AsciiString("/share/occt-" OCC_VERSION_STRING "/") + theResName,
    TCollection_AsciiString("/share/occt/") + theResName,
    TCollection_AsciiString("/") + theResName,
    TCollection_AsciiString("/share/opencascade"),
    TCollection_AsciiString("/share/occt"),
    TCollection_AsciiString("/share"),
    TCollection_AsciiString("/src"),
    TCollection_AsciiString("")};

  const TCollection_AsciiString anExeDir(OSD_Process::ExecutableFolder());
  for (Standard_Integer aLayIter = 0;; ++aLayIter)
  {
    const TCollection_AsciiString& aResLayout = aResLayouts[aLayIter];
    const TCollection_AsciiString  aProbeFile = aResLayout + "/" + theProbeFile;
    if (OSD_File(anExeDir + aProbeFile).Exists())
    {
      theCasRoot = anExeDir;
      theResRoot = theCasRoot + aResLayout;
      return true;
    }
    // <prefix>/bin(d)
    else if (OSD_File(anExeDir + "../" + aProbeFile).Exists())
    {
      theCasRoot = anExeDir + "..";
      theResRoot = theCasRoot + aResLayout;
      return true;
    }
    // <prefix>/gcc/bin(d)
    else if (OSD_File(anExeDir + "../../" + aProbeFile).Exists())
    {
      theCasRoot = anExeDir + "../..";
      theResRoot = theCasRoot + aResLayout;
      return true;
    }
    // <prefix>/win64/vc10/bin(d)
    else if (OSD_File(anExeDir + "../../../" + aProbeFile).Exists())
    {
      theCasRoot = anExeDir + "../../..";
      theResRoot = theCasRoot + aResLayout;
      return true;
    }

    if (aResLayout.IsEmpty())
    {
      return false;
    }
  }
}

//=================================================================================================

Draw_Interpretor& Draw::GetInterpretor()
{
  return theCommands;
}

//=======================================================================
// function :
// purpose  : Set/Get Progress Indicator
//=======================================================================
void Draw::SetProgressBar(const Handle(Draw_ProgressIndicator)& theProgress)
{
  global_Progress = theProgress;
}

Handle(Draw_ProgressIndicator) Draw::GetProgressBar()
{
  return global_Progress;
}

#ifndef _WIN32
/*--------------------------------------------------------*\
|  exitProc: finalization handler for Tcl/Tk thread. Forces parent process to die
\*--------------------------------------------------------*/
void exitProc(ClientData /*dc*/)
{
  if (!Draw_Batch)
  {
    for (Standard_Integer id = 0; id < MAXVIEW; id++)
      dout.DeleteView(id);
  }
}
#endif

// *******************************************************************
// main
// *******************************************************************
#ifdef _WIN32
Standard_EXPORT void Draw_Appli(HINSTANCE             hInst,
                                HINSTANCE             hPrevInst,
                                int                   nShow,
                                int                   argc,
                                wchar_t**             argv,
                                const FDraw_InitAppli Draw_InitAppli)
#else
void Draw_Appli(int argc, char** argv, const FDraw_InitAppli Draw_InitAppli)
#endif
{

// prepend extra DLL search path to override system libraries like opengl32.dll
#ifdef _WIN32
  OSD_Environment                  aUserDllEnv("CSF_UserDllPath");
  const TCollection_ExtendedString aUserDllPath(aUserDllEnv.Value());
  if (!aUserDllPath.IsEmpty())
  {
    // This function available since Win XP SP1 #if (_WIN32_WINNT >= 0x0502).
    // We retrieve dynamically here (kernel32 should be always preloaded).
    typedef BOOL(WINAPI * SetDllDirectoryW_t)(const wchar_t* thePathName);
    HMODULE            aKern32Module = GetModuleHandleW(L"kernel32");
    SetDllDirectoryW_t aFunc =
      (aKern32Module != NULL)
        ? (SetDllDirectoryW_t)GetProcAddress(aKern32Module, "SetDllDirectoryW")
        : NULL;
    if (aFunc != NULL)
    {
      aFunc(aUserDllPath.ToWideString());
    }
    else
    {
      // std::cerr << "SetDllDirectoryW() is not available on this system!\n";
    }
    if (aKern32Module != NULL)
    {
      FreeLibrary(aKern32Module);
    }
  }
#endif

  // *****************************************************************
  // analyze arguments
  // *****************************************************************
  Draw_Batch = Standard_False;
  TCollection_AsciiString aRunFile, aCommand;
  Standard_Boolean        isInteractiveForced = Standard_False;

  // parse command line
  for (int anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArg(argv[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-h" || anArg == "--help")
    {
      std::cout << "Open CASCADE " << OCC_VERSION_STRING_EXT << " DRAW Test Harness\n\n";
      std::cout << "Options:\n";
      std::cout << "  -b: batch mode (no GUI, no viewers)\n";
      std::cout << "  -v: no GUI, use virtual (off-screen) windows for viewers\n";
      std::cout << "  -i: interactive mode\n";
      std::cout << "  -f file: execute script from file\n";
      std::cout << "  -c command args...: execute command (with optional arguments)\n\n";
      std::cout << "Options -b, -v, and -i are mutually exclusive.\n";
      std::cout << "If -c or -f are given, -v is default; otherwise default is -i.\n";
      std::cout << "Options -c and -f are alternatives and should be at the end \n";
      std::cout << "of the command line.\n";
      std::cout << "Option -c can accept set of commands separated by ';'.\n";
      return;
    }
    else if (anArg == "-b")
    {
      Draw_Batch = Standard_True;
    }
    else if (anArg == "-v")
    {
      // force virtual windows
      Draw_VirtualWindows = Standard_True;
    }
    else if (anArg == "-i")
    {
      // force interactive
      Draw_VirtualWindows = Standard_False;
      isInteractiveForced = Standard_True;
    }
    else if (anArg == "-f") // -f option should be LAST!
    {
      Draw_VirtualWindows = !isInteractiveForced;
      if (++anArgIter < argc)
      {
        aRunFile = TCollection_AsciiString(argv[anArgIter]);
      }
      break;
    }
    else if (anArg == "-c") // -c option should be LAST!
    {
      Draw_VirtualWindows = !isInteractiveForced;
      if (++anArgIter < argc)
      {
        aCommand = TCollection_AsciiString(argv[anArgIter]);
      }
      while (++anArgIter < argc)
      {
        aCommand.AssignCat(" ");
        aCommand.AssignCat(argv[anArgIter]);
      }
      break;
    }
    else
    {
      std::cout << "Error: unsupported option " << TCollection_AsciiString(argv[anArgIter]) << "\n";
    }
  }

  // *****************************************************************
  // set signals
  // *****************************************************************
  OSD::SetSignal(Standard_False);
  // OSD::SetSignalStackTraceLength (10);

#ifdef _WIN32
  // in interactive mode, force Windows to report dll loading problems interactively
  if (!Draw_VirtualWindows && !Draw_Batch)
    ::SetErrorMode(0);
#endif

    // *****************************************************************
    // init X window and create display
    // *****************************************************************
#ifdef _WIN32
  HWND hWnd = NULL;
#endif

  if (!Draw_Batch)
#ifdef _WIN32
    Draw_Batch = !Init_Appli(hInst, hPrevInst, nShow, hWnd);
#else
    Draw_Batch = !Init_Appli();
#endif
  else
  {
    std::cout << "DRAW is running in batch mode" << std::endl;
    theCommands.Init();
    Tcl_Init(theCommands.Interp());
  }

  if (!Draw_Batch)
  {
    // Default colors
    for (int i = 0; i < MAXCOLOR; ++i)
    {
      if (!dout.DefineColor(i, ColorNames[i]))
      {
        std::cout << "Could not allocate default color " << ColorNames[i] << std::endl;
      }
    }
  }

  // *****************************************************************
  // set maximum precision for cout
  // *****************************************************************
  std::cout.precision(15);

  // *****************************************************************
  // standard commands
  // *****************************************************************
  Draw::BasicCommands(theCommands);
  Draw::MessageCommands(theCommands);
  Draw::VariableCommands(theCommands);
  Draw::UnitCommands(theCommands);
  if (!Draw_Batch)
    Draw::GraphicCommands(theCommands);

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
  const TCollection_AsciiString aDrawDef(OSD_Environment("DRAWDEFAULT").Value());
  if (!aDrawDef.IsEmpty())
  {
    ReadInitFile(aDrawDef);
  }
  else
  {
    TCollection_AsciiString aDrawHome;
    TCollection_AsciiString aCasRoot(OSD_Environment("CASROOT").Value());
    if (!aCasRoot.IsEmpty())
    {
      aDrawHome = aCasRoot + "/src/DrawResources";
    }
    else
    {
      // search for relative locations within standard development environment
      TCollection_AsciiString aResPath;
      if (searchResources(aCasRoot, aResPath, "resources", "DrawResources/DrawDefault"))
      {
        aDrawHome = aResPath + "/DrawResources";
        setOcctTclEnv("CASROOT", aCasRoot);
        setOcctTclEnv("DRAWHOME", aDrawHome);
        setOcctTclEnv("CSF_OCCTResourcePath", aResPath);
      }

      TCollection_AsciiString aSamplesPath;
      if (OSD_Environment("CSF_OCCTSamplesPath").Value().IsEmpty()
          && searchResources(aCasRoot, aSamplesPath, "samples", "tcl/Readme.txt"))
      {
        setOcctTclEnv("CSF_OCCTSamplesPath", aSamplesPath);
      }

      TCollection_AsciiString aTestsPath;
      if (OSD_Environment("CSF_TestScriptsPath").Value().IsEmpty()
          && searchResources(aCasRoot, aTestsPath, "tests", "parse.rules"))
      {
        setOcctTclEnv("CSF_TestScriptsPath", aTestsPath);
      }
    }

    if (!aDrawHome.IsEmpty())
    {
      const TCollection_AsciiString aDefStr = aDrawHome + "/DrawDefault";
      ReadInitFile(aDefStr);
    }
    else
    {
#ifdef _WIN32
      ReadInitFile("ddefault");
#else
      std::cout << " the CASROOT variable is mandatory to Run OpenCascade " << std::endl;
      std::cout << "No default file" << std::endl;
#endif
    }
  }

  // read commands from file
  if (!aRunFile.IsEmpty())
  {
    if (!isInteractiveForced)
    {
      // disable console messages colorization to avoid spoiling log with color codes
      theCommands.SetToColorize(Standard_False);
    }
    ReadInitFile(aRunFile);
    // provide a clean exit, this is useful for some analysis tools
    if (!isInteractiveForced)
#ifndef _WIN32
      return;
#else
      ExitProcess(0);
#endif
  }

  // execute command from command line
  if (!aCommand.IsEmpty())
  {
#ifdef _WIN32
    if (!Draw_Batch)
    {
      // on Windows except batch mode, commands are executed in separate thread
      while (console_semaphore == HAS_CONSOLE_COMMAND)
        Sleep(10);
      TCollection_ExtendedString aCmdWide(aCommand);
      wcscpy_s(console_command, aCmdWide.ToWideString());
      console_semaphore = HAS_CONSOLE_COMMAND;
      while (console_semaphore == HAS_CONSOLE_COMMAND)
        Sleep(10);
    }
    else
#endif
      Draw_Interprete(aCommand.ToCString()); // Linux and Windows batch mode
    // provide a clean exit, this is useful for some analysis tools
    if (!isInteractiveForced)
#ifndef _WIN32
      return;
#else
      ExitProcess(0);
#endif
  }

  // *****************************************************************
  // X loop
  // *****************************************************************
  if (!Draw_Batch)
  {
#ifdef _WIN32
    Run_Appli(hWnd);
#else
    Run_Appli(Draw_Interprete);
#endif
  }
  else
  {
    const int MAXCMD = 2048;
    char      cmd[MAXCMD];
    for (int ncmd = 1;; ++ncmd)
    {
      std::cout << "Draw[" << ncmd << "]> ";
      if (std::cin.getline(cmd, MAXCMD).fail())
      {
        break;
      }
      Draw_Interprete(cmd);
    }
  }
#ifdef _WIN32
  // Destruction de l'application
  Destroy_Appli(hInst);
#endif
}
// #endif

// User functions called before and after each command
void (*Draw_BeforeCommand)()                = NULL;
void (*Draw_AfterCommand)(Standard_Integer) = NULL;

Standard_Boolean Draw_Interprete(const char* com)
{

  static Standard_Boolean first = Standard_True;
  static Tcl_DString      command;

  if (first)
  {
    first = Standard_False;
    Tcl_DStringInit(&command);
  }

#ifdef _WIN32
  // string is already converted into UTF-8
  Tcl_DStringAppend(&command, com, -1);
#elif ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 1)))
  // OCC63: Since Tcl 8.1 it uses UTF-8 encoding for internal representation of strings
  Tcl_ExternalToUtfDString(NULL, com, -1, &command);
#else
  Tcl_DStringAppend(&command, com, -1);
#endif

  if (!theCommands.Complete(Tcl_DStringValue(&command)))
    return Standard_False;

  // *******************************************************************
  // Command interpreter
  // *******************************************************************

  //  Standard_Integer i = 0;
  //  Standard_Integer j = 0;

  Standard_Boolean wasspying = Draw_Spying;

  OSD_Timer        tictac;
  Standard_Boolean hadchrono = Draw_Chrono;
  if (hadchrono)
    tictac.Start();

  if (Draw_BeforeCommand)
    (*Draw_BeforeCommand)();

  Standard_Integer c;

  c = theCommands.RecordAndEval(Tcl_DStringValue(&command));

  if (Draw_AfterCommand)
    (*Draw_AfterCommand)(c);

  if (wasspying && Draw_Spying)
  {
    if (c > 0)
      spystream << "# ";
    spystream << Tcl_DStringValue(&command) << "\n";
  }

  dout.Flush();

  if (*theCommands.Result())
  {
    if (c > 0 && theCommands.ToColorize())
    {
      Message_PrinterOStream::SetConsoleTextColor(&std::cout, Message_ConsoleColor_Red, true);
    }
    std::cout << theCommands.Result() << std::endl;
    if (c > 0 && theCommands.ToColorize())
    {
      Message_PrinterOStream::SetConsoleTextColor(&std::cout, Message_ConsoleColor_Default, false);
    }
  }

  if (Draw_Chrono && hadchrono)
  {
    tictac.Stop();
    tictac.Show();
  }

  Tcl_DStringFree(&command);

  return Standard_True;
}

//
// for TCl
//

Standard_Integer Tcl_AppInit(Tcl_Interp*)
{
  return 0;
}

//
// for debug call
//

Standard_Integer Draw_Call(char* c)
{
  Standard_Integer r = theCommands.Eval(c);
  std::cout << theCommands.Result() << std::endl;
  return r;
}

//=================================================================================
//
//=================================================================================
void Draw::Load(Draw_Interpretor&              theDI,
                const TCollection_AsciiString& theKey,
                const TCollection_AsciiString& theResourceFileName,
                const TCollection_AsciiString& theDefaultsDirectory,
                const TCollection_AsciiString& theUserDefaultsDirectory,
                const Standard_Boolean         theIsVerbose)
{
  static Plugin_MapOfFunctions theMapOfFunctions;
  OSD_Function                 aFunc = NULL;
  if (!theMapOfFunctions.Find(theKey, aFunc))
  {
    TCollection_AsciiString  aPluginLibrary;
    Handle(Resource_Manager) aPluginResource = new Resource_Manager(theResourceFileName,
                                                                    theDefaultsDirectory,
                                                                    theUserDefaultsDirectory,
                                                                    theIsVerbose);
    if (!aPluginResource->Find(theKey, aPluginLibrary))
    {
      Message::SendFail() << "could not find the resource:" << theKey;
      Standard_SStream aMsg;
      aMsg << "Could not find the resource:" << theKey << std::endl;
      throw Draw_Failure(aMsg.str().c_str());
    }

#if !defined(_WIN32) || defined(__MINGW32__)
    aPluginLibrary = TCollection_AsciiString("lib") + aPluginLibrary;
#endif
#ifdef _WIN32
    aPluginLibrary += ".dll";
#elif __APPLE__
    aPluginLibrary += ".dylib";
#elif defined(HPUX) || defined(_hpux)
    aPluginLibrary += ".sl";
#else
    aPluginLibrary += ".so";
#endif
    OSD_SharedLibrary aSharedLibrary(aPluginLibrary.ToCString());
    if (!aSharedLibrary.DlOpen(OSD_RTLD_LAZY))
    {
      const TCollection_AsciiString anError(aSharedLibrary.DlError());
      Standard_SStream              aMsg;
      aMsg << "Could not open: " << aPluginLibrary << "; reason: " << anError;
#ifdef OCCT_DEBUG
      std::cout << "could not open: " << aPluginLibrary << " ; reason: " << anError << std::endl;
#endif
      throw Draw_Failure(aMsg.str().c_str());
    }

    aFunc = aSharedLibrary.DlSymb("PLUGINFACTORY");
    if (aFunc == NULL)
    {
      const TCollection_AsciiString anError(aSharedLibrary.DlError());
      Standard_SStream              aMsg;
      aMsg << "Could not find the factory in: " << aPluginLibrary << anError;
      throw Draw_Failure(aMsg.str().c_str());
    }
    theMapOfFunctions.Bind(theKey, aFunc);
  }

  void (*fp)(Draw_Interpretor&) = NULL;
  fp                            = (void (*)(Draw_Interpretor&))aFunc;
  (*fp)(theDI);
}

namespace
{
const Standard_Integer   THE_MAX_INTEGER_COLOR_COMPONENT = 255;
const Standard_ShortReal THE_MAX_REAL_COLOR_COMPONENT    = 1.0f;

//! Parses string and get an integer color component (only values within range 0 .. 255 are allowed)
//! @param theColorComponentString the string representing the color component
//! @param theIntegerColorComponent an integer color component that is a result of parsing
//! @return true if parsing was successful, or false otherwise
static bool parseNumericalColorComponent(const Standard_CString theColorComponentString,
                                         Standard_Integer&      theIntegerColorComponent)
{
  Standard_Integer anIntegerColorComponent;
  if (!Draw::ParseInteger(theColorComponentString, anIntegerColorComponent))
  {
    return false;
  }
  if ((anIntegerColorComponent < 0) || (anIntegerColorComponent > THE_MAX_INTEGER_COLOR_COMPONENT))
  {
    return false;
  }
  theIntegerColorComponent = anIntegerColorComponent;
  return true;
}

//! Parses the string and gets a real color component from it (only values within range 0.0 .. 1.0
//! are allowed)
//! @param theColorComponentString the string representing the color component
//! @param theRealColorComponent a real color component that is a result of parsing
//! @return true if parsing was successful, or false otherwise
static bool parseNumericalColorComponent(const Standard_CString theColorComponentString,
                                         Standard_ShortReal&    theRealColorComponent)
{
  Standard_Real aRealColorComponent;
  if (!Draw::ParseReal(theColorComponentString, aRealColorComponent))
  {
    return false;
  }
  const Standard_ShortReal aShortRealColorComponent =
    static_cast<Standard_ShortReal>(aRealColorComponent);
  if ((aShortRealColorComponent < 0.0f)
      || (aShortRealColorComponent > THE_MAX_REAL_COLOR_COMPONENT))
  {
    return false;
  }
  theRealColorComponent = aShortRealColorComponent;
  return true;
}

//! Parses the string and gets a real color component from it (integer values 2 .. 255 are scaled to
//! the 0.0 .. 1.0 range, values 0 and 1 are leaved as they are)
//! @param theColorComponentString the string representing the color component
//! @param theColorComponent a color component that is a result of parsing
//! @return true if parsing was successful, or false otherwise
static bool parseColorComponent(const Standard_CString theColorComponentString,
                                Standard_ShortReal&    theColorComponent)
{
  Standard_Integer anIntegerColorComponent;
  if (parseNumericalColorComponent(theColorComponentString, anIntegerColorComponent))
  {
    if (anIntegerColorComponent == 1)
    {
      theColorComponent = THE_MAX_REAL_COLOR_COMPONENT;
    }
    else
    {
      theColorComponent = anIntegerColorComponent * 1.0f / THE_MAX_INTEGER_COLOR_COMPONENT;
    }
    return true;
  }
  return parseNumericalColorComponent(theColorComponentString, theColorComponent);
}

//! Parses the array of strings and gets an integer color (only values within range 0 .. 255 are
//! allowed and at least one of components must be greater than 1)
//! @tparam TheNumber the type of resulting color vector elements
//! @param theNumberOfColorComponents the number of color components
//! @param theColorComponentStrings the array of strings representing color components
//! @param theNumericalColor a 4-component vector that is a result of parsing
//! @return true if parsing was successful, or false otherwise
template <typename TheNumber>
static bool parseNumericalColor(Standard_Integer&            theNumberOfColorComponents,
                                const char* const* const     theColorComponentStrings,
                                NCollection_Vec4<TheNumber>& theNumericalColor)
{
  for (Standard_Integer aColorComponentIndex = 0; aColorComponentIndex < theNumberOfColorComponents;
       ++aColorComponentIndex)
  {
    const char* const aColorComponentString = theColorComponentStrings[aColorComponentIndex];
    TheNumber         aNumericalColorComponent;
    if (parseNumericalColorComponent(aColorComponentString, aNumericalColorComponent))
    {
      theNumericalColor[aColorComponentIndex] = aNumericalColorComponent;
    }
    else
    {
      if (aColorComponentIndex == 3)
      {
        theNumberOfColorComponents = 3;
      }
      else
      {
        return false;
      }
    }
  }
  return true;
}

//! Parses an array of strings and get an integer color (only values within range 0 .. 255 are
//! allowed and at least one of components must be greater than 1)
//! @param theNumberOfColorComponents the number of color components
//! @param theColorComponentStrings the array of strings representing color components
//! @param theColor a color that is a result of parsing
//! @return true if parsing was successful, or false otherwise
static bool parseIntegerColor(Standard_Integer&        theNumberOfColorComponents,
                              const char* const* const theColorComponentStrings,
                              Quantity_ColorRGBA&      theColor)
{
  const Standard_Integer THE_COLOR_COMPONENT_NOT_PARSED = -1;
  NCollection_Vec4<int>  anIntegerColor(THE_COLOR_COMPONENT_NOT_PARSED);
  if (!parseNumericalColor(theNumberOfColorComponents, theColorComponentStrings, anIntegerColor)
      || anIntegerColor.maxComp() <= 1)
  {
    return false;
  }
  if (anIntegerColor.a() == THE_COLOR_COMPONENT_NOT_PARSED)
  {
    anIntegerColor.a() = THE_MAX_INTEGER_COLOR_COMPONENT;
  }

  const NCollection_Vec4<float> aRealColor =
    NCollection_Vec4<float>(anIntegerColor) / static_cast<float>(THE_MAX_INTEGER_COLOR_COMPONENT);
  theColor = Quantity_ColorRGBA(Quantity_ColorRGBA::Convert_sRGB_To_LinearRGB(aRealColor));
  return true;
}

//! Parses an array of strings and get a real color (only values within range 0.0 .. 1.0 are
//! allowed)
//! @param theNumberOfColorComponents the number of color components
//! @param theColorComponentStrings the array of strings representing color components
//! @param theColor a color that is a result of parsing
//! @return true if parsing was successful, or false otherwise
static bool parseRealColor(Standard_Integer&        theNumberOfColorComponents,
                           const char* const* const theColorComponentStrings,
                           Quantity_ColorRGBA&      theColor)
{
  NCollection_Vec4<float> aRealColor(THE_MAX_REAL_COLOR_COMPONENT);
  if (!parseNumericalColor(theNumberOfColorComponents, theColorComponentStrings, aRealColor))
  {
    return false;
  }
  theColor = Quantity_ColorRGBA(aRealColor);
  return true;
}
} // namespace

//=================================================================================================

Standard_Integer Draw::parseColor(const Standard_Integer   theArgNb,
                                  const char* const* const theArgVec,
                                  Quantity_ColorRGBA&      theColor,
                                  const bool               theToParseAlpha)
{
  if ((theArgNb >= 1) && Quantity_ColorRGBA::ColorFromHex(theArgVec[0], theColor, !theToParseAlpha))
  {
    return 1;
  }
  if (theArgNb >= 1 && Quantity_ColorRGBA::ColorFromName(theArgVec[0], theColor))
  {
    if (theArgNb >= 2 && theToParseAlpha)
    {
      const Standard_CString anAlphaStr = theArgVec[1];
      Standard_ShortReal     anAlphaComponent;
      if (parseColorComponent(anAlphaStr, anAlphaComponent))
      {
        theColor.SetAlpha(anAlphaComponent);
        return 2;
      }
    }
    return 1;
  }
  if (theArgNb >= 3)
  {
    const Standard_Integer aNumberOfColorComponentsToParse = Min(theArgNb, theToParseAlpha ? 4 : 3);
    Standard_Integer       aNumberOfColorComponentsParsed  = aNumberOfColorComponentsToParse;
    if (parseIntegerColor(aNumberOfColorComponentsParsed, theArgVec, theColor))
    {
      return aNumberOfColorComponentsParsed;
    }
    aNumberOfColorComponentsParsed = aNumberOfColorComponentsToParse;
    if (parseRealColor(aNumberOfColorComponentsParsed, theArgVec, theColor))
    {
      return aNumberOfColorComponentsParsed;
    }
    return 0;
  }
  return 0;
}

//=================================================================================================

Standard_Boolean Draw::ParseOnOff(Standard_CString theArg, Standard_Boolean& theIsOn)
{
  TCollection_AsciiString aFlag(theArg);
  aFlag.LowerCase();
  if (aFlag == "on" || aFlag == "1")
  {
    theIsOn = Standard_True;
    return Standard_True;
  }
  else if (aFlag == "off" || aFlag == "0")
  {
    theIsOn = Standard_False;
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean Draw::ParseOnOffIterator(Standard_Integer  theArgsNb,
                                          const char**      theArgVec,
                                          Standard_Integer& theArgIter)
{
  Standard_Boolean isOn = Standard_True;
  if (theArgIter + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[theArgIter + 1], isOn))
  {
    ++theArgIter;
  }
  return isOn;
}

//=================================================================================================

Standard_Boolean Draw::ParseOnOffNoIterator(Standard_Integer  theArgsNb,
                                            const char**      theArgVec,
                                            Standard_Integer& theArgIter)
{
  Standard_Boolean toReverse = strncasecmp(theArgVec[theArgIter], "no", 2) == 0
                               || strncasecmp(theArgVec[theArgIter], "-no", 3) == 0;
  Standard_Boolean isOn = Draw::ParseOnOffIterator(theArgsNb, theArgVec, theArgIter);
  return toReverse ? !isOn : isOn;
}
