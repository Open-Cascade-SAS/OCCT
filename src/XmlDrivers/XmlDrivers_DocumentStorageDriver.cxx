// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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
