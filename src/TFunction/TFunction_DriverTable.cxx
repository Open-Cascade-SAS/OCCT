// Created on: 1999-06-11
// Created by: Vladislav ROMASHKO
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
#include <TCollection_ExtendedString.hxx>
#include <TDF.hxx>
#include <TFunction_DataMapIteratorOfDataMapOfGUIDDriver.hxx>
#include <TFunction_DataMapOfGUIDDriver.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TFunction_DriverTable,Standard_Transient)

static Handle(TFunction_DriverTable) DT;

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Handle(TFunction_DriverTable) TFunction_DriverTable::Get()
{
  if (DT.IsNull()) DT = new TFunction_DriverTable;
  return DT;
}

//=======================================================================
//function : TFunction_DriverTable
//purpose  : Constructor
//=======================================================================

TFunction_DriverTable::TFunction_DriverTable()
{}

//=======================================================================
//function : AddDriver
//purpose  : Adds a driver to the DriverTable
//=======================================================================

Standard_Boolean TFunction_DriverTable::AddDriver(const Standard_GUID&            guid,
						  const Handle(TFunction_Driver)& driver,
						  const Standard_Integer          thread)
{
  if (thread == 0)
    return myDrivers.Bind(guid,driver);
  else if (thread > 0) 
  {
    if (myThreadDrivers.IsNull())
    {
      // Create a new table for thread-drivers.
      myThreadDrivers = new TFunction_HArray1OfDataMapOfGUIDDriver(1, thread);
    }
    else if (myThreadDrivers->Upper() < thread)
    {
      // Create a bigger table for thread-drivers.
      Handle(TFunction_HArray1OfDataMapOfGUIDDriver) new_dt = new TFunction_HArray1OfDataMapOfGUIDDriver(1, thread);
      // Copy old table to the expanded (new) one.
      Standard_Integer i = 1, old_upper = myThreadDrivers->Upper();
      for (; i <= old_upper; i++)
      {
	const TFunction_DataMapOfGUIDDriver& t = myThreadDrivers->Value(i);
	TFunction_DataMapIteratorOfDataMapOfGUIDDriver itrt(t);
	for (; itrt.More(); itrt.Next())
	{
	  new_dt->ChangeValue(i).Bind(itrt.Key(), itrt.Value());
	}
      }//for...
      myThreadDrivers = new_dt;
    }//else...
    return myThreadDrivers->ChangeValue(thread).Bind(guid, driver);
  }
  return Standard_False;
}

//=======================================================================
//function : HasDriver
//purpose  : 
//=======================================================================

Standard_Boolean TFunction_DriverTable::HasDriver(const Standard_GUID&   guid,
						  const Standard_Integer thread) const
{
  if (thread == 0)
    return myDrivers.IsBound(guid);
  else if (thread > 0 && !myThreadDrivers.IsNull() && myThreadDrivers->Upper() >= thread)
    return myThreadDrivers->Value(thread).IsBound(guid);
  return Standard_False;
}

//=======================================================================
//function : FindDriver
//purpose  : Returns the driver if find
//=======================================================================

Standard_Boolean TFunction_DriverTable::FindDriver(const Standard_GUID&      guid,
						   Handle(TFunction_Driver)& driver,
						   const Standard_Integer    thread) const
{
  if (thread == 0)
  {
    if (myDrivers.IsBound(guid))
    {
      driver = myDrivers.Find(guid);
      return Standard_True;
    }
  }
  else if (thread > 0 && !myThreadDrivers.IsNull() && myThreadDrivers->Upper() >= thread)
  {
    if (myThreadDrivers->Value(thread).IsBound(guid))
    {
      driver = myThreadDrivers->Value(thread).Find(guid);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TFunction_DriverTable::Dump(Standard_OStream& anOS) const
{
  TFunction_DataMapIteratorOfDataMapOfGUIDDriver itr(myDrivers);
  for (; itr.More(); itr.Next())
  {
    itr.Key().ShallowDump(anOS); 
    anOS<<"\t";
    TCollection_ExtendedString es;
    TDF::ProgIDFromGUID(itr.Key(), es);
    anOS<<es<<"\n";
  }
  return anOS;
}

//=======================================================================
//function : RemoveDriver
//purpose  : Removes a driver from the DriverTable
//=======================================================================

Standard_Boolean TFunction_DriverTable::RemoveDriver(const Standard_GUID&   guid,
						     const Standard_Integer thread)
{
  if (thread == 0)
    return myDrivers.UnBind(guid);
  else if (thread > 0 && !myThreadDrivers.IsNull() && myThreadDrivers->Upper() >= thread)
    myThreadDrivers->ChangeValue(thread).UnBind(guid);
  return Standard_False;
}

//=======================================================================
//function : Clear
//purpose  : Removes all drivers
//=======================================================================

void TFunction_DriverTable::Clear()
{
  myDrivers.Clear();
  if (!myThreadDrivers.IsNull())
    myThreadDrivers.Nullify();
}
