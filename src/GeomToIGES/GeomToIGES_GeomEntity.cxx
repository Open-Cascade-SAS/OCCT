// Created on: 1995-09-13
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//rln 06.01.98 new method SetUnit

#include <GeomToIGES_GeomEntity.hxx>
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


