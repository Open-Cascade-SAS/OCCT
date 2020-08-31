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
#include <Message.hxx>
#include <Message_PrinterOStream.hxx>
#include <OSD.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Process.hxx>
#include <Standard_SStream.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <string.h>
#include <tcl.h>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#endif

// for capturing of cout and cerr (dup(), dup2())
#ifdef _WIN32
#include <io.h>
#include <sys/stat.h>  
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
    os << std::endl;
  }

  void flush_standard_streams ()
  {
    fflush (stderr);
    fflush (stdout);
    std::cerr << std::flush;
    std::cout << std::flush;
  }

  int capture_start (int theFDStd, int theFDLog)
  {
    Standard_ASSERT_RETURN (theFDLog >= 0, "Invalid descriptor of log file", -1);

    // Duplicate a file descriptor of the standard stream to be able to restore output to it later
    int aFDSave = dup (theFDStd);
    if (aFDSave < 0)
    {
      perror ("Error capturing standard stream to log: dup() returned");
      return -1;
    }

    // Redirect the stream to the log file
    if (dup2 (theFDLog, theFDStd) < 0)
    {
      close (aFDSave);
      perror ("Error capturing standard stream to log: dup2() returned");
      return -1;
    }

    // remember saved file descriptor of standard stream
    return aFDSave;
  }

  void capture_end (int theFDStd, int& theFDSave)
  {
    if (theFDSave < 0)
      return;

    // restore normal descriptors of console stream
    if (dup2(theFDSave, theFDStd) < 0)
    {
      perror ("Error returning capturing standard stream to log: dup2() returned");
      return;
    }

    // close saved file descriptor
    close(theFDSave);
    theFDSave = -1;
  }

} // anonymous namespace

