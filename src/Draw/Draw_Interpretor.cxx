// Created on: 1995-02-23
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <Standard_SStream.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Macro.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_Process.hxx>
#include <OSD_Path.hxx>
#include <OSD.hxx>
#include <OSD_File.hxx>

#include <string.h>
#include <tcl.h>
#ifndef _WIN32
#include <unistd.h>
#endif

// for capturing of cout and cerr (dup(), dup2())
#ifdef _WIN32
#include <io.h>
#endif

#if ! defined(STDOUT_FILENO)
#define STDOUT_FILENO fileno(stdout)
#endif
#if ! defined(STDERR_FILENO)
#define STDERR_FILENO fileno(stderr)
#endif

#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 1)))
#define TCL_USES_UTF8
#endif

// logging helpers
namespace {
  void dumpArgs (Standard_OStream& os, int argc, const char *argv[])
  {
    for (int i=0; i < argc; i++)
      os << argv[i] << " ";
    os << endl;
  }

  void flush_standard_streams ()
  {
    fflush (stderr);
    fflush (stdout);
    cerr << flush;
    cout << flush;
  }

  int capture_start (OSD_File& theTmpFile, int std_fd)
  {
    theTmpFile.BuildTemporary();
    if (theTmpFile.Failed())
    {
      cerr << "Error: cannot create temporary file for capturing console output" << endl;
      return -1;
    }

    // remember current file descriptors of standard stream, and replace it by temporary
    return theTmpFile.Capture(std_fd);
  }

  void capture_end (OSD_File* tmp_file, int std_fd, int save_fd, Standard_OStream &log, Standard_Boolean doEcho)
  {
    if (!tmp_file)
      return;

    // restore normal descriptors of console stream
  #ifdef _WIN32
    _dup2(save_fd, std_fd);
    _close(save_fd);
  #else
    dup2(save_fd, std_fd);
    close(save_fd);
  #endif

    // extract all output and copy it to log and optionally to cout
    const int BUFSIZE = 2048;
    TCollection_AsciiString buf;
    tmp_file->Rewind();
    while (tmp_file->ReadLine (buf, BUFSIZE) > 0)
    {
      log << buf;
      if (doEcho) 
        cout << buf;
    }

    // close temporary file
    tmp_file->Close();

    // remove temporary file if this is not done by the system
    if (tmp_file->Exists())
      tmp_file->Remove();
  }

} // anonymous namespace

// MKV 29.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
static Standard_Integer CommandCmd 
(ClientData theClientData, Tcl_Interp *interp,
 Standard_Integer argc, const char* argv[])
#else
static Standard_Integer CommandCmd 
(ClientData theClientData, Tcl_Interp *interp,
 Standard_Integer argc, char* argv[])
