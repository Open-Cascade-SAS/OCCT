// Created on: 1999-06-11
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AxisDriver.hxx>
#include <TPrsStd_ConstraintDriver.hxx>
#include <TPrsStd_DataMapOfGUIDDriver.hxx>
#include <TPrsStd_Driver.hxx>
#include <TPrsStd_DriverTable.hxx>
#include <TPrsStd_GeometryDriver.hxx>
#include <TPrsStd_NamedShapeDriver.hxx>
#include <TPrsStd_PlaneDriver.hxx>
#include <TPrsStd_PointDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_DriverTable,Standard_Transient)

static Handle(TPrsStd_DriverTable) drivertable;

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Handle(TPrsStd_DriverTable) TPrsStd_DriverTable::Get()
{
  if ( drivertable.IsNull() )
  {
    drivertable = new TPrsStd_DriverTable;
#ifdef OCCT_DEBUG
    cout << "The new TPrsStd_DriverTable was created" << endl;
#endif
  }
  return drivertable;
}

//=======================================================================
//function : TPrsStd_DriverTable
//purpose  : 
//=======================================================================

TPrsStd_DriverTable::TPrsStd_DriverTable()
{
  InitStandardDrivers();
}

//=======================================================================
//function : InitStandardDrivers
//purpose  : Adds standard drivers to the DriverTable
//=======================================================================

void TPrsStd_DriverTable::InitStandardDrivers() 
{
  if (myDrivers.Extent() > 0) return;

  Handle(TPrsStd_AxisDriver) axisdrv = new TPrsStd_AxisDriver;
  Handle(TPrsStd_ConstraintDriver) cnstrdrv = new TPrsStd_ConstraintDriver;
  Handle(TPrsStd_GeometryDriver) geomdrv = new TPrsStd_GeometryDriver ;
  Handle(TPrsStd_NamedShapeDriver) nshapedrv = new TPrsStd_NamedShapeDriver;
  Handle(TPrsStd_PlaneDriver) planedrv = new TPrsStd_PlaneDriver;
  Handle(TPrsStd_PointDriver) pointdrv = new TPrsStd_PointDriver;

  myDrivers.Bind(TDataXtd_Axis::GetID(), axisdrv);
  myDrivers.Bind(TDataXtd_Constraint::GetID(), cnstrdrv);
  myDrivers.Bind(TDataXtd_Geometry::GetID(), geomdrv);
  myDrivers.Bind(TNaming_NamedShape::GetID(), nshapedrv);
  myDrivers.Bind(TDataXtd_Plane::GetID(), planedrv);
  myDrivers.Bind(TDataXtd_Point::GetID(), pointdrv);
}

//=======================================================================
//function : AddDriver
//purpose  : Adds a driver to the DriverTable
//=======================================================================

Standard_Boolean TPrsStd_DriverTable::AddDriver(const Standard_GUID&  guid,
					        const Handle(TPrsStd_Driver)& driver)
{
  return myDrivers.Bind(guid,driver);
}

//=======================================================================
//function : FindDriver
//purpose  : Returns the driver if find
//=======================================================================

Standard_Boolean TPrsStd_DriverTable::FindDriver(const Standard_GUID& guid,
						 Handle(TPrsStd_Driver)& driver) const
{
  if (myDrivers.IsBound(guid))
  {
    driver = myDrivers.Find(guid);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : RemoveDriver
//purpose  : Removes a driver from the DriverTable
//=======================================================================

Standard_Boolean TPrsStd_DriverTable::RemoveDriver(const Standard_GUID& guid)
{
  return myDrivers.UnBind(guid);
}

//=======================================================================
//function : Clear
//purpose  : Removes all drivers
//=======================================================================

void TPrsStd_DriverTable::Clear()
{
  myDrivers.Clear();
}
