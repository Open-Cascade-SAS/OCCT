// Created on: 2007-07-31
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <TDataStd_AsciiString.ixx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

//=======================================================================
//function : TDataStd_AsciiString
//purpose  : 
//=======================================================================

TDataStd_AsciiString::TDataStd_AsciiString()
{
  myString.Clear();
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_AsciiString::GetID()
{
  static Standard_GUID theGUID ("3bbefc60-e618-11d4-ba38-0060b0ee18ea");
  return theGUID;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_AsciiString::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_AsciiString) TDataStd_AsciiString::Set (
                             const TDF_Label& theLabel,
                             const TCollection_AsciiString& theAsciiString)
{
  Handle(TDataStd_AsciiString) A;
  if (!theLabel.FindAttribute(TDataStd_AsciiString::GetID(), A))
  {
    A = new TDataStd_AsciiString;
    theLabel.AddAttribute(A);
  }
  A->Set(theAsciiString);
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Set (const TCollection_AsciiString& theAsciiString)
{
  Backup();
  myString = theAsciiString;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

const TCollection_AsciiString& TDataStd_AsciiString::Get () const
{
  return myString;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_AsciiString::NewEmpty () const
{
  return new TDataStd_AsciiString();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Restore (const Handle(TDF_Attribute)& theWith)
{
  Handle(TDataStd_AsciiString) R = Handle(TDataStd_AsciiString)::DownCast(theWith);
  myString = R->Get();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Paste (const Handle(TDF_Attribute)& theInto,
                              const Handle(TDF_RelocationTable)& /* RT */) const
{ 
  Handle(TDataStd_AsciiString) R = Handle(TDataStd_AsciiString)::DownCast (theInto);
  R->Set(myString);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_AsciiString::IsEmpty () const
{
  return myString.IsEmpty();
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_AsciiString::Dump(Standard_OStream& theOS) const
{
  Standard_OStream& anOS = TDF_Attribute::Dump( theOS );
  anOS << myString;
  return anOS;
}
