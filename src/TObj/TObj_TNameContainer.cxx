// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_TNameContainer.hxx>
#include <Standard_GUID.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_TNameContainer,TDF_Attribute)
IMPLEMENT_STANDARD_RTTIEXT(TObj_TNameContainer,TDF_Attribute)

//=======================================================================
//function : TObj_TNameContainer
//purpose  : 
//=======================================================================

TObj_TNameContainer::TObj_TNameContainer()
{
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TNameContainer::GetID()
{
  static Standard_GUID theID ("3bbefb47-e618-11d4-ba38-0060b0ee18ea");
  return theID;
}
  
//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TNameContainer::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TObj_TNameContainer) TObj_TNameContainer::Set(const TDF_Label& theLabel)
{
  Handle(TObj_TNameContainer) A;
  if (!theLabel.FindAttribute(TObj_TNameContainer::GetID(), A)) 
  {
    A = new TObj_TNameContainer;
    theLabel.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : RecordName
//purpose  : 
//=======================================================================

void TObj_TNameContainer::RecordName(const Handle(TCollection_HExtendedString)& theName,
                                         const TDF_Label& theLabel)
{
  if(!IsRegistered(theName)) 
  {
    Backup();
    myMap.Bind(new TCollection_HExtendedString(theName->String()),theLabel);
  }
}

//=======================================================================
//function : RemoveName
//purpose  : 
//=======================================================================

void TObj_TNameContainer::RemoveName(const Handle(TCollection_HExtendedString)& theName)
{
  if(IsRegistered(theName)) 
  {
    Backup();
    myMap.UnBind(theName);
  }
}

void TObj_TNameContainer::Clear()
{
  Backup();
  myMap.Clear();
}

//=======================================================================
//function : CheckName
//purpose  : 
//=======================================================================

Standard_Boolean TObj_TNameContainer::IsRegistered(const Handle(TCollection_HExtendedString)& theName) const
{
  return myMap.IsBound(theName);
}
  
//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TObj_TNameContainer::NewEmpty () const
{  
  return new TObj_TNameContainer();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TObj_TNameContainer::Restore(const Handle(TDF_Attribute)& theWith) 
{
  Handle(TObj_TNameContainer) R = Handle(TObj_TNameContainer)::DownCast (theWith);
  
  myMap = R->Get();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TObj_TNameContainer::Paste (const Handle(TDF_Attribute)& theInto,
                                     const Handle(TDF_RelocationTable)& /* RT */) const
{ 
  Handle(TObj_TNameContainer) R = Handle(TObj_TNameContainer)::DownCast (theInto);
  R->Set(myMap);
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

const TObj_DataMapOfNameLabel& TObj_TNameContainer::Get() const
{
  return myMap;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TObj_TNameContainer::Set(const TObj_DataMapOfNameLabel& theMap)
{
  Backup();
  myMap = theMap;
}
