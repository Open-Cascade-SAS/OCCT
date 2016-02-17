// Created on: 2001-07-09
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


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TNaming_NamedShape.hxx>
#include <XmlDrivers.hxx>
#include <XmlDrivers_DocumentStorageDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMNaming_NamedShapeDriver.hxx>
#include <XmlObjMgt_Element.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlDrivers_DocumentStorageDriver,XmlLDrivers_DocumentStorageDriver)

//=======================================================================
//function : XmlDrivers_DocumentStorageDriver
//purpose  : Constructor
//=======================================================================
XmlDrivers_DocumentStorageDriver::XmlDrivers_DocumentStorageDriver
                                (const TCollection_ExtendedString& theCopyright) :
  XmlLDrivers_DocumentStorageDriver(theCopyright)
{ 
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable) XmlDrivers_DocumentStorageDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver) 
{
  return XmlDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : WriteShapeSection
//purpose  : Implements WriteShapeSection
//=======================================================================
Standard_Boolean XmlDrivers_DocumentStorageDriver::WriteShapeSection
                                         (XmlObjMgt_Element&  theElement)
{
  Standard_Boolean isShape(Standard_False);
  Handle(XmlMDF_ADriver) aDriver;
  if (myDrivers->GetDriver (STANDARD_TYPE(TNaming_NamedShape), aDriver))
  {
    Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver = 
      Handle(XmlMNaming_NamedShapeDriver)::DownCast (aDriver);
    aNamedShapeDriver -> WriteShapeSection (theElement); 
    isShape = Standard_True;
  }
  return isShape;
}
