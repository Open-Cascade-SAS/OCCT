// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2012 OPEN CASCADE SAS
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



#include <TDataXtd_Shape.ixx>
#include <TDataStd.hxx>
#include <TDataXtd.hxx>
#include <TDF_Label.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Solid.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming.hxx>

#define OCC2932
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
  while (1) {
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
    Standard_DomainError::Raise("TDataXtd_Shape::New : not an empty label");
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

#ifdef OCC2932
  Handle(TNaming_NamedShape) aNS;
  if(label.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
    if(!aNS->Get().IsNull())
      if(aNS->Get() == shape)
	return A;
  }
#endif

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

void TDataXtd_Shape::Restore(const Handle(TDF_Attribute)& with) 
{  
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataXtd_Shape::Paste (const Handle(TDF_Attribute)& into,
			   const Handle(TDF_RelocationTable)& RT) const
{  
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TDataXtd_Shape::References (const Handle(TDF_DataSet)& DS) const
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
