// File:      XmlDrivers_DocumentStorageDriver.cxx
// Created:   Mon Jul  9 12:29:49 MSK DST 2001
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001

#include <XmlDrivers_DocumentStorageDriver.ixx>
#include <XmlDrivers.hxx>
#include <XmlMNaming_NamedShapeDriver.hxx>
#include <TNaming_NamedShape.hxx>
#include <XmlObjMgt_Element.hxx>
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
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver;
  Standard_Boolean isShape(Standard_False);
  if (myDrivers -> GetDriver (STANDARD_TYPE(TNaming_NamedShape),
                              aNamedShapeDriver)) {
    aNamedShapeDriver -> WriteShapeSection (theElement); 
    isShape = Standard_True;
  }
  return isShape;
}
