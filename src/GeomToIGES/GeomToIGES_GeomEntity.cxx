// Copyright: 	Matra-Datavision 1995
// File:	GeomToIGES_GeomEntity.cxx
// Created:	Wed Sep 13 14:29:47 1995
// Author:	Marie Jose MARTZ
//		<mjm>
//rln 06.01.98 new method SetUnit


#include <GeomToIGES_GeomEntity.ixx>

#include <IGESData_IGESModel.hxx>


//=======================================================================
//function : GeomToIGES_GeomEntity
//purpose  : 
//=======================================================================

GeomToIGES_GeomEntity::GeomToIGES_GeomEntity()
{
}


//=======================================================================
//function : GeomToIGES_GeomEntity
//purpose  : 
//=======================================================================

GeomToIGES_GeomEntity::GeomToIGES_GeomEntity
(const GeomToIGES_GeomEntity& other)
{
  TheUnitFactor = other.GetUnit();
  TheModel      = other.GetModel();
}


//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================
void GeomToIGES_GeomEntity::SetModel(const Handle(IGESData_IGESModel)& model)
{  
  TheModel = model;  
  Standard_Real unitfactor = TheModel->GlobalSection().UnitValue();
  TheUnitFactor = unitfactor;
}


//=======================================================================
//function : GetModel
//purpose  : 
//=======================================================================
Handle(IGESData_IGESModel) GeomToIGES_GeomEntity::GetModel() const
{ 
  return TheModel; 
}


//=======================================================================
//function : GetUnit
//purpose  : 
//=======================================================================
void GeomToIGES_GeomEntity::SetUnit(const Standard_Real unit)
{
  TheUnitFactor = unit;
}

//=======================================================================
//function : GetUnit
//purpose  : 
//=======================================================================
Standard_Real GeomToIGES_GeomEntity::GetUnit() const
{
  return TheUnitFactor;
}  


