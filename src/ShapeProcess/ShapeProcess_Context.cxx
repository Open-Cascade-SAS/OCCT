// Created on: 2000-08-21
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Resource_Manager.hxx>
#include <ShapeProcess_Context.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <sys/stat.h>
IMPLEMENT_STANDARD_RTTIEXT(ShapeProcess_Context,Standard_Transient)

//=======================================================================
//function : ShapeProcess_Context
//purpose  : 
//=======================================================================
ShapeProcess_Context::ShapeProcess_Context() 
{
  myMessenger = Message::DefaultMessenger();
  myTraceLev = 1;
}
	
//=======================================================================
//function : ShapeProcess_Context
//purpose  : 
//=======================================================================

ShapeProcess_Context::ShapeProcess_Context (const Standard_CString file,
                                            const Standard_CString scope)
{
  Init ( file, scope );
  myMessenger = Message::DefaultMessenger();
  myTraceLev = 1;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_Context::Init (const Standard_CString file,
                                             const Standard_CString scope)
{
  myScope.Nullify();
  myRC = LoadResourceManager ( file ); 
  if ( scope && scope[0] ) {
    SetScope ( scope );
  }
  return Standard_True; // myRC->Length() >0; NOT IMPLEMENTED
}

//=======================================================================
//function : LoadResourceManager
//purpose  : 
//=======================================================================

Handle(Resource_Manager) ShapeProcess_Context::LoadResourceManager (const Standard_CString name)
{
  // Optimisation of loading resource file: file is load only once
  // and reloaded only if file date has changed
  static Handle(Resource_Manager) sRC;
  static Standard_Time sMtime, sUMtime;
  static TCollection_AsciiString sName;

  struct stat buf;
  Standard_Time aMtime(0), aUMtime(0);
  TCollection_AsciiString aPath,aUserPath;
  Resource_Manager::GetResourcePath(aPath,name,Standard_False);
  Resource_Manager::GetResourcePath(aUserPath,name,Standard_True);
  if ( !aPath.IsEmpty() )
  {
    stat( aPath.ToCString(), &buf );
    aMtime = (Standard_Time)buf.st_mtime;
  }
  if ( !aUserPath.IsEmpty() )
  {
    stat( aUserPath.ToCString(), &buf );
    aUMtime = (Standard_Time)buf.st_mtime;
  }

  Standard_Boolean isFileModified = Standard_False;
  if ( !sRC.IsNull() ) {
    if ( sName.IsEqual ( name ) ) {
      if ( sMtime != aMtime )
      {
        sMtime = aMtime;
        isFileModified = Standard_True;
      }
      if ( sUMtime != aUMtime )
      {
        sUMtime = aUMtime;
        isFileModified = Standard_True;
      }
      if (isFileModified)
        sRC.Nullify();
    }
    else
      sRC.Nullify();
  }
  if ( sRC.IsNull() ) {
#ifdef OCCT_DEBUG
    cout << "Info: ShapeProcess_Context: Reload Resource_Manager: " 
         << sName.ToCString() << " -> " << name << endl;
#endif
    sRC = new Resource_Manager ( name );
    if (!isFileModified)
    {
      sName = name;
      sMtime = aMtime;
      sUMtime = aUMtime;
    }
  }
  return sRC;
}

//=======================================================================
//function : ResourceManager
//purpose  : 
//=======================================================================

const Handle(Resource_Manager) &ShapeProcess_Context::ResourceManager () const
{
  return myRC;
}

//=======================================================================
//function : SetScope
//purpose  : 
//=======================================================================

void ShapeProcess_Context::SetScope (const Standard_CString scope)
{
  if ( myScope.IsNull() ) myScope = new TColStd_HSequenceOfHAsciiString;
  Handle(TCollection_HAsciiString) str;
  if ( myScope->Length() >0 ) {
    str = new TCollection_HAsciiString ( myScope->Value ( myScope->Length() ) );
    str->AssignCat ( "." );
    str->AssignCat ( scope );
  }
  else str = new TCollection_HAsciiString ( scope );
  myScope->Append ( str );
}
	
//=======================================================================
//function : UnSetScope
//purpose  : 
//=======================================================================

void ShapeProcess_Context::UnSetScope ()
{
  if ( ! myScope.IsNull() && myScope->Length() >0 ) 
    myScope->Remove ( myScope->Length() );
}
	
//=======================================================================
//function : IsParamSet
//purpose  : 
//=======================================================================

static Handle(TCollection_HAsciiString) MakeName (const Handle(TColStd_HSequenceOfHAsciiString) &scope,
						  const Standard_CString param)
{
  Handle(TCollection_HAsciiString) str;
  if ( ! scope.IsNull() && scope->Length() >0 ) {
    str = new TCollection_HAsciiString ( scope->Value ( scope->Length() )->String() );
    str->AssignCat (".");
    str->AssignCat ( param );
  }
  else str = new TCollection_HAsciiString ( param );
  return str;
}

Standard_Boolean ShapeProcess_Context::IsParamSet (const Standard_CString param) const
{
  return ! myRC.IsNull() && myRC->Find ( MakeName ( myScope, param )->ToCString() );
}

//=======================================================================
//function : GetString
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_Context::GetString (const Standard_CString param,
                                                  TCollection_AsciiString &str) const
{
  if ( myRC.IsNull() ) return Standard_False;
  Handle(TCollection_HAsciiString) pname = MakeName ( myScope, param );
  if ( ! myRC->Find ( pname->ToCString() ) ) {
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << pname->ToCString() << " is not defined" << endl;
#endif
    return Standard_False;
  }
  str = myRC->Value ( pname->ToCString() );
  return Standard_True;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_Context::GetReal (const Standard_CString param,
                                                Standard_Real &val) const
{
  if ( myRC.IsNull() ) return Standard_False;
  
  TCollection_AsciiString str;
  if ( ! GetString ( param, str ) ) return Standard_False;
  
  if ( str.IsRealValue() ) {
    val = str.RealValue();
    return Standard_True;
  }

  // if not real, try to treat as alias "&param"
  str.LeftAdjust();
  if ( str.Value(1) == '&' ) {
    TCollection_AsciiString ref = str.Split ( 1 );
    ref.LeftAdjust();
    ref.RightAdjust();
    if ( ! myRC->Find ( ref.ToCString() ) ) {
#ifdef OCCT_DEBUG
      cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << ref.ToCString() << " is not defined" << endl;
#endif
      return Standard_False;
    }
    str = myRC->Value ( ref.ToCString() );
    if ( str.IsRealValue() ) {
      val = str.RealValue();
      return Standard_True;
    }
  }
#ifdef OCCT_DEBUG
  cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << param << " is neither Real nor reference to Real";
#endif
  return Standard_False;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_Context::GetInteger (const Standard_CString param,
                                                   Standard_Integer &val) const
{
  if ( myRC.IsNull() ) return Standard_False;
  
  TCollection_AsciiString str;
  if ( ! GetString ( param, str ) ) return Standard_False;
  
  if ( str.IsIntegerValue() ) {
    val = str.IntegerValue();
    return Standard_True;
  }

  // if not integer, try to treat as alias "&param"
  str.LeftAdjust();
  if ( str.Value(1) == '&' ) {
    TCollection_AsciiString ref = str.Split ( 1 );
    ref.LeftAdjust();
    ref.RightAdjust();
    if ( ! myRC->Find ( ref.ToCString() ) ) {
#ifdef OCCT_DEBUG
      cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << ref.ToCString() << " is not defined" << endl;
#endif
      return Standard_False;
    }
    str = myRC->Value ( ref.ToCString() );
    if ( str.IsIntegerValue() ) {
      val = str.IntegerValue();
      return Standard_True;
    }
  }
#ifdef OCCT_DEBUG
  cout << "Warning: ShapeProcess_Context::GetInteger(): Parameter " << param << " is neither Integer nor reference to Integer";
#endif
  return Standard_False;
}

//=======================================================================
//function : GetBoolean
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_Context::GetBoolean (const Standard_CString param,
                                                   Standard_Boolean &val) const
{
  if ( myRC.IsNull() ) return Standard_False;
  try {
    OCC_CATCH_SIGNALS
    val = myRC->Integer (MakeName (myScope, param)->ToCString()) != 0;
    return Standard_True;
  }
  catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeProcess_Context::GetInteger(): " << param << ": ";
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
  }
  return Standard_False;
}

//=======================================================================
//function : RealVal
//purpose  : 
//=======================================================================

Standard_Real ShapeProcess_Context::RealVal (const Standard_CString param,
                                             const Standard_Real def) const
{
  Standard_Real val;
  return GetReal ( param, val ) ? val : def;
}

//=======================================================================
//function : BooleanVal
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess_Context::BooleanVal (const Standard_CString param,
                                                   const Standard_Boolean def) const
{
  Standard_Boolean val;
  return GetBoolean ( param, val ) ? val : def;
}

//=======================================================================
//function : IntegerVal
//purpose  : 
//=======================================================================

Standard_Integer ShapeProcess_Context::IntegerVal (const Standard_CString param,
                                                   const Standard_Integer def) const
{
  Standard_Integer val;
  return GetInteger ( param, val ) ? val : def;
}

//=======================================================================
//function : StringVal
//purpose  : 
//=======================================================================

Standard_CString ShapeProcess_Context::StringVal (const Standard_CString param,
                                                  const Standard_CString def) const
{
  if ( myRC.IsNull() ) return def;
  try {
    OCC_CATCH_SIGNALS
    return myRC->Value ( MakeName ( myScope, param )->ToCString() );
  }
  catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeProcess_Context::GetInteger(): " << param << ": ";
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
  }
  return def;
}

//=======================================================================
//function : SetMessenger
//purpose  : 
//=======================================================================

void ShapeProcess_Context::SetMessenger (const Handle(Message_Messenger)& messenger)
{
  if ( messenger.IsNull() )
    myMessenger = Message::DefaultMessenger();
  else
    myMessenger = messenger;
}

//=======================================================================
//function : Messenger
//purpose  : 
//=======================================================================

Handle(Message_Messenger) ShapeProcess_Context::Messenger () const
{
  return myMessenger;
}

//=======================================================================
//function : SetProgress
//purpose  : 
//=======================================================================

void ShapeProcess_Context::SetProgress (const Handle(Message_ProgressIndicator)& progress)
{
  myProgress = progress;
}

//=======================================================================
//function : Progress
//purpose  : 
//=======================================================================

Handle(Message_ProgressIndicator) ShapeProcess_Context::Progress() const
{
  return myProgress;
}

//=======================================================================
//function : SetTraceLevel
//purpose  : 
//=======================================================================

void ShapeProcess_Context::SetTraceLevel (const Standard_Integer tracelev)
{
  myTraceLev = tracelev;
}

//=======================================================================
//function : TraceLevel
//purpose  : 
//=======================================================================

Standard_Integer ShapeProcess_Context::TraceLevel () const
{
  return myTraceLev;
}
