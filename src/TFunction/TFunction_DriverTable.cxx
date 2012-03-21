// Created on: 1999-06-11
// Created by: Vladislav ROMASHKO
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



#include <TFunction_DriverTable.ixx>

#include <TFunction_DataMapOfGUIDDriver.hxx>
#include <TFunction_DataMapIteratorOfDataMapOfGUIDDriver.hxx>
#include <TFunction_Driver.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF.hxx>


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
