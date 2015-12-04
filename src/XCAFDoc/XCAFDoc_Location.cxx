// Created on: 2000-08-15
// Created by: data exchange team
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TopLoc_Location.hxx>
#include <XCAFDoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Location,TDF_Attribute)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
XCAFDoc_Location::XCAFDoc_Location()
{
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Location::GetID() 
{
  static Standard_GUID LocationID ("efd212ef-6dfd-11d4-b9c8-0060b0ee281b");
  return LocationID; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 Handle(XCAFDoc_Location) XCAFDoc_Location::Set(const TDF_Label& L,const TopLoc_Location& Loc) 
{
  Handle(XCAFDoc_Location) A;
  if (!L.FindAttribute (XCAFDoc_Location::GetID(), A)) {
    A = new XCAFDoc_Location ();
    L.AddAttribute(A);
  }
  A->Set (Loc); 
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 void XCAFDoc_Location::Set(const TopLoc_Location& Loc) 
{
  Backup();
  myLocation = Loc;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

const TopLoc_Location& XCAFDoc_Location::Get() const
{
  return myLocation;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Location::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

 void XCAFDoc_Location::Restore(const Handle(TDF_Attribute)& With) 
{
  myLocation = Handle(XCAFDoc_Location)::DownCast(With)->Get();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XCAFDoc_Location::NewEmpty() const
{
  return new XCAFDoc_Location();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void XCAFDoc_Location::Paste(const Handle(TDF_Attribute)& Into,const Handle(TDF_RelocationTable)& /* RT */) const
{
  Handle(XCAFDoc_Location)::DownCast(Into)->Set(myLocation);

}

