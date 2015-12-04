// Created on: 2001-07-25
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

#ifndef _XmlDrivers_DocumentStorageDriver_HeaderFile
#define _XmlDrivers_DocumentStorageDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlLDrivers_DocumentStorageDriver.hxx>
#include <Standard_Boolean.hxx>
#include <XmlObjMgt_Element.hxx>
class TCollection_ExtendedString;
class XmlMDF_ADriverTable;
class CDM_MessageDriver;


class XmlDrivers_DocumentStorageDriver;
DEFINE_STANDARD_HANDLE(XmlDrivers_DocumentStorageDriver, XmlLDrivers_DocumentStorageDriver)


class XmlDrivers_DocumentStorageDriver : public XmlLDrivers_DocumentStorageDriver
{

public:

  
  Standard_EXPORT XmlDrivers_DocumentStorageDriver(const TCollection_ExtendedString& theCopyright);
  
  Standard_EXPORT virtual Handle(XmlMDF_ADriverTable) AttributeDrivers (const Handle(CDM_MessageDriver)& theMsgDriver) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean WriteShapeSection (XmlObjMgt_Element& thePDoc) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XmlDrivers_DocumentStorageDriver,XmlLDrivers_DocumentStorageDriver)

protected:




private:




};







#endif // _XmlDrivers_DocumentStorageDriver_HeaderFile
