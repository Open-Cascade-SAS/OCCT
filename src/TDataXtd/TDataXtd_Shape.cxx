// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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


#include <BRep_Builder.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataXtd_Shape,TDF_Attribute)

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Shape::Find (const TDF_Label& current,
				      Handle(TDataXtd_Shape)& S) 
{  
  TDF_Label L = current;
  Handle(TDataXtd_Shape) SA;
  if (L.IsNull()) return Standard_False; 
  for(;;) {
    if(L.FindAttribute(TDataXtd_Shape::GetID(), SA))  break;
    L = L.Father();
    if (L.IsNull()) break; 
  }
  
  
  if (!SA.IsNull()) { 
    S = SA;
    return Standard_True; 
  }
  return Standard_False; 
}


//=======================================================================
//function : New
//purpose  : 
//=======================================================================

Handle(TDataXtd_Shape) TDataXtd_Shape::New (const TDF_Label& label)
{  
  if (label.HasAttribute()) {
    throw Standard_DomainError("TDataXtd_Shape::New : not an empty label");
  }
  Handle(TDataXtd_Shape) A = new TDataXtd_Shape ();  
  label.AddAttribute(A);                     
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataXtd_Shape) TDataXtd_Shape::Set (const TDF_Label& label, const TopoDS_Shape& shape)
{
  Handle(TDataXtd_Shape) A;  
  if (!label.FindAttribute(TDataXtd_Shape::GetID(),A)) { 
    A = TDataXtd_Shape::New (label);
  }  

  Handle(TNaming_NamedShape) aNS;
  if(label.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
    if(!aNS->Get().IsNull())
      if(aNS->Get() == shape)
	return A;
  }

  TNaming_Builder B(label);
  B.Generated(shape);
  return A; 
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

TopoDS_Shape TDataXtd_Shape::Get (const TDF_Label& label)
{
  TopoDS_Shape shape;

  Handle(TNaming_NamedShape) NS;
  if( label.FindAttribute(TNaming_NamedShape::GetID(), NS)) {
    shape = TNaming_Tool::GetShape(NS);
    return shape;
  }

  shape.Nullify();  
  return shape; 
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Shape::GetID() 
{
  static Standard_GUID TDataXtd_ShapeID("2a96b620-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_ShapeID;
}

//=======================================================================
//function : TDataXtd_Shape
//purpose  : 
//=======================================================================

TDataXtd_Shape::TDataXtd_Shape()
{
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Shape::ID() const
{ return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataXtd_Shape::NewEmpty () const
{  
  return new TDataXtd_Shape(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataXtd_Shape::Restore(const Handle(TDF_Attribute)& ) 
{  
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataXtd_Shape::Paste (const Handle(TDF_Attribute)&,
			   const Handle(TDF_RelocationTable)&) const
{  
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TDataXtd_Shape::References (const Handle(TDF_DataSet)&) const
{  
}
 
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataXtd_Shape::Dump (Standard_OStream& anOS) const
{  
  anOS << "Shape";
  return anOS;
}
