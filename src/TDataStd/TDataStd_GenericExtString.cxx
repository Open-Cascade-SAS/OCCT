// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <TDataStd_GenericExtString.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_GenericExtString,TDF_Attribute)

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void TDataStd_GenericExtString::Set (const TCollection_ExtendedString& S) 
{
  if(myString == S) return;
 
  Backup();
  myString = S;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
const TCollection_ExtendedString& TDataStd_GenericExtString::Get () const
{
  return myString;
}

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================
void TDataStd_GenericExtString::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;

  Backup();
  myID = theGuid;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_GenericExtString::ID () const { return myID; }

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_GenericExtString::Restore(const Handle(TDF_Attribute)& with) 
{
   Handle(TDataStd_GenericExtString) anAtt = Handle(TDataStd_GenericExtString)::DownCast (with);
   myString = anAtt->Get();
   myID = anAtt->ID();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_GenericExtString::Paste (const Handle(TDF_Attribute)& into,
                                       const Handle(TDF_RelocationTable)&/* RT*/) const
{
  Handle(TDataStd_GenericExtString) anAtt = Handle(TDataStd_GenericExtString)::DownCast (into);
  anAtt->Set(myString);
  anAtt->SetID(myID);
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void TDataStd_GenericExtString::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, TDF_Attribute)
  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, Get())
  OCCT_DUMP_FIELD_VALUE_GUID (theOStream, myID)
}
