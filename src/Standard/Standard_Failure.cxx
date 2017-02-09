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


#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Macro.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>

#include <string.h>
IMPLEMENT_STANDARD_RTTIEXT(Standard_Failure,Standard_Transient)

static Standard_CString allocate_message(const Standard_CString AString)
{
  Standard_CString aStr = 0;
  if(AString) {
    const Standard_Size aLen = strlen(AString);
    aStr = (Standard_CString) malloc(aLen+sizeof(Standard_Integer)+1);
    if (aStr) {
      Standard_PCharacter pStr=(Standard_PCharacter)aStr;
      strcpy(pStr+sizeof(Standard_Integer),AString);
      *((Standard_Integer*)aStr) = 1;
    }
  }
  return aStr;
}

static Standard_CString copy_message(Standard_CString aMessage)
{
  Standard_CString aStr = 0;
  if(aMessage) {
    aStr = aMessage;
    (*((Standard_Integer*)aStr))++;
  }
  return aStr;
}

static void deallocate_message(Standard_CString aMessage)
{
  if(aMessage) {
    (*((Standard_Integer*)aMessage))--;
    if(*((Standard_Integer*)aMessage)==0)
      free((void*)aMessage);
  }
}

// Define Standard_THREADLOCAL modifier as C++11 thread_local keyword where it is available.
#if defined(__clang__)
  // CLang version: standard CLang > 3.3 or XCode >= 8 (but excluding 32-bit ARM)
  // Note: this has to be in separate #if to avoid failure of preprocessor on other platforms
  #if __has_feature(cxx_thread_local)
    #define Standard_THREADLOCAL thread_local
  #else
    #define Standard_THREADLOCAL
  #endif
#elif (defined(__INTEL_COMPILER) && __INTEL_COMPILER > 1400) || \
      (defined(_MSC_VER) && _MSC_VER >= 1900) /* MSVC++ >= 14 */ || \
      (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))) /* GCC >= 4.8 */
  #define Standard_THREADLOCAL thread_local
#else
  #define Standard_THREADLOCAL
#endif

// ******************************************************************
//                           Standard_Failure                       *
// ******************************************************************
static Standard_THREADLOCAL Handle(Standard_Failure) RaisedError;

// ------------------------------------------------------------------
//
// ------------------------------------------------------------------
Standard_Failure::Standard_Failure ()
: myMessage(NULL) 
{
}

// ------------------------------------------------------------------
// Create returns mutable Failure;
// ------------------------------------------------------------------
Standard_Failure::Standard_Failure (const Standard_CString AString) 
:  myMessage(NULL)
{
  myMessage = allocate_message(AString);
}

Standard_Failure::Standard_Failure (const Standard_Failure& theFailure) 
: Standard_Transient(theFailure)
{
  myMessage = copy_message(theFailure.myMessage);
}

Standard_Failure::~Standard_Failure()
{
  deallocate_message(myMessage);
}

void Standard_Failure::SetMessageString(const Standard_CString AString)
{
  if ( AString == GetMessageString() ) return;
  deallocate_message(myMessage);
  myMessage = allocate_message(AString);
}

// ------------------------------------------------------------------
// Caught (myclass) returns mutable Failure raises NoSuchObject ;
// ------------------------------------------------------------------
Handle(Standard_Failure) Standard_Failure::Caught() 
{
  return RaisedError ;
}

// ------------------------------------------------------------------
// Raise (myclass; aMessage: CString = "") ;
// ------------------------------------------------------------------
void Standard_Failure::Raise (const Standard_CString AString) 
{ 
  Handle(Standard_Failure) E = new Standard_Failure()  ;
  E->Reraise (AString) ;
}

// ------------------------------------------------------------------
// Raise(myclass; aReason: in out SStream) ;
// ------------------------------------------------------------------
void Standard_Failure::Raise (const Standard_SStream& AReason) 
{ 
  Handle(Standard_Failure) E = new Standard_Failure();
  E->Reraise (AReason);
}

// ------------------------------------------------------------------
// Reraise (me: mutable; aMessage: CString) ;
// ------------------------------------------------------------------
void Standard_Failure::Reraise (const Standard_CString AString) 
{
  SetMessageString(AString);
  Reraise();
}

void Standard_Failure::Reraise (const Standard_SStream& AReason) 
{
  SetMessageString(AReason.str().c_str());
  Reraise();
}

void Standard_Failure::Reraise () 
{
  RaisedError = this;
  Throw();
}

void Standard_Failure::Jump()
{
#if defined (OCC_CONVERT_SIGNALS)
  Standard_ErrorHandler::Error (this);
  Standard_ErrorHandler::Abort (this);
#else
  RaisedError = this;
  Throw();
#endif
}


// ------------------------------------------------------------------
// Throw (me) is virtual ;
// ------------------------------------------------------------------
void Standard_Failure::Throw() const
{
  throw *this;
}

// ------------------------------------------------------------------
// Print (me; s: in out OStream) returns OStream;
// ------------------------------------------------------------------
void Standard_Failure::Print (Standard_OStream& AStream) const
{
if(myMessage){ 
    AStream << DynamicType() << ": " << GetMessageString(); 
 } 
 else { 
    AStream << DynamicType();
 }
}

Handle(Standard_Failure) Standard_Failure::NewInstance(const Standard_CString AString)
{
  return new Standard_Failure(AString)  ;
}

//=======================================================================
//function : GetMessageString
//purpose  : Returns error message
//=======================================================================
Standard_CString Standard_Failure::GetMessageString () const
{
  return (myMessage ? myMessage+sizeof(Standard_Integer) : "");
}

