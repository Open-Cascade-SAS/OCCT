// Created on: 2001-09-26
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlMDF_ADriverTable_HeaderFile
#define _XmlMDF_ADriverTable_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlMDF_TypeADriverMap.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
class XmlMDF_ADriver;


class XmlMDF_ADriverTable;
DEFINE_STANDARD_HANDLE(XmlMDF_ADriverTable, Standard_Transient)

//! A driver table is an object building links between
//! object types and object drivers. In the
//! translation process, a driver table is asked to
//! give a translation driver for each current object
//! to be translated.
class XmlMDF_ADriverTable : public Standard_Transient
{

public:

  
  //! Creates a mutable ADriverTable from XmlMDF.
  Standard_EXPORT XmlMDF_ADriverTable();
  
  //! Sets a translation driver: <aDriver>.
  Standard_EXPORT void AddDriver (const Handle(XmlMDF_ADriver)& anHDriver);
  
  //! Gets a map of drivers.
  Standard_EXPORT const XmlMDF_TypeADriverMap& GetDrivers() const;
  
  //! Gets a driver <aDriver> according to <aType>
  //!
  //! Returns True if a driver is found; false otherwise.
  Standard_EXPORT Standard_Boolean GetDriver (const Handle(Standard_Type)& aType, Handle(XmlMDF_ADriver)& anHDriver) const;




  DEFINE_STANDARD_RTTIEXT(XmlMDF_ADriverTable,Standard_Transient)

protected:




private:


  XmlMDF_TypeADriverMap myMap;


};







#endif // _XmlMDF_ADriverTable_HeaderFile
