// Created on: 2000-09-08
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



#include <XCAFDoc_Area.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

XCAFDoc_Area::XCAFDoc_Area()
{
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Area::GetID() 
{
  static Standard_GUID AreaID ("efd212f2-6dfd-11d4-b9c8-0060b0ee281b");
  return AreaID;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Area::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_Area) XCAFDoc_Area::Set (const TDF_Label& L,const Standard_Real V) 
{
  Handle(XCAFDoc_Area) A;
  if (!L.FindAttribute(XCAFDoc_Area::GetID(), A)) {
    A = new XCAFDoc_Area;
    L.AddAttribute(A);
  }
  A->Set(V); 
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void XCAFDoc_Area::Set (const Standard_Real V) 
{
  Backup();
  myValue = V;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Real XCAFDoc_Area::Get() const { return myValue; }

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_Area::Get(const TDF_Label& label,Standard_Real& area) 
{
  Handle(XCAFDoc_Area) anArea;
  if (!label.FindAttribute(XCAFDoc_Area::GetID(), anArea))
    return Standard_False;
  
  area = anArea->Get();
  return Standard_True;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_Area::NewEmpty () const
{  
  return new XCAFDoc_Area(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_Area::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(XCAFDoc_Area) R = Handle(XCAFDoc_Area)::DownCast (With);
  myValue = R->Get();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_Area::Paste (const Handle(TDF_Attribute)& Into,
			   const Handle(TDF_RelocationTable)& /* RT */) const
{ 
  Handle(XCAFDoc_Area) R = Handle(XCAFDoc_Area)::DownCast (Into);
  R->Set(myValue);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& XCAFDoc_Area::Dump (Standard_OStream& anOS) const
{  
  anOS << "Area "; 
  anOS << Get();
  return anOS;
}