#endif
{
  static Standard_Integer code;
  code = TCL_OK;
  Draw_Interpretor::CallBackData* aCallback = (Draw_Interpretor::CallBackData* )theClientData;
  Draw_Interpretor& di = *(aCallback->myDI);

  // log command execution, except commands manipulating log itself and echo
  Standard_Boolean isLogManipulation = (strcmp (argv[0], "dlog") == 0 || 
                                        strcmp (argv[0], "decho") == 0);
  Standard_Boolean doLog  = (di.GetDoLog() && ! isLogManipulation);
  Standard_Boolean doEcho = (di.GetDoEcho() && ! isLogManipulation);
  if (doLog)
    dumpArgs (di.Log(), argc, argv);
  if (doEcho)
    dumpArgs (cout, argc, argv);

  // flush cerr and cout
  flush_standard_streams();

  // capture cout and cerr to log
  OSD_File aFile_out, aFile_err;
  int fd_err_save = -1;
  int fd_out_save = -1;
  if (doLog)
  {
    fd_out_save = capture_start (aFile_out, STDOUT_FILENO);
    fd_err_save = capture_start (aFile_err, STDERR_FILENO);
  }

  // run command
  try {
    OCC_CATCH_SIGNALS

    // get exception if control-break has been pressed 
    OSD::ControlBreak();

    // OCC680: Transfer UTF-8 directly to OCC commands without locale usage
      
    Standard_Integer fres = aCallback->Invoke ( di, argc, argv /*anArgs.GetArgv()*/ );
    if (fres != 0) 
      code = TCL_ERROR;
  }
  catch (Standard_Failure const& anException) {
    // fail if Draw_ExitOnCatch is set
    // MKV 29.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
    const char*  cc = Tcl_GetVar(interp,
			  "Draw_ExitOnCatch",TCL_GLOBAL_ONLY);
#else
    char* const cc = Tcl_GetVar(interp,
			  "Draw_ExitOnCatch",TCL_GLOBAL_ONLY);
#endif

    cout << "An exception was caught " << anException << endl;

    if (cc && Draw::Atoi(cc)) {
#ifdef _WIN32
      Tcl_Exit(0);
#else      
      Tcl_Eval(interp,"exit");
#endif
    }

    // get the error message
    Standard_SStream ss;
    ss << "** Exception ** " << anException << ends;
    Tcl_SetResult(interp,(char*)(ss.str().c_str()),TCL_VOLATILE);
    code = TCL_ERROR;
  }

  // flush streams
  flush_standard_streams();

  // end capturing cout and cerr 
  if (doLog) 
  {
    capture_end (&aFile_err, STDERR_FILENO, fd_err_save, di.Log(), doEcho);
    capture_end (&aFile_out, STDOUT_FILENO, fd_out_save, di.Log(), doEcho);
  }

  // log command result
  const char* aResultStr = NULL;
  if (doLog)
  {
    aResultStr = Tcl_GetStringResult (interp);
    if (aResultStr != 0 && aResultStr[0] != '\0' )
      di.Log() << Tcl_GetStringResult (interp) << endl;
  }
  if (doEcho)
  {
    if (aResultStr == NULL)
      aResultStr = Tcl_GetStringResult (interp);
    if (aResultStr != 0 && aResultStr[0] != '\0' )
      cout << Tcl_GetStringResult (interp) << endl;
  }

  return code;
}

static void CommandDelete (ClientData theClientData)
{
  Draw_Interpretor::CallBackData* aCallback = (Draw_Interpretor::CallBackData* )theClientData;
  delete aCallback;
}

//=======================================================================
//function : Draw_Interpretor
//purpose  : 
//=======================================================================

Draw_Interpretor::Draw_Interpretor() :
  isAllocated(Standard_False), myDoLog(Standard_False), myDoEcho(Standard_False)
{
// The tcl interpreter is not created immediately as it is kept 
// by a global variable and created and deleted before the main().
  myInterp  = NULL;
}

//=======================================================================
//function : Init
//purpose  : It is necessary to call this function
//=======================================================================

void Draw_Interpretor::Init()
{
  if (isAllocated) 
    Tcl_DeleteInterp(myInterp);
  isAllocated=Standard_True;
  myInterp=Tcl_CreateInterp();
}

//=======================================================================
//function : Draw_Interpretor
//purpose  : 
//=======================================================================

Draw_Interpretor::Draw_Interpretor(const Draw_PInterp& p) :
  isAllocated(Standard_False),
  myInterp(p),
  myDoLog(Standard_False),
  myDoEcho(Standard_False)
{
}

