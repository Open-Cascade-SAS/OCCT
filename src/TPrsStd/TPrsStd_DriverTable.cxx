// Created on: 1999-06-11
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
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



#include <TPrsStd_DriverTable.ixx>

#include <TPrsStd_DataMapOfGUIDDriver.hxx>

#include <TPrsStd_Driver.hxx>
#include <TPrsStd_AxisDriver.hxx>
#include <TPrsStd_ConstraintDriver.hxx>
#include <TPrsStd_GeometryDriver.hxx>
#include <TPrsStd_NamedShapeDriver.hxx>
#include <TPrsStd_PlaneDriver.hxx>
#include <TPrsStd_PointDriver.hxx>

#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>


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
#ifdef DEB
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
