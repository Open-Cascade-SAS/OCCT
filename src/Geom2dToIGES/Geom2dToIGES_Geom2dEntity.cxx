// Copyright: 	Matra-Datavision 1995
// File:	Geom2dToIGES_Geom2dEntity.cxx
// Created:	Wed Sep 13 14:44:29 1995
// Author:	Marie Jose MARTZ
//		<mjm>
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