static Standard_Integer CommandCmd (ClientData theClientData, Tcl_Interp* interp, Standard_Integer argc, const char* argv[])
{
  Standard_Integer code = TCL_OK;
  Draw_Interpretor::CallBackData* aCallback = (Draw_Interpretor::CallBackData* )theClientData;
  Draw_Interpretor& di = *(aCallback->myDI);

  // log command execution, except commands manipulating log itself and echo
  Standard_Boolean isLogManipulation = (strcmp (argv[0], "dlog") == 0 || 
                                        strcmp (argv[0], "decho") == 0);
  Standard_Boolean doLog  = (di.GetDoLog() && ! isLogManipulation);
  Standard_Boolean doEcho = (di.GetDoEcho() && ! isLogManipulation);

  // flush cerr and cout
  flush_standard_streams();

  // capture cout and cerr to log
  int aFDstdout = STDOUT_FILENO;
  int aFDstderr = STDERR_FILENO;
  int aFDerr_save = -1;
  int aFDout_save = -1;
  if (doLog)
  {
    aFDout_save = capture_start (aFDstdout, di.GetLogFileDescriptor());
    aFDerr_save = capture_start (aFDstderr, di.GetLogFileDescriptor());
  }

  if (doEcho || doLog)
    dumpArgs (std::cout, argc, argv);

  // run command
  try
  {
    OCC_CATCH_SIGNALS

    // get exception if control-break has been pressed 
    OSD::ControlBreak();

    Standard_Integer fres = aCallback->Invoke ( di, argc, argv /*anArgs.GetArgv()*/ );
    if (fres != 0)
    {
      code = TCL_ERROR;
    }
  }
  catch (Standard_Failure const& anException)
  {
    // fail if Draw_ExitOnCatch is set
    const char* toExitOnCatch = Tcl_GetVar (interp, "Draw_ExitOnCatch", TCL_GLOBAL_ONLY);
    if (toExitOnCatch != NULL && Draw::Atoi (toExitOnCatch))
    {
      Message::SendFail() << "An exception was caught " << anException;
#ifdef _WIN32
      Tcl_Exit(0);
#else      
      Tcl_Eval(interp,"exit");
#endif
    }

    Standard_SStream ss;
    ss << "An exception was caught " << anException << std::ends;
    Tcl_SetResult(interp,(char*)(ss.str().c_str()),TCL_VOLATILE);
    code = TCL_ERROR;
  }
  catch (std::exception const& theStdException)
  {
    const char* toExitOnCatch = Tcl_GetVar (interp, "Draw_ExitOnCatch", TCL_GLOBAL_ONLY);
    if (toExitOnCatch != NULL && Draw::Atoi (toExitOnCatch))
    {
      Message::SendFail() << "An exception was caught " << theStdException.what() << " [" << typeid(theStdException).name() << "]";
    #ifdef _WIN32
      Tcl_Exit (0);
    #else
      Tcl_Eval (interp, "exit");
    #endif
    }

    Standard_SStream ss;
    ss << "An exception was caught " << theStdException.what() << " [" << typeid(theStdException).name() << "]" << std::ends;
    Tcl_SetResult(interp,(char*)(ss.str().c_str()),TCL_VOLATILE);
    code = TCL_ERROR;
  }
  catch (...)
  {
    const char* toExitOnCatch = Tcl_GetVar (interp, "Draw_ExitOnCatch", TCL_GLOBAL_ONLY);
    if (toExitOnCatch != NULL && Draw::Atoi (toExitOnCatch))
    {
      Message::SendFail() << "UNKNOWN exception was caught ";
    #ifdef _WIN32
      Tcl_Exit (0);
    #else
      Tcl_Eval (interp,"exit");
    #endif
    }

    Standard_SStream ss;
    ss << "UNKNOWN exception was caught " << std::ends;
    Tcl_SetResult(interp,(char*)(ss.str().c_str()),TCL_VOLATILE);
    code = TCL_ERROR;
  }

  // log command result
  if (doLog || doEcho)
  {
    const char* aResultStr = Tcl_GetStringResult (interp);
    if (aResultStr != 0 && aResultStr[0] != '\0' )
    {
      std::cout << aResultStr << std::endl;
    }
  }

  // flush streams
  flush_standard_streams();

  // end capturing cout and cerr 
  if (doLog) 
  {
    capture_end (aFDstderr, aFDerr_save);
    capture_end (aFDstdout, aFDout_save);
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
Draw_Interpretor::Draw_Interpretor()
: // the tcl interpreter is not created immediately as it is kept
  // by a global variable and created and deleted before the main()
  myInterp (NULL),
  isAllocated (Standard_False),
  myDoLog (Standard_False),
  myDoEcho (Standard_False),
  myToColorize (Standard_True),
  myFDLog (-1)
{
  //
}

//=======================================================================
//function : Draw_Interpretor
//purpose  :
//=======================================================================
Draw_Interpretor::Draw_Interpretor (const Draw_PInterp& theInterp)
: myInterp (theInterp),
  isAllocated (Standard_False),
  myDoLog (Standard_False),
  myDoEcho (Standard_False),
  myToColorize (Standard_True),
  myFDLog (-1)
{
  //
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
//function : SetToColorize
//purpose  :
//=======================================================================
void Draw_Interpretor::SetToColorize (Standard_Boolean theToColorize)
{
  myToColorize = theToColorize;
  for (Message_SequenceOfPrinters::Iterator aPrinterIter (Message::DefaultMessenger()->Printers());
       aPrinterIter.More(); aPrinterIter.Next())
  {
    if (Handle(Message_PrinterOStream) aPrinter = Handle(Message_PrinterOStream)::DownCast (aPrinterIter.Value()))
    {
      aPrinter->SetToColorize (theToColorize);
    }
  }
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
  Standard_ASSERT_RAISE (myInterp != NULL, "Attempt to add command to Null interpretor");

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
  Tcl_AppendResult(myInterp,s,(Standard_CString)0);
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
  Tcl_AppendElement(myInterp, s);
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
  SetDoLog (Standard_False);
  if (myFDLog >=0)
  {
    close (myFDLog);
    myFDLog = 0;
  }

  // MKV 01.02.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  try {
    OCC_CATCH_SIGNALS
    Tcl_Exit(0);
  }
  catch (Standard_Failure const&) {
#ifdef OCCT_DEBUG
    std::cout <<"Tcl_Exit have an exeption" << std::endl;
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
  if (myDoLog == doLog)
    return;

  // create log file if not opened yet
  if (doLog && myFDLog < 0)
  {
#ifdef _WIN32
    char tmpfile[L_tmpnam + 1];
    tmpnam(tmpfile);
    myFDLog = open (tmpfile, O_RDWR | O_CREAT | O_EXCL | O_TEMPORARY, S_IREAD | S_IWRITE);
#else
    // according to Linux Filesystem Hierarchy Standard, 3.17,
    // /tmp/ is the right directory for temporary files
    char tmpfile[256] = "/tmp/occt_draw_XXXXXX";
    myFDLog = mkstemp (tmpfile);
    if (myFDLog >= 0)
    {
//      printf ("Tmp file: %s\n", tmpfile);
      unlink (tmpfile); // make sure the file will be deleted on close
    }
#endif
    if (myFDLog < 0)
    {
      perror ("Error creating temporary file for capturing console output");
      printf ("path: %s\n", tmpfile);
      return;
    }
  }

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

void Draw_Interpretor::ResetLog ()
{
  if (myFDLog < 0)
    return;

  // flush cerr and cout, for the case if they are bound to the log
  flush_standard_streams();

  lseek (myFDLog, 0, SEEK_SET);

#ifdef _WIN32
  if (_chsize_s (myFDLog, 0) != 0)
#else
  if (ftruncate (myFDLog, 0) != 0)
#endif
  {
    perror ("Error truncating the console log");
  }
}

void Draw_Interpretor::AddLog (const Standard_CString theStr)
{
  if (myFDLog < 0 || ! theStr || ! theStr[0])
    return;

  // flush cerr and cout, for the case if they are bound to the log
  flush_standard_streams();

  // write as plain bytes
  if (write (myFDLog, theStr, (unsigned int)strlen(theStr)) <0)
  {
    perror ("Error writing to console log");
  }
}

TCollection_AsciiString Draw_Interpretor::GetLog ()
{
  TCollection_AsciiString aLog;
  if (myFDLog < 0)
    return aLog;

  // flush cerr and cout
  flush_standard_streams();

  // rewind the file to its start
  lseek (myFDLog, 0, SEEK_SET);

  // read the whole log to string; this implementation
  // is not optimized but should be sufficient
  const int BUFSIZE = 4096;
  char buffer[BUFSIZE + 1];
  for (;;)
  {
    int nbRead = read (myFDLog, buffer, BUFSIZE);
    if (nbRead <= 0)
    {
      break;
    }
    buffer[nbRead] = '\0';
    aLog.AssignCat (buffer);
  }

  return aLog;
}
