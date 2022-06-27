// Created on: 1999-08-04
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TPrsStd_DriverTable_HeaderFile
#define _TPrsStd_DriverTable_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TPrsStd_DataMapOfGUIDDriver.hxx>
#include <Standard_Transient.hxx>
class Standard_GUID;
class TPrsStd_Driver;


class TPrsStd_DriverTable;
DEFINE_STANDARD_HANDLE(TPrsStd_DriverTable, Standard_Transient)

//! This class is   a  container to record  (AddDriver)
//! binding between  GUID and  TPrsStd_Driver.
//! You create a new instance of TPrsStd_Driver
//! and use the method AddDriver to load it into the driver table. the method
class TPrsStd_DriverTable : public Standard_Transient
{

public:

  
  //! Returns the static table.
  //! If it does not exist, creates it and fills it with standard drivers.
  Standard_EXPORT static Handle(TPrsStd_DriverTable) Get();
  
  //! Default constructor
  Standard_EXPORT TPrsStd_DriverTable();
  
  //! Fills the table with standard drivers
  Standard_EXPORT void InitStandardDrivers();
  
  //! Returns true if the driver has been added successfully to the driver table.
  Standard_EXPORT Standard_Boolean AddDriver (const Standard_GUID& guid, const Handle(TPrsStd_Driver)& driver);
  
  //! Returns true if the driver was found.
  Standard_EXPORT Standard_Boolean FindDriver (const Standard_GUID& guid, Handle(TPrsStd_Driver)& driver) const;
  

  //! Removes a driver with the given GUID.
  //! Returns true if the driver has been removed successfully.
  Standard_EXPORT Standard_Boolean RemoveDriver (const Standard_GUID& guid);
  
  //! Removes all drivers. Returns
  //! true if the driver has been removed successfully.
  //! If this method is used, the InitStandardDrivers method should be
  //! called to fill the table with standard drivers.
  Standard_EXPORT void Clear();




  DEFINE_STANDARD_RTTIEXT(TPrsStd_DriverTable,Standard_Transient)

protected:




private:


  TPrsStd_DataMapOfGUIDDriver myDrivers;


};







#endif // _TPrsStd_DriverTable_HeaderFile
