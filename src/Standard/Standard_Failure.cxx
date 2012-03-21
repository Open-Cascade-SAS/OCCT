// Copyright (c) 1998-1999 Matra Datavision
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


#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.ixx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_Type.hxx>
#include <Standard_Macro.hxx>
#include <string.h>
#include <Standard_PCharacter.hxx>

static Standard_CString allocate_message(const Standard_CString AString)
{
  Standard_CString aStr = 0;
  if(AString) {
    const Standard_Size aLen = strlen(AString);
    aStr = new Standard_Character[aLen+sizeof(Standard_Integer)+1];
    Standard_PCharacter pStr=(Standard_PCharacter)aStr;
    strcpy(pStr+sizeof(Standard_Integer),AString);
    *((Standard_Integer*)aStr) = 1;
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
      delete [](Standard_PCharacter)aMessage;
  }
}


// ******************************************************************
//                           Standard_Failure                       *
// ******************************************************************
#ifndef NO_CXX_EXCEPTION
static Handle(Standard_Failure) RaisedError;
#endif
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

Standard_Failure::Standard_Failure (const Standard_Failure& aFailure) 
{
  myMessage = copy_message(aFailure.myMessage);
}

void Standard_Failure::Destroy()
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
#ifdef NO_CXX_EXCEPTION
  return Standard_ErrorHandler::LastCaughtError();
#else
  return RaisedError ;
#endif
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
#ifdef USE_STL_STREAM
  SetMessageString(AReason.str().c_str());
#else
  // Note: use dirty tricks -- unavoidable with old streams 
  ((Standard_SStream&)AReason) << ends;
  SetMessageString(((Standard_SStream&)AReason).str());
  ((Standard_SStream&)AReason).freeze (false);
#endif
  Reraise();
}

void Standard_Failure::Reraise () 
{
#ifdef NO_CXX_EXCEPTION
  Standard_ErrorHandler::Error (this);
  Standard_ErrorHandler::Abort (this);
#else
  RaisedError = this;
  Throw();
#endif
}

void Standard_Failure::Jump() const 
{
#if defined (NO_CXX_EXCEPTION) || defined (OCC_CONVERT_SIGNALS)
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
#ifndef NO_CXX_EXCEPTION
  throw *this;
#endif
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
