// Created on: 1996-09-17
// Created by: Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <StdSelect_ShapeTypeFilter.ixx>
#include <StdSelect_BRepOwner.hxx>


//==================================================
// Function: StdSelect_ShapeTypeFilter
// Purpose : Constructeur
//==================================================

StdSelect_ShapeTypeFilter::StdSelect_ShapeTypeFilter(const TopAbs_ShapeEnum aType):
myType(aType){}


//==================================================
// Function: IsOk
// Purpose : Renvoie True si la shape est du type defini a la construction
//==================================================

Standard_Boolean StdSelect_ShapeTypeFilter::IsOk(const Handle(SelectMgr_EntityOwner)& EO)  const 
{ 
  Handle(StdSelect_BRepOwner) BRO = Handle(StdSelect_BRepOwner)::DownCast( EO );
  if( BRO.IsNull() || !BRO->HasShape() ) return Standard_False;
  const TopoDS_Shape& anobj= BRO->Shape();
  return anobj.ShapeType() == myType;
}
Standard_Boolean StdSelect_ShapeTypeFilter::ActsOn(const TopAbs_ShapeEnum aStandardMode) const 
{return aStandardMode==myType;}
