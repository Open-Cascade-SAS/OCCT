// File:	Draw_Interpretor.cxx
// Created:	Thu Feb 23 17:53:09 1995
// Author:	Remi LEQUETTE
//		<rle@bravox>


#include <Draw_Interpretor.ixx>
#include <Draw_Appli.hxx>
#include <Standard_SStream.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Macro.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <string.h>

#include <tcl.h>

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
  TclUTFToLocalStringSentry (int, const char **argv) : Argv((char**)argv) {}
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

  try {
    OCC_CATCH_SIGNALS

    // OCC63: Convert strings from UTF-8 to local encoding, normally expected by OCC commands
    TclUTFToLocalStringSentry anArgs ( argc, (const char**)argv );
      
    Draw_Interpretor& di = *(C->i);
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

    if (cc && atoi(cc)) {
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
  isAllocated(Standard_False)
{
// On ne cree pas tout de suite l'interpreteur tcl car s'il est detenu
// par une variable globale il est cree et ecrase avant le main().
  myInterp  = NULL;
}

//=======================================================================
//function : Init
//purpose  : Il faut appeler cette fonction
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
  myInterp(p)
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
  Standard_Integer length, num_slashes,  ii,  jj,  kk;
  Tcl_CreateCommand(myInterp,pN,CommandCmd, (ClientData) C, CommandDelete);

  // add the help
  Tcl_SetVar2(myInterp,"Draw_Helps",pN,pHelp,TCL_GLOBAL_ONLY);
  Tcl_SetVar2(myInterp,"Draw_Groups",pGroup,pN,
	      TCL_GLOBAL_ONLY|TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
  length = strlen(pFileName) ;
  char * a_string = 
    new char[length + 1] ;
  jj = 0 ;
  num_slashes = 0 ;
  ii = length ;
  while (num_slashes < 3 && ii >= 0) {
    if (file_name[ii] == '/') {
      num_slashes += 1 ;
    }
    ii -= 1 ; 
  } 
  jj = 0 ;
  for (kk = ii+2 , jj =0 ; kk < length ; kk++) {
     a_string[jj] = file_name[kk] ;
     jj += 1 ;
   }
  a_string[jj] = '\0' ;
 
  Tcl_SetVar2(myInterp,"Draw_Files",pN,a_string,TCL_GLOBAL_ONLY);

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
  sprintf(c,"%d",i);
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
  sprintf(s,"%.17g",r);
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
