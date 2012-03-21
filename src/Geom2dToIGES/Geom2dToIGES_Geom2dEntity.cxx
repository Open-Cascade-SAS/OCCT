// Created on: 1995-09-13
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
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

//rln 06.01.98 new method SetUnit


#include <Geom2dToIGES_Geom2dEntity.ixx>

#include <Geom2dToIGES_Geom2dCurve.hxx>
#include <Geom2dToIGES_Geom2dPoint.hxx>
#include <Geom2dToIGES_Geom2dVector.hxx>

#include <IGESData_IGESModel.hxx>


//=======================================================================
//function : Geom2dToIGES_Geom2dEntity
//purpose  : 
//=======================================================================

Geom2dToIGES_Geom2dEntity::Geom2dToIGES_Geom2dEntity()
{  
}


//=======================================================================
//function : Geom2dToIGES_Geom2dEntity
//purpose  : 
//=======================================================================

Geom2dToIGES_Geom2dEntity::Geom2dToIGES_Geom2dEntity
(const Geom2dToIGES_Geom2dEntity& other)
{
  TheUnitFactor = other.GetUnit();
  TheModel      = other.GetModel();
}


//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================
void Geom2dToIGES_Geom2dEntity::SetModel(const Handle(IGESData_IGESModel)& model)
{  
  TheModel = model;  
  Standard_Real unitfactor = TheModel->GlobalSection().UnitValue();
  TheUnitFactor = unitfactor;
}


//=======================================================================
//function : GetModel
//purpose  : 
//=======================================================================
Handle(IGESData_IGESModel) Geom2dToIGES_Geom2dEntity::GetModel() const
{ 
  return TheModel; 
}


//=======================================================================
//function : GetUnit
//purpose  : 
//=======================================================================
void Geom2dToIGES_Geom2dEntity::SetUnit(const Standard_Real unit)
{
  TheUnitFactor = unit;
}

//=======================================================================
//function : GetUnit
//purpose  : 
//=======================================================================
Standard_Real Geom2dToIGES_Geom2dEntity::GetUnit() const
{
  return TheUnitFactor;
}  

