// Created on: 2000-09-08
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
#include <XCAFDoc_Area.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Area,TDF_Attribute)

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
