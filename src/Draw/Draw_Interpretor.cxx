// Created on: 1995-02-23
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Draw_Interpretor.ixx>
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

#include <string.h>
#include <tcl.h>

// for capturing of cout and cerr (dup(), dup2())
#ifdef _MSC_VER
#include <io.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
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

//
// Auxiliary tool to convert strings in command arguments from UTF-8 
// (Tcl internal encoding since Tcl 8.1) to system local encoding, 
// normally extended Ascii as expected by OCC commands
//
class TclUTFToLocalStringSentry {
 public:

#ifdef TCL_USES_UTF8
  TclUTFToLocalStringSentry (int argc, const char **argv) :
    nb(0),
    TclArgv(new Tcl_DString[argc]),
    Argv(new char*[argc])
  {
    for (; nb < argc; nb++ ) {
      Tcl_UtfToExternalDString ( NULL, argv[nb], -1, &TclArgv[nb] );
      Argv[nb] = Tcl_DStringValue ( &TclArgv[nb] );
    }
  }
  
  ~TclUTFToLocalStringSentry () 
  {
    delete[] Argv;
    while ( nb-- >0 ) Tcl_DStringFree ( &TclArgv[nb] );
    delete[] TclArgv;
  }
#else
  TclUTFToLocalStringSentry (int, const char **argv) : 
	   nb(0),
       TclArgv(NULL),
	   Argv((char**)argv)
  {}
#endif

  const char **GetArgv () const { return (const char **)Argv; }
  
 private:
  int nb;
  Tcl_DString *TclArgv;
  char **Argv;
};


//
// Call backs for TCL
//

struct CData {
  CData(Draw_CommandFunction ff, Draw_Interpretor* ii) : f(ff), i(ii) {}
  Draw_CommandFunction f;
  Draw_Interpretor*    i;
};

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

  FILE* capture_start (int std_fd, int *save_fd, char*& tmp_name)
  {
    *save_fd = 0;

    // open temporary files
  #if defined(_WIN32)
    // use _tempnam() to decrease chances of failure (tmpfile() creates 
    // file in root folder and will fail if it is write protected), see #24132
    static const char* tmpdir = getenv("TEMP");
    static char prefix[256] = ""; // prefix for temporary files, initialize once per process using pid
    if (prefix[0] == '\0')
      sprintf (prefix, "drawtmp%d_", (int)OSD_Process().ProcessId());
    tmp_name = _tempnam (tmpdir, prefix);
    FILE* aTmpFile = (tmp_name != NULL ? fopen (tmp_name, "w+b") : tmpfile());
  #else
    tmp_name = NULL;
    FILE* aTmpFile = tmpfile();
  #endif
    int fd_tmp = (aTmpFile != NULL ? fileno (aTmpFile) : -1);
    if (fd_tmp < 0)
    {
      cerr << "Error: cannot create temporary file for capturing console output" << endl;
      fclose (aTmpFile);
      return NULL;
    }

    // remember current file descriptors of standard stream, and replace it by temporary
    (*save_fd) = dup(std_fd);
    dup2(fd_tmp, std_fd);
    return aTmpFile;
  }

  void capture_end (FILE* tmp_file, int std_fd, int save_fd, char* tmp_name, Standard_OStream &log, Standard_Boolean doEcho)
  {
    if (! tmp_file)
      return;

    // restore normal descriptors of console stream
    dup2 (save_fd, std_fd);
    close(save_fd);

    // extract all output and copy it to log and optionally to cout
    const int BUFSIZE = 2048;
    char buf[BUFSIZE];
    rewind(tmp_file);
    while (fgets (buf, BUFSIZE, tmp_file) != NULL)
    {
      log << buf;
      if (doEcho) 
        cout << buf;
    }

    // close temporary file
    fclose (tmp_file);

    // remove temporary file if this is not done by the system
    if (tmp_name)
      remove (tmp_name);
  }
};

// MKV 29.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
static Standard_Integer CommandCmd 
(ClientData clientData, Tcl_Interp *interp,
 Standard_Integer argc, const char* argv[])
#else
static Standard_Integer CommandCmd 
(ClientData clientData, Tcl_Interp *interp,
 Standard_Integer argc, char* argv[])
