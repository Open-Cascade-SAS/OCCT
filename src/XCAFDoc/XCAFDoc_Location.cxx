// Created on: 2000-08-15
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <XCAFDoc_Location.ixx>

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

 TopLoc_Location XCAFDoc_Location::Get() const
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