//=======================================================================
//function : add
//purpose  :
//=======================================================================
void Draw_Interpretor::add (const Standard_CString          theCommandName,
                            const Standard_CString          theHelp,
                            const Standard_CString          theFileName,
                            Draw_Interpretor::CallBackData* theCallback,
                            const Standard_CString          theGroup)
{
  if (myInterp == NULL)
  {
    Init();
  }

  Standard_PCharacter aName  = (Standard_PCharacter )theCommandName;
  Standard_PCharacter aHelp  = (Standard_PCharacter )theHelp;
  Standard_PCharacter aGroup = (Standard_PCharacter )theGroup;
  Tcl_CreateCommand (myInterp, aName, CommandCmd, (ClientData )theCallback, CommandDelete);

  // add the help
  Tcl_SetVar2 (myInterp, "Draw_Helps",  aName,  aHelp, TCL_GLOBAL_ONLY);
  Tcl_SetVar2 (myInterp, "Draw_Groups", aGroup, aName,
	             TCL_GLOBAL_ONLY | TCL_APPEND_VALUE | TCL_LIST_ELEMENT);

  // add path to source file (keep not more than two last subdirectories)
  if (theFileName  == NULL
   || *theFileName == '\0')
  {
    return;
  }

  OSD_Path aPath (theFileName);
  Standard_Integer nbTrek = aPath.TrekLength();
  for (Standard_Integer i = 2; i < nbTrek; ++i)
  {
    aPath.RemoveATrek (1);
  }
  aPath.SetDisk ("");
  aPath.SetNode ("");
  TCollection_AsciiString aSrcPath;
  aPath.SystemName (aSrcPath);
  if (aSrcPath.Value(1) == '/')
    aSrcPath.Remove(1);
  Tcl_SetVar2 (myInterp, "Draw_Files", aName, aSrcPath.ToCString(), TCL_GLOBAL_ONLY);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean Draw_Interpretor::Remove(Standard_CString const n)
{
  Standard_PCharacter pN;
  //
  pN=(Standard_PCharacter)n;
 
  Standard_Integer result = Tcl_DeleteCommand(myInterp,pN);
  return result == 0;
}

//=======================================================================
//function : Result
//purpose  : 
//=======================================================================

Standard_CString Draw_Interpretor::Result() const
{
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 5)))
  return Tcl_GetStringResult(myInterp);
#else
  return myInterp->result;