#endif
{
  static Standard_Integer code;
  code = TCL_OK;
  CData* C = (CData*) clientData;
  Draw_Interpretor& di = *(C->i);

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
  char *err_name = NULL, *out_name = NULL;
  FILE * aFile_err = NULL;
  FILE * aFile_out = NULL;
  int fd_err_save = 0;
  int fd_out_save = 0;
  if (doLog)
  {
    aFile_out = capture_start (STDOUT_FILENO, &fd_out_save, out_name);
    aFile_err = capture_start (STDERR_FILENO, &fd_err_save, err_name);
  }

  // run command
  try {
    OCC_CATCH_SIGNALS

    // get exception if control-break has been pressed 
    OSD::ControlBreak();

    // OCC63: Convert strings from UTF-8 to local encoding, normally expected by OCC commands
    TclUTFToLocalStringSentry anArgs ( argc, (const char**)argv );
      
    Standard_Integer fres = C->f ( di, argc, anArgs.GetArgv() );
    if (fres != 0) 
      code = TCL_ERROR;
  }
  catch (Standard_Failure) {

    Handle(Standard_Failure) E = Standard_Failure::Caught();

    // fail if Draw_ExitOnCatch is set
    // MKV 29.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
    const char*  cc = Tcl_GetVar(interp,
			  "Draw_ExitOnCatch",TCL_GLOBAL_ONLY);
#else
    char* const cc = Tcl_GetVar(interp,
			  "Draw_ExitOnCatch",TCL_GLOBAL_ONLY);
#endif

    cout << "An exception was caught " << E << endl;

    if (cc && Draw::Atoi(cc)) {
#ifdef WNT
      Tcl_Exit(0);
#else      
      Tcl_Eval(interp,"exit");
#endif
    }

    // get the error message
    Standard_SStream ss;
    ss << "** Exception ** " << E << ends ;
#ifdef USE_STL_STREAM
    Tcl_SetResult(interp,(char*)(ss.str().c_str()),TCL_VOLATILE);
#else
    Tcl_SetResult(interp,(char*)(ss.str()),TCL_VOLATILE);
#endif    
    code = TCL_ERROR;
  }

  // flush streams
  flush_standard_streams();

  // end capturing cout and cerr 
  if (doLog) 
  {
    capture_end (aFile_err, STDERR_FILENO, fd_err_save, err_name, di.Log(), doEcho);
    capture_end (aFile_out, STDOUT_FILENO, fd_out_save, out_name, di.Log(), doEcho);
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


static void CommandDelete (ClientData clientData)
{
  CData *C = (CData*) clientData;
  delete C;
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
//function : Add
//purpose  : 
//=======================================================================
//#ifdef WNT
void Draw_Interpretor::Add(const Standard_CString n,
			   const Standard_CString help,
			   const Draw_CommandFunction f,
			   const Standard_CString group)
//#else
//void Draw_Interpretor::Add(const Standard_CString n,
//			   const Standard_CString help,
//			   const Draw_CommandFunction& f,
//			   const Standard_CString group)
//#endif
{
  Standard_PCharacter pN, pHelp, pGroup;
  //
  pN=(Standard_PCharacter)n;
  pHelp=(Standard_PCharacter)help;
  pGroup=(Standard_PCharacter)group;
  //
  if (myInterp==NULL) Init();

  CData* C = new CData(f,this);
  
  Tcl_CreateCommand(myInterp, pN ,CommandCmd, (ClientData) C, CommandDelete);

  // add the help
  Tcl_SetVar2(myInterp,"Draw_Helps", pN, pHelp, TCL_GLOBAL_ONLY);
  Tcl_SetVar2(myInterp,"Draw_Groups",pGroup,pN,
	      TCL_GLOBAL_ONLY|TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void Draw_Interpretor::Add(const Standard_CString n,
			   const Standard_CString help,
			   const Standard_CString file_name,
			   const Draw_CommandFunction f,
			   const Standard_CString group)
{
  Standard_PCharacter pN, pHelp, pGroup, pFileName;
  //
  pN=(Standard_PCharacter)n;
  pHelp=(Standard_PCharacter)help;
  pGroup=(Standard_PCharacter)group;
  pFileName=(Standard_PCharacter)file_name;
  //
  if (myInterp==NULL) Init();

  CData* C = new CData(f,this);
  Tcl_CreateCommand(myInterp,pN,CommandCmd, (ClientData) C, CommandDelete);

  // add the help
  Tcl_SetVar2(myInterp,"Draw_Helps",pN,pHelp,TCL_GLOBAL_ONLY);
  Tcl_SetVar2(myInterp,"Draw_Groups",pGroup,pN,
	      TCL_GLOBAL_ONLY|TCL_APPEND_VALUE|TCL_LIST_ELEMENT);

  // add path to source file (keep not more than two last subdirectories)
  OSD_Path aPath (pFileName);
  Standard_Integer nbTrek = aPath.TrekLength();
  for (Standard_Integer i = 2; i < nbTrek; i++)
    aPath.RemoveATrek (1);
  aPath.SetDisk("");
  aPath.SetNode("");
  TCollection_AsciiString aSrcPath;
  aPath.SystemName (aSrcPath);
  Tcl_SetVar2(myInterp,"Draw_Files",pN,aSrcPath.ToCString(),TCL_GLOBAL_ONLY);
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
#ifdef USE_STL_STREAM
  return Append (s.str().c_str());
#else
  // Note: use dirty tricks -- unavoidable with old streams 
  TCollection_AsciiString aStr (((Standard_SStream&)AReason).str(), AReason.pcount());
  ((Standard_SStream&)AReason).freeze (false);
  return Append (aStr.ToCString());
#endif
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
  Standard_PCharacter pLine;
  //
  pLine=(Standard_PCharacter)line;
  //
  return Tcl_Eval(myInterp,pLine);
}


//=======================================================================
//function : Eval
//purpose  : 
//=======================================================================

Standard_Integer Draw_Interpretor::RecordAndEval(const Standard_CString line,
						 const Standard_Integer flags)
{
  Standard_PCharacter pLine;
  //
  pLine=(Standard_PCharacter)line;
  return Tcl_RecordAndEval(myInterp,pLine,flags);
}

//=======================================================================
//function : EvalFile
//purpose  : 
//=======================================================================

Standard_Integer Draw_Interpretor::EvalFile(const Standard_CString fname)
{
  Standard_PCharacter pfname;
  //
  pfname=(Standard_PCharacter)fname;
  return Tcl_EvalFile(myInterp,pfname);
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
  return Tcl_CommandComplete(pLine);
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void Draw_Interpretor::Destroy()
{
  // MKV 01.02.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  try {
    OCC_CATCH_SIGNALS
    Tcl_Exit(0);
  }
  catch (Standard_Failure) {
#ifdef DEB
    cout <<"Tcl_Exit have an exeption" << endl;
#endif
  }
#else
#ifdef WNT
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
