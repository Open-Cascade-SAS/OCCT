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


#include <XmlDrivers_DocumentRetrievalDriver.ixx>

#include <XmlDrivers.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMNaming.hxx>
#include <XmlObjMgt_Element.hxx>
#include <XmlMNaming_NamedShapeDriver.hxx>
#include <TNaming_NamedShape.hxx>
#include <XmlMDF_ADriver.hxx>
#include <CDM_MessageDriver.hxx>
//=======================================================================
//function : XmlDrivers_DocumentRetrievalDriver
//purpose  : Constructor
//=======================================================================
XmlDrivers_DocumentRetrievalDriver::XmlDrivers_DocumentRetrievalDriver()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable) XmlDrivers_DocumentRetrievalDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver) 
{
  return XmlDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : PropagateDocumentVersion
//purpose  : 
//=======================================================================
void XmlDrivers_DocumentRetrievalDriver::PropagateDocumentVersion(
                                   const Standard_Integer theDocVersion )
{
#ifdef DEB
//    cout << "DocCurVersion =" << theDocVersion <<endl;
#endif
  XmlMDataStd::SetDocumentVersion(theDocVersion);
  XmlMNaming::SetDocumentVersion(theDocVersion);
}

//=======================================================================
//function : ReadShapeSection
//purpose  : Implementation of ReadShapeSection
//=======================================================================
Handle(XmlMDF_ADriver) XmlDrivers_DocumentRetrievalDriver::ReadShapeSection(
                               const XmlObjMgt_Element&       theElement,
                               const Handle(CDM_MessageDriver)& theMsgDriver)
{
  if (myDrivers.IsNull()) myDrivers = AttributeDrivers (theMsgDriver);
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver;
  if (myDrivers -> GetDriver (STANDARD_TYPE(TNaming_NamedShape),
                              aNamedShapeDriver))
    aNamedShapeDriver -> ReadShapeSection (theElement);
  return aNamedShapeDriver;
}

//=======================================================================
//function : ShapeSetCleaning
//purpose  : definition of ShapeSetCleaning
//=======================================================================
void XmlDrivers_DocumentRetrievalDriver::ShapeSetCleaning(
			      const Handle(XmlMDF_ADriver)& theDriver) 
{
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver = 
    Handle(XmlMNaming_NamedShapeDriver)::DownCast(theDriver);
  if (aNamedShapeDriver.IsNull() == Standard_False)
    aNamedShapeDriver -> Clear();
}