#endif
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void Draw_Interpretor::Reset()
{
  Tcl_ResetResult(myInterp);
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

Draw_Interpretor& Draw_Interpretor::Append(const Standard_CString s)
{
#ifdef TCL_USES_UTF8
  // Convert string to UTF-8 format for Tcl
  Tcl_DString TclString;
  Tcl_ExternalToUtfDString ( NULL, s, -1, &TclString );
  Tcl_AppendResult ( myInterp, Tcl_DStringValue ( &TclString ), (Standard_CString)0 );
  Tcl_DStringFree ( &TclString );
#else
  Tcl_AppendResult(myInterp,s,(Standard_CString)0);
#endif
  return *this;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

Draw_Interpretor& Draw_Interpretor::Append(const TCollection_AsciiString& s)
{
  return Append (s.ToCString());
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

Draw_Interpretor& Draw_Interpretor::Append(const TCollection_ExtendedString& theString)
{
#ifdef TCL_USES_UTF8
  // Convert string to UTF-8 format for Tcl
  char *str = new char[theString.LengthOfCString()+1];
  theString.ToUTF8CString (str);
  Tcl_AppendResult ( myInterp, str, (Standard_CString)0 );
  delete[] str;
#else
  // put as ascii string, replacing non-ascii characters by '?'
  TCollection_AsciiString str (theString, '?');
  Tcl_AppendResult(myInterp,str.ToCString(),(Standard_CString)0);
#endif
  return *this;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

Draw_Interpretor& Draw_Interpretor::Append(const Standard_Integer i)
{
  char c[100];
  Sprintf(c,"%d",i);
  Tcl_AppendResult(myInterp,c,(Standard_CString)0);
  return *this;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

Draw_Interpretor& Draw_Interpretor::Append(const Standard_Real r)
{
  char s[100];
  Sprintf(s,"%.17g",r);
  Tcl_AppendResult(myInterp,s,(Standard_CString)0);
  return *this;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

Draw_Interpretor& Draw_Interpretor::Append(const Standard_SStream& s)
{
  return Append (s.str().c_str());
}

//=======================================================================
//function : AppendElement
//purpose  : 
//=======================================================================

void Draw_Interpretor::AppendElement(const Standard_CString s)
{
#ifdef TCL_USES_UTF8
  // Convert string to UTF-8 format for Tcl
  Tcl_DString TclString;
  Tcl_ExternalToUtfDString ( NULL, s, -1, &TclString );
  Tcl_AppendElement ( myInterp, Tcl_DStringValue ( &TclString ) );
  Tcl_DStringFree ( &TclString );
#else
#ifdef IRIX
  //AppendElement is declared as (Tcl_Interp *interp, char *string)
  //on SGI 32
  Tcl_AppendElement(myInterp,(char*) s);
#else
  Tcl_AppendElement(myInterp, s);
#endif
#endif
}

//=======================================================================
//function : Eval
//purpose  : 
//=======================================================================

Standard_Integer Draw_Interpretor::Eval(const Standard_CString line)
{
  return Tcl_Eval(myInterp,line);
}


//=======================================================================
//function : Eval
//purpose  : 
//=======================================================================

Standard_Integer Draw_Interpretor::RecordAndEval(const Standard_CString line,
						 const Standard_Integer flags)
{
  return Tcl_RecordAndEval(myInterp,line,flags);
}

//=======================================================================
//function : EvalFile
//purpose  : 
//=======================================================================

Standard_Integer Draw_Interpretor::EvalFile(const Standard_CString fname)
{
  return Tcl_EvalFile(myInterp,fname);
}

//=======================================================================
//function : PrintHelp
//purpose  :
//=======================================================================

Standard_Integer Draw_Interpretor::PrintHelp (const Standard_CString theCommandName)
{
  TCollection_AsciiString aCmd     = TCollection_AsciiString ("help ") + theCommandName;
  Standard_PCharacter     aLinePtr = (Standard_PCharacter )aCmd.ToCString();
  return Tcl_Eval (myInterp, aLinePtr);
}

//=======================================================================
//function :Complete
//purpose  : 
//=======================================================================

Standard_Boolean Draw_Interpretor::Complete(const Standard_CString line)
{
  Standard_PCharacter pLine;
  //
  pLine=(Standard_PCharacter)line;
  return Tcl_CommandComplete (pLine) != 0;
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

Draw_Interpretor::~Draw_Interpretor()
{
  // MKV 01.02.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  try {
    OCC_CATCH_SIGNALS
    Tcl_Exit(0);
  }
  catch (Standard_Failure) {
#ifdef OCCT_DEBUG
    cout <<"Tcl_Exit have an exeption" << endl;
#endif
  }
#else
#ifdef _WIN32
  Tcl_Exit(0);
#endif  
#endif
}

//=======================================================================
//function : Interp
//purpose  : 
//=======================================================================

Draw_PInterp Draw_Interpretor::Interp() const
{
  Standard_DomainError_Raise_if (myInterp==NULL , "No call for  Draw_Interpretor::Init()");
  return myInterp;
}

void Draw_Interpretor::Set(const Draw_PInterp& PIntrp)
{
  if (isAllocated)
    Tcl_DeleteInterp(myInterp);
  isAllocated = Standard_False;
  myInterp = PIntrp;
}

//=======================================================================
//function : Logging
//purpose  : 
//=======================================================================

void Draw_Interpretor::SetDoLog (Standard_Boolean doLog)
{
  myDoLog = doLog;
}

void Draw_Interpretor::SetDoEcho (Standard_Boolean doEcho)
{
  myDoEcho = doEcho;
}

Standard_Boolean Draw_Interpretor::GetDoLog () const
{
  return myDoLog;
}

Standard_Boolean Draw_Interpretor::GetDoEcho () const
{
  return myDoEcho;
}

Standard_SStream& Draw_Interpretor::Log ()
{
  return myLog;
}
